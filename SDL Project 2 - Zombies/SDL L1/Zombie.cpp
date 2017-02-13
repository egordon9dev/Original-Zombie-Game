#include "stdafx.h"
#include <SDL/SDL.h>
#include "Zombie.h"
#include <ctime>
#include "Texture.h"
#include "iostream"
#include <vector>

//player spawn location
extern const int nPlayerSpawnX;
extern const int nPlayerSpawnY;

//zombie dimensions
extern const int nZombieW;
extern const int nZombieH;
extern const int nZombieR;

//range in which zombies do not spawn
extern const int nZombieSpawnRangeX;
extern const int nZombieSpawnRangeY;

//screen dimensions
extern const int nScreenW;
extern const int nScreenH;

//tile dimensions
extern const int nTileW;
extern const int nTileH;

//player dimensions
extern const int nPlayerH;
extern const int nPlayerW;

//map data access function
extern const int nMapH;
extern const int nMapW;
//extern int GetMapData(int x, int y);

//tile constants
extern const int TILE_brick;
extern const int TILE_unbreakable_wall;
extern const int TILE_wall;

//function that handles rectangular collisions angles at 45 degree increments (e.g. down, down_left, left, up_left, up ...)
void HandleCollisions(std::vector<std::vector<int>> vMapData, SDL_Rect &box, int &xVel, int &yVel, Direction &eDir, int nPlayerW, int nPlayerH);

//blood texture
extern Texture cBlood_texture;

//direction enum
enum class Direction
{
	up,
	up_left,
	left,
	down_left,
	down,
	down_right,
	right,
	up_right,
	uninitialized
};

//speed zombies walk,run, or sprint at
extern const int nZombieWalkVel;
extern const int nZombieRunVel;
extern const int nZombieSprintVel;

extern bool IsWallTile(int n);

//zombie texture
extern Texture cZombie_texture;

//renderer
extern SDL_Renderer gRenderer;

extern bool CheckCollision(SDL_Rect box, Circle circle);

Zombie::Zombie(std::vector<std::vector<int>> MapData)
{
	using namespace std;

	m_Box.w = nZombieW;
	m_Box.h = nZombieH;
	m_Circle.r = nZombieR;
	m_Circle.d = 2 * nZombieR;

	m_AttractionPoint.x = nScreenW / 2;
	m_AttractionPoint.y = nScreenH / 2;
	
	char success = -1;

	int spawnX = 0, spawnY = 0;
	while (success != 1)
	{
		success = -1;

		srand(time(0));
		spawnX = rand() % nScreenW;
		spawnY = rand() % nScreenH;

		int y = 0;
		for (vector<vector<int>>::iterator itMapY = MapData.begin(); itMapY != MapData.end(); ++itMapY, ++y)
		{
			int x = 0;
			//cout << "\n\n" << (*itMapY).size() << "\n\n";
			for (vector<int>::iterator itMapX = (*itMapY).begin(); itMapX != (*itMapY).end(); ++itMapX, ++x)
			{
				//int nCurrentTileType = (*itMapX);
				if (x == spawnX/nTileW && y == spawnY/nTileH)
				{
					if (IsWallTile(*itMapX))
					{
						success = 0;
					}
					else
					{
						success = 1;
					}
				}

				if (success != -1)
					break;
			}
			if (success != -1)
				break;
		}
	}
	m_Circle.x = spawnX;
	m_Circle.y = spawnY;
}

void Zombie::Move(std::vector<std::vector<int>> vMapData, SDL_Rect PlayerBox, SDL_Point AttractionPoint)
{
	using namespace std;

	m_AttractionPoint = AttractionPoint;

	Direction ZombieXDir = Direction::uninitialized;
	Direction ZombieYDir = Direction::uninitialized;

	int xVel = 0;
	int yVel = 0;

	//if m_Box is not within 5 pixels of m_AttrectionPoint
	if (!(/* x */m_Circle.x >= m_AttractionPoint.x - 5 && m_Circle.x <= m_AttractionPoint.x + 5 /* y */ && m_Circle.y >= m_AttractionPoint.y - 5 && m_Circle.y <= m_AttractionPoint.y + 5))
	{
		//move toward attraction point on the x-axis
		if (m_Circle.x < m_AttractionPoint.x)
		{
			m_Circle.x += nZombieWalkVel;
			xVel = nZombieWalkVel;
		}
		if (m_Circle.x > m_AttractionPoint.x)
		{
			m_Circle.x -= nZombieWalkVel;
			xVel = -nZombieWalkVel;
		}

		//move toward attraction point on the y-axis
		if (m_Circle.y < m_AttractionPoint.y)
		{
			m_Circle.y += nZombieWalkVel;
			yVel = nZombieWalkVel;
		}
		if (m_Circle.y > m_AttractionPoint.y)
		{
			m_Circle.y -= nZombieWalkVel;
			yVel = -nZombieWalkVel;
		}
	}

	Direction eZombieDir;

	if (xVel > 0 && yVel > 0)
		eZombieDir = Direction::down_right;
	if (xVel > 0 && yVel < 0)
		eZombieDir = Direction::up_right;
	if (xVel < 0 && yVel > 0)
		eZombieDir = Direction::down_left;
	if (xVel < 0 && yVel < 0)
		eZombieDir = Direction::up_left;
	if (xVel < 0 && yVel == 0)
		eZombieDir = Direction::left;
	if (xVel > 0 && yVel == 0)
		eZombieDir = Direction::right;
	if (xVel == 0 && yVel < 0)
		eZombieDir = Direction::up;
	if (xVel == 0 && yVel > 0)
		eZombieDir = Direction::down;

	SDL_Rect ZombieBox = { m_Circle.x - m_Circle.r, m_Circle.y - m_Circle.r, m_Circle.r * 2, m_Circle.r * 2 };
	
	HandleCollisions(vMapData, ZombieBox, xVel, yVel, eZombieDir, m_Circle.r * 2, m_Circle.r * 2);
	Entity::Move();

	//update circle
	m_Circle.x = ZombieBox.x + m_Circle.r;
	m_Circle.y = ZombieBox.y + m_Circle.r;
}

void Zombie::Render(SDL_Renderer* renderer)
{
	SDL_Rect clip = { 0, 0, nZombieW, nZombieH };

	SDL_Rect drect = { m_Circle.x - m_Circle.r, m_Circle.y - m_Circle.r, m_Box.w, m_Box.h };

	cZombie_texture.Render(renderer, clip, drect, 0.0);
}