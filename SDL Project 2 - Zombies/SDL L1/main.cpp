#include "stdafx.h"
#include <iostream>
#include <ctime> //for time function
#include <cmath> //for pow function (exponents)
#include <vector> //for vector type
#include <iterator> // for std::next function

//SDL
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


//class header files
#include "Texture.h"
#include "Zombie.h"
#include "Player.h"
#include "fstream"
#include "Tile.h"
#include "Bullet.h"
#include "Blood.h"

//screen dimensions
//extern because constants are implicately static
extern const int nScreenW = 1040;
extern const int nScreenH = 640;

//Tile Dimensions
extern const int nTileW = 80;
extern const int nTileH = 80;

//dimensions of the area that is generated
extern const int nGeneratedW = nScreenW + (4 * nTileW);
extern const int nGeneratedH = nScreenH + (4 * nTileH);

//bullet dimensions
extern const int nBulletW = 9;
extern const int nBulletH = 20;

//bullet velocity
extern const int nBulletVel = 40;

std::vector<std::vector<int>> nLoadedMap;

extern const int nRenderDistanceX = nScreenW / nTileW;
extern const int nRenderDistanceY = nScreenH / nTileH;

//Player dimensions
extern const int nPlayerW = 43;
extern const int nPlayerH = 43;

extern const int nZombieSpawnRangeX = double(nScreenW) * (2 / 3);
extern const int nZombieSpawnRangeY = double(nScreenH) * (2 / 3);

//number of frames in the player animation
const int nPlayerAnimationFrames = 5;

//speed of car animation
// 60 / nCarAnimationSpeed = the frames per second of the car animation
const double nCarAnimationSpeed = 1;

//zombie dimensions
extern const int nZombieW = 50;
extern const int nZombieH = 50;
extern const int nZombieR = 25;

//dimensions of zombie blood splatter
extern const int nBloodW = 50;
extern const int nBloodH = 50;

//number of zombies spawned per frame
const int nZombieSpawnRate = 1;

//number of zombies on the screen at a time
//must be at least 2
const int nZombieSpawnNum = 20;

//speed zombies walk,run, or sprint at
extern const int nZombieWalkVel = 2;

//player velocity (has to be less than both [nTileW / 2] and [nTileH / 2])
extern const int nPlayerVel = 1;

//acceleration rate of the player/car
extern const int nPlayerAccelRate = 5;

extern const unsigned char MaxPlayerHealth = 200;
const unsigned char ZombieDamage = 1;

//player spawn loction
extern const int nPlayerSpawnX = 200;// (nScreenW / 2)*nTileW;
extern const int nPlayerSpawnY = 200;// (nScreenH / 2)*nTileH;

//Window
SDL_Window* gWindow;

//Renderer
SDL_Renderer* gRenderer;

//tile sprites texture
Texture cTileSprites_texture;

//holds all of the car sprites side-by-side in one texture
Texture cCarSprites_texture;

//bullet texture
Texture cBullet_texture;

//zombie textures
Texture cZombie_texture;
Texture cBlood_texture;

//corresponding numbers for each tile
extern const int TILE_brick = 0;
extern const int TILE_wall = 1;
extern const int TILE_unbreakable_wall = 2;

bool IsWallTile(int nTile){ return (nTile == 1 || nTile == 2); }

bool Init()
{
	using namespace std;

	bool success = true;

	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL Error: " << SDL_GetError() << "failed to initialize SDL" << endl;
		success = false;
	}
	else
	{
		//enable linear texture filtering
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			cout << "Warning: Linear filtering is not enabled" << endl;
		}
		//Create window
		gWindow = SDL_CreateWindow("ZOMBIES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, nScreenW, nScreenH, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			cout << "SDL Error: " << SDL_GetError() << "couldn't create window" << endl;
			success = false;
		}
		else
		{
			//create vsynced, accelerated renderer for gWindow
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				cout << "SDL Error: " << SDL_GetError() << "; couldn't create renderer" << endl;
				success = false;
			}
			else
			{
				//set render draw color to opaque white
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					cout << "SDL_Image could not be initialized, SDL_Image Error: " << IMG_GetError() << endl;
					success = false;
				}
			}
		}
	}
	return success;
}

float CosineInterpolate(float a, float b, float x)
{
	float f = (1 - cos(x * M_PI)) * .5;

	return (a * (1 - f)) + b * f;
}

float Noise(int nOctave, int x, int y)
{
	if (nOctave == 1)
	{
		int n = x + y * 57;
		n = (n << 13) ^ n;
		return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
	}
	if (nOctave == 2)
	{
		int n = x + y * 13;
		n = (n << 17) ^ n;
		return (1.0 - ((n * (n * n * 6113 + 8629) + 4599271) & 0x11fffffff) / 2927419459.0);
	}
	if (nOctave == 3)
	{
		int n = x + y * 17;
		n = (n << 11) ^ n;
		return (1.0 - ((n * (n * n * 6961 + 2713) + 1456703) & 0x17fffffff) / 39801473.0);
	}
	return -99;
}

float SmoothNoise(int nOctave, float x, float y)
{
	float corners = (Noise(nOctave, x - 1, y - 1) + Noise(nOctave, x + 1, y - 1) + Noise(nOctave, x - 1, y + 1) + Noise(nOctave, x + 1, y + 1)) / 16;
	float sides = (Noise(nOctave, x - 1, y) + Noise(nOctave, x + 1, y) + Noise(nOctave, x, y - 1) + Noise(nOctave, x, y + 1)) / 8;
	float center = Noise(nOctave, x, y) / 4;
	return corners + sides + center;

}

float InterpolatedNoise(int nOctave, float x, float y)
{
	int integer_X = int(x);
	float fractional_X = x - integer_X;

	int integer_Y = int(y);
	float fractional_Y = y - integer_Y;

	float v1 = SmoothNoise(nOctave, integer_X, integer_Y);
	float v2 = SmoothNoise(nOctave, integer_X + 1, integer_Y);
	float v3 = SmoothNoise(nOctave, integer_X, integer_Y + 1);
	float v4 = SmoothNoise(nOctave, integer_X + 1, integer_Y + 1);

	float i1 = CosineInterpolate(v1, v2, fractional_X);
	float i2 = CosineInterpolate(v3, v4, fractional_X);

	return CosineInterpolate(i1, i2, fractional_Y);
}

float GenerateNoise_2D(float x, float y)
{
	float total = 0;
	int p = 2;
	int n = 2; //Number_Of_Octaves - 1

	for (int i = 0; i < n; ++i)
	{
		int frequency = pow(2, i);
		int amplitude = pow(p, i);

		total = total + (InterpolatedNoise(i+1, x * frequency, y * frequency) * amplitude);
	}
	return total;
}

int SeedMapGenerator()
{
	using namespace std;
	cout << "select one: " << endl << "A: continue a saved game \t or \t B: begin a new game" << endl;
	char chGameChoice;
	
	// repeat untill they give valid input
	do
	{
		cin >> chGameChoice;
		if (chGameChoice != 'a' && chGameChoice != 'b' && chGameChoice != 'A' && chGameChoice != 'B')
		{
			cout << "you entered invalid input try again" << endl;
		}
	}while (chGameChoice != 'a' && chGameChoice != 'b' && chGameChoice != 'A' && chGameChoice != 'B'); 
	
	//seed for the map
	int seed;

	if (chGameChoice == 'b' || chGameChoice == 'B')
	{
		cout << "Do you want to A: enter a seed or B: generate a random seed";
			
		char chSeedChoice;

		// repeat untill the user enters a valid choice
		do
		{
			//get user's choice
			cin >> chSeedChoice;
			//if the user didn't enter a valid choice
			if (chSeedChoice != 'a' && chSeedChoice != 'A' && chSeedChoice != 'b' && chSeedChoice != 'B')
			{
				cout << "you entered invalid input, try again" << endl;
			}
		}while (chSeedChoice != 'a' && chSeedChoice != 'A' && chSeedChoice != 'b' && chSeedChoice != 'B');


		//if user chose to enter a seed
		if (chSeedChoice == 'a' || chSeedChoice == 'A')
		{
			cout << "enter a whole number between -3.4 billion and 3.4 billion" << endl;
			cin >> seed;
		}
		//if user chose to generate a random seed
		if (chSeedChoice == 'b' || chSeedChoice == 'B')
		{
			seed = (int)time(0) * (int)time(0) * 2 / 2 + (int)time(0) - 27;
		}
	}
	if (chGameChoice == 'a' || chGameChoice == 'A')
	{
		ifstream ifsMap("Map.txt");
		ifsMap >> seed;
		ifsMap.close();
	}

	return seed;
}

std::vector<std::vector<int>> GenerateMap(int seed, SDL_Rect PlayerBox, SDL_Renderer* renderer)
{
	using namespace std;

	std::vector<std::vector<int>> MapData;
	int tileType = TILE_brick;
	
	int firstXVal = (PlayerBox.x + (PlayerBox.w / 2) - (nScreenW / 2) - nTileW) / nTileW;
	int firstYVal = (PlayerBox.y + (PlayerBox.h / 2) - (nScreenH / 2) - nTileH) / nTileH;
	int lastXVal = firstXVal + nScreenW/nTileW;
	int lastYVal = firstYVal + nScreenH/nTileH;

	printf("%i, %i\n", firstXVal, firstYVal);
	printf("%i, %i\n\n", lastXVal, lastYVal);

	for (int noiseY = firstYVal; noiseY <= lastYVal; ++noiseY)
	{
		vector<int> newRow;
		MapData.push_back(newRow);

		for (int noiseX = firstXVal; noiseX <= lastXVal; ++noiseX)
		{
			//Generate noise
			float fNoiseVal = GenerateNoise_2D(noiseX, noiseY);
			
			if (fNoiseVal <= 0)
				tileType = TILE_wall;
			else
				tileType = TILE_brick;

			//create a tile from the current integer from the map
			Tile cTile({ (noiseX * nTileW) + (PlayerBox.x % nTileW), (noiseY * nTileH) + (PlayerBox.y % nTileH), nTileW, nTileH }, tileType);
			MapData[noiseY - firstYVal].push_back(tileType);
			cTile.Render(renderer);
		}
	}

	return MapData;
}
		
int GetMapSeed(std::string path)
{
	using namespace std;
	//success flag
	bool success = true;

	//create an input file stream that reads our map
	ifstream inf(path);

	//if the input file stream couldn't be opened for reading
	if (!inf)
	{
		cout << "Error: Map file couldn't be opened for reading" << endl;
		success = false;
	}
	int seed;
	inf >> seed;

	//close the file
	inf.close();

	//return the success flag
	return success;
}


//load images used later in the program
bool LoadMedia()
{
	using namespace std;

	bool success = true;

	//load bullet.png
	if (!cBullet_texture.LoadFromFile(gRenderer, "lasers.png"))
	{
		//print error message if loading failed
		cout << "failed to load Bullet.png, SDL Error: " << SDL_GetError() << endl;
		success = false;
	}
	
	//load Car.png
	if (!cCarSprites_texture.LoadFromFile(gRenderer, "alien.png"))
	{
		//print error message if loading failed
		cout << "failed to load Humvee.png, SDL Error: " << SDL_GetError() << endl;
		success = false;
	}

	//load Tiles.png
	if (!cTileSprites_texture.LoadFromFile(gRenderer, "Tiles.png"))
	{
		//print error message if loaded
		cout << "failed to load Tiles.png, SDL Error: " << SDL_GetError() << endl;
		success = false;
	}

	//load Zombie.png
	if (!cZombie_texture.LoadFromFile(gRenderer, "Zombie.png"))
	{
		//print error message if loaded
		cout << "failed to load Zombie.png, SDL Error: " << SDL_GetError() << endl;
		success = false;
	}

	//load blood splatter.png
	if (!cBlood_texture.LoadFromFile(gRenderer, "blood splatter.png"))
	{
		//print error message if loaded
		cout << "failed to load Zombie.png, SDL Error: " << SDL_GetError() << endl;
		success = false;
	}

	return success;
}

//box with box
bool CheckCollision(SDL_Rect a, SDL_Rect b)
{
	//if a is to the left of b
	if (a.x + a.w < b.x)
		return false;

	//if a is to the right of b
	if (a.x > b.x + b.w)
		return false;

	//if a is above b
	if (a.y + a.h < b.y)
		return false;

	//if a is below b
	if (a.y > b.y + b.h)
		return false;

	//if a is not above, below, to the right of, or to the left of b, return true
	return true;
}

//box with circle
bool CheckCollision(SDL_Rect box, Circle circle)
{
	const int BOX_T = box.y;
	const int BOX_B = box.y + box.h;
	const int BOX_L = box.x;
	const int BOX_R = box.x + box.w;

	const int CIRCLE_T = circle.y - circle.r;
	const int CIRCLE_B = circle.y + circle.r;
	const int CIRCLE_L = circle.x - circle.r;
	const int CIRCLE_R = circle.x + circle.r;

	//circle is to the left of box
	if (CIRCLE_R < BOX_L)
		return false;
	//circle is to the right of box
	if (BOX_R < CIRCLE_L)
		return false;
	//circle is below box
	if (CIRCLE_T > BOX_B)
		return false;
	//circle is above box
	if (BOX_T > CIRCLE_B)
		return false;

	//if the circle is not above, below, to the right of, or to the left of the box, return true
	return true;
}


void UpdateMap(int seed)
{
	using namespace std;
	ofstream ofsMap("Map.txt", ofstream::trunc);
	ofsMap << seed;
	ofsMap.close();
}


//cleanup
void Close()
{
	//free textures
	cCarSprites_texture.Free();
	cBullet_texture.Free();
	cTileSprites_texture.Free();
	cZombie_texture.Free();

	//destroy and dealocate window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//destroy and dealocate renderer
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	//quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char** argv)
{
	char bRunAgain = 'y';
	while (bRunAgain == 'y' || bRunAgain == 'Y')
	{
		const int SEED = SeedMapGenerator();

		using namespace std;

		if (!Init())
		{
			cout << "failed to initialize SDL, SDL Error: " << SDL_GetError() << endl;
			return -1;
		}
	
		else if (!LoadMedia())
		{
			cout << "failed to load media, SDL Error:" << SDL_GetError << endl;
			return -2;
		}

		else
		{
			//true if the user clicks the "x" at the top right of the window
			bool quit = false;

			//event variable where all events are stored
			SDL_Event e;

			//the number of frames one frame of the car animation has been on screen
			int nCount = 1;

			//frame of the animation for the car/player
			int nCarFrame = 1;

			//direction player is facing
			Direction ePlayerDirection = Direction::down;

			//previous direction of the car
			Direction ePreviousPlayerDir = Direction::uninitialized;

			//player
			Player cPlayer(nPlayerSpawnX, nPlayerSpawnY);

			//key states
			Direction KStateW = Direction::up;
			Direction KStateA = Direction::up;
			Direction KStateS = Direction::up;
			Direction KStateD = Direction::up;

			//camera
			//SDL_Rect camera = { 0, 0, nScreenW, nScreenH };

			bool bShootBullet = false;

			//holds all of the bullets
			vector<Bullet*> *vcBullets = new vector<Bullet*>;

			//holds all of the zombies
			vector<Zombie*> *vcZombies = new vector<Zombie*>;

			//holds the blood
			vector<Blood> *vcBlood = new vector<Blood>;

			//number of bullets on screen
			int nBulletsOnScreen = 0;
		
			//flag that tells whether or not the player is stopped
			bool bIsPlayerStopped = true;

			srand((int)time(0));

			bool shoot = false;

			while (!quit)
			{
				//clear renderer
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
				SDL_RenderClear(gRenderer);


				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					if (e.type == SDL_KEYDOWN)
					{
						//up
						if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_UP)
						{
							KStateW = Direction::down;
						}
						//left
						if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_LEFT)
						{
							KStateA = Direction::down;
						}
						//down
						if (e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN)
						{
							KStateS = Direction::down;
						}
						//right
						if (e.key.keysym.sym == SDLK_d || e.key.keysym.sym == SDLK_RIGHT)
						{
							KStateD = Direction::down;
						}
						if (e.key.keysym.sym == SDLK_KP_5 || e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN)
						{
							bShootBullet = true;
						}/*
						if (e.key.keysym.sym == SDLK_KP_1)
						{
							for (vector<Zombie*>::iterator itZombies = vcZombies->begin(); itZombies != vcZombies->end();)
							{
								Zombie* cCurrentZombie = (*itZombies);

								//set blood dimensions
								Circle ZombieCircle = cCurrentZombie->GetCircle();
								SDL_Rect BloodBox = { ZombieCircle.x - ZombieCircle.r, ZombieCircle.y - ZombieCircle.r, nBloodW, nBloodH };

								//kill zombie and put a blood splatter where it died
								vcBlood->emplace_back(BloodBox);

								if (cCurrentZombie != vcZombies->at(vcZombies->size() - 1))
								{
									itZombies = vcZombies->erase(itZombies);
								}
								else
								{
									vcZombies->erase(itZombies);
									break;
								}
							}
						}*/
					}

					//change the players velocity when the user lets go of w, a, s, or d
					if (e.type == SDL_KEYUP)
					{
						if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_UP)
						{
							//set w key as "down"
							KStateW = Direction::up;
						}
						if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_LEFT)
						{
							//set a key as "down"
							KStateA = Direction::up;
						}
						if (e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN)
						{
							//set s key as "down"
							KStateS = Direction::up;
						}
						if (e.key.keysym.sym == SDLK_d || e.key.keysym.sym == SDLK_RIGHT)
						{
							//set d key as "down"
							KStateD = Direction::up;
						}
					}
				}


	//---------------------------------------------------------  Determine the direction of the car  ---------------------------------------------------------//

				//up & left
				if (KStateW == Direction::down && KStateA == Direction::down && KStateS == Direction::up && KStateD == Direction::up)
				{
					ePlayerDirection = Direction::up_left;
				}
				//down & left
				if (KStateW == Direction::up && KStateA == Direction::down && KStateS == Direction::down && KStateD == Direction::up)
				{
					ePlayerDirection = Direction::down_left;
				}
				//down & right
				if (KStateW == Direction::up && KStateA == Direction::up && KStateS == Direction::down && KStateD == Direction::down)
				{
					ePlayerDirection = Direction::down_right;
				}
				//up & right
				if (KStateW == Direction::down && KStateA == Direction::up && KStateS == Direction::up && KStateD == Direction::down)
				{
					ePlayerDirection = Direction::up_right;
				}

				//Not diagonal
				//up
				if (KStateW == Direction::down && KStateA == Direction::up && KStateS == Direction::up && KStateD == Direction::up)
				{
					ePlayerDirection = Direction::up;
				}
				//left
				if (KStateW == Direction::up && KStateA == Direction::down && KStateS == Direction::up && KStateD == Direction::up)
				{
					ePlayerDirection = Direction::left;
				}
				//down
				if (KStateW == Direction::up && KStateA == Direction::up && KStateS == Direction::down && KStateD == Direction::up)
				{
					ePlayerDirection = Direction::down;
				}
				//right
				if (KStateW == Direction::up && KStateA == Direction::up && KStateS == Direction::up && KStateD == Direction::down)
				{
					ePlayerDirection = Direction::right;
				}

				//no direction:
				//if no keys are pressed
				if (KStateW == Direction::up && KStateA == Direction::up && KStateS == Direction::up && KStateD == Direction::up)
				{
					//stop the player
					cPlayer.SetVel(0, 0);

					//set player stopped flag
					bIsPlayerStopped = true;
				}
				else
					bIsPlayerStopped = false;

				//------------------------------------------------------------------------------------------------------------------------------------------------------------//

			
				//Generate the map
				nLoadedMap = GenerateMap(SEED, cPlayer.GetBox(), gRenderer);

				//move the player
				cPlayer.Move(nLoadedMap, ePlayerDirection, ePreviousPlayerDir, bIsPlayerStopped);

				//-----------------------------------------------------------  Handle Bullet Shooting  -----------------------------------------------------------------------//
				//shoot bullet?
				if (bShootBullet)
				{
					// Non Diagonal
					//up
					if (ePlayerDirection == Direction::up)
					{
						vcBullets->emplace_back(new Bullet(cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2), 0, -nBulletVel));
						++nBulletsOnScreen;
					}
					//left
					if (ePlayerDirection == Direction::left)
					{
						vcBullets->emplace_back(new Bullet(cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2), -nBulletVel, 0));
						++nBulletsOnScreen;
					}
					//down
					if (ePlayerDirection == Direction::down)
					{
						vcBullets->emplace_back(new Bullet(cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2), 0, nBulletVel));
						++nBulletsOnScreen;
					}
					//right
					if (ePlayerDirection == Direction::right)
					{
						vcBullets->emplace_back(new Bullet(cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2), nBulletVel, 0));
						++nBulletsOnScreen;
					}

					// Diagonal

					//up and left
					if (ePlayerDirection == Direction::up_left)
					{
						vcBullets->emplace_back(new Bullet(cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2), -nBulletVel, -nBulletVel));
						++nBulletsOnScreen;
					}
					//down and left
					if (ePlayerDirection == Direction::down_left)
					{
						vcBullets->emplace_back(new Bullet(cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2), -nBulletVel, nBulletVel));
						++nBulletsOnScreen;
					}
					//down and right
					if (ePlayerDirection == Direction::down_right)
					{
						vcBullets->emplace_back(new Bullet(cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2), nBulletVel, nBulletVel));
						++nBulletsOnScreen;
					}
					//up and right
					if (ePlayerDirection == Direction::up_right)
					{
						vcBullets->emplace_back(new Bullet(cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2), nBulletVel, -nBulletVel));
						++nBulletsOnScreen;
					}

					//delete bullets that are despawned
					for (vector<Bullet*>::iterator itBullets = vcBullets->begin(); itBullets != vcBullets->end(); )
					{
						Bullet* &cCurrentBullet = *itBullets;
						if (cCurrentBullet->isNotSpawned())
						{
							if (cCurrentBullet != vcBullets->at(vcBullets->size() - 1))
							{
								itBullets = vcBullets->erase(itBullets);
								continue;
							}
							else
							{
								vcBullets->erase(itBullets);
								break;
							}
						}
						++itBullets;
					}

				}
				//------------------------------------------------------------------------------------------------------------------------------------------------------------//
				
				// ~~~~~~~~~~ Handle blood splatters ~~~~~~~~~~ //

				for (vector<Blood>::iterator itBlood = vcBlood->begin(); itBlood != vcBlood->end();)
				{
					Blood* cCurrentBlood = &*itBlood;
				
					if (cCurrentBlood->GetCount() <= 100)
					{
						cCurrentBlood->Render(gRenderer);
						cCurrentBlood->IncrementCount();
						itBlood++;
					}
					else if (cCurrentBlood != &vcBlood->at(vcBlood->size() - 1))
					{
						itBlood = vcBlood->erase(itBlood);
					}
					else
					{
						vcBlood->erase(itBlood);
						break;
					}
				}

				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

				//render player
				cPlayer.Render(gRenderer, ePlayerDirection, nCarFrame);

				for (vector<Bullet*>::iterator itBullets = vcBullets->begin(); itBullets != vcBullets->end(); ++itBullets)
				{
					Bullet* &cCurrentBullet = *itBullets;

					cCurrentBullet->Move(nLoadedMap);
					cCurrentBullet->Render();
				}

				//==========================================================<< Handle Zombie Spawning >>=============================================================\\*
				
				for (int iii = 0; iii < (nZombieSpawnRate - vcZombies->size()); ++iii)
				{
					vcZombies->emplace_back( new Zombie(nLoadedMap));
				}

				bool bBreakInBulletLoop = false;

				bool bBreakInZombieLoop = false;
				bool bContinueInZombieLoop = false;

				Zombie* &cCurrentZombie = (*(vcZombies->begin()));

				for (vector<Zombie*>::iterator itZombies = vcZombies->begin(); itZombies != vcZombies->end(); )
				{
					//reference to current zombie
					cCurrentZombie = (*itZombies);

					bBreakInZombieLoop = false;
					bContinueInZombieLoop = false;

					//set attraction point to the center of the car
					SDL_Point AttractionPoint { cPlayer.GetBox().x + (cPlayer.GetBox().w / 2), cPlayer.GetBox().y + (cPlayer.GetBox().h / 2) };

					//move zombie
					cCurrentZombie->Move(nLoadedMap, cPlayer.GetBox(), AttractionPoint);

					//despawn all zombies that are off the screen
					if (cCurrentZombie->GetCircle().x > nScreenW || cCurrentZombie->GetCircle().x < 0 || cCurrentZombie->GetCircle().y > nScreenH || cCurrentZombie->GetCircle().y < 0)
					{
						if (cCurrentZombie != vcZombies->at(vcZombies->size() - 1))
						{
							//move to the next zombie after the current one is deleted
							itZombies = vcZombies->erase(itZombies);
						}
						else
						{
							vcZombies->erase(itZombies);
							break;
						}

						continue;
					}

					//affect player's health if a zombie touches them
					if (CheckCollision(cPlayer.GetBox(), (cCurrentZombie)->GetCircle()))
					{
						cPlayer.SetHealth(cPlayer.GetHealth() - ZombieDamage);
					}

					//render zombie
					(cCurrentZombie)->Render(gRenderer/*, camera*/);

					for (vector<Bullet*>::iterator itBullets = vcBullets->begin(); itBullets != vcBullets->end(); )
					{
						//current bullet
						Bullet* cCurrentBullet = (*itBullets);

						bBreakInBulletLoop = false;

						//despawn all bullets that are off the screen
						if (cCurrentBullet->GetBox().x > nScreenW || cCurrentBullet->GetBox().x < 0|| cCurrentBullet->GetBox().y > nScreenH || cCurrentBullet->GetBox().y < 0)
						{
							if (cCurrentBullet == vcBullets->at(vcBullets->size() - 1))
							{
								vcBullets->erase(itBullets);
								break;
							}
							else
							{
								//move to the next zombie after the current one is deleted
								itBullets = vcBullets->erase(itBullets);
							}

							continue;
						}
					
						//if a bullet hits a zombie
						if (CheckCollision((cCurrentBullet->GetBox()), cCurrentZombie->GetCircle()))
						{
							if (cCurrentBullet != vcBullets->at(vcBullets->size() - 1))
							{
								//move to the next zombie after the current one is deleted
								itBullets = vcBullets->erase(itBullets);
							}
							else
							{
								vcBullets->erase(itBullets);
								bBreakInBulletLoop = true;
							}
						
							//set blood dimensions
							Circle ZombieCircle = cCurrentZombie->GetCircle();
							SDL_Rect BloodBox = { ZombieCircle.x - ZombieCircle.r, ZombieCircle.y - ZombieCircle.r, nBloodW, nBloodH };

							//kill zombie and put a blood splatter where it died
							vcBlood->emplace_back(BloodBox);
							
							//   |      |      |      |      |      |      |      |      |      | 
							//  \ /    \ /    \ /    \ /    \ /    \ /    \ /    \ /    \ /    \ /
							//   '      '      '      '      '      '      '      '      '      '
							if (!bBreakInZombieLoop && !bContinueInZombieLoop) //make sure that itZombies is a valid iterator 
							{
								if (cCurrentZombie != vcZombies->at(vcZombies->size() - 1))
								{
									//move to the next zombie after the current one is deleted
									itZombies = vcZombies->erase(itZombies);
									bContinueInZombieLoop = true;
								}
								else
								{
									vcZombies->erase(itZombies);
									bBreakInZombieLoop = true;
								}
							}

							if (bBreakInBulletLoop)
								break;
							else
								continue;
						}

						++itBullets;
					}

					if (bBreakInZombieLoop)
						break;
					else if (bContinueInZombieLoop)
						continue;
					else
						++itZombies;
				}
				//===================================================================================================================================================\\

				//update renderer
				SDL_RenderPresent(gRenderer);

				//increment the count if the player is not stopped
				if (!bIsPlayerStopped)
					++nCount;
				if (nCount > nCarAnimationSpeed)
				{
					//loop nCount
					nCount = 0;

					//increment the car animation frame if the player is not stopped
					if (!bIsPlayerStopped)
						++nCarFrame;

					//loop the car animation frame if it's over 4
					if (nCarFrame > nPlayerAnimationFrames)
					{
						nCarFrame = 1;
					}
				}

				//set the previous player direction
				ePreviousPlayerDir = ePlayerDirection;

				//reset bullet shooting flag
				bShootBullet = false;
				/*
				if (cPlayer.GetHealth() <= 0)
					quit = true;
				*/	
			}

			UpdateMap(SEED);

			//clear zombie list
			for (vector<Zombie*>::iterator it = vcZombies->begin(); it != vcZombies->end(); it)
			{
				Zombie* cCurrentZombie = (*it);
				if (cCurrentZombie != vcZombies->at(vcZombies->size() - 1))
				{
					it = vcZombies->erase(it);
				}
				else
				{
					vcZombies->erase(it);
					break;
				}
			}
			for (unsigned int iii = 0; iii < vcZombies->size(); ++iii)
			{
				delete (vcZombies->at(iii));
			}

			//clear bullet list
			for (vector<Bullet*>::iterator it = vcBullets->begin(); it != vcBullets->end(); it)
			{
				Bullet* cCurrentBullet = (*it);
				if (cCurrentBullet != vcBullets->at(vcBullets->size() - 1))
				{
					it = vcBullets->erase(it);
				}
				else
				{
					vcBullets->erase(it);
					break;
				}
			}
			for (unsigned int iii = 0; iii < vcBullets->size(); ++iii)
			{
				delete (vcBullets->at(iii));
			}

			delete vcZombies;
			delete vcBullets;
			delete vcBlood;
		}

		//free recources and quit SDL subsystems
		Close();

		cout << "Do you want to run again" << endl;
		cin >> bRunAgain;
	}
	return 0;
}