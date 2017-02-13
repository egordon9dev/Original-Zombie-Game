#include "stdafx.h"
#include "Player.h"
#include "SDL/SDL.h"
#include "Texture.h"
#include <iostream>
#include <cmath> //for sqrt function
#include <vector>

//forward declaration of the external player dimension variables
extern const int nPlayerW;
extern const int nPlayerH;

//player velocity
extern const int nPlayerVel;

//acceleration rate of the player/car
extern const int nPlayerAccelRate;

//tile dimensions
extern const int nTileW;
extern const int nTileH;

//screen dimensions
extern const int nScreenW;
extern const int nScreenH;

extern const int nGeneratedW;
extern const int nGeneratedH;

//corresponding numbers for each tile
extern const int TILE_brick;
extern const int TILE_wall;
extern const int TILE_unbreakable_wall;

//texture that holds the sprites for the player
extern Texture cCarSprites_texture;

//shows direction
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

Player::Player(int spawnX, int spawnY)
{
	//set the players box
	m_Box = { spawnX, spawnY, nPlayerW, nPlayerH };

	//set maximum and minimum velocities
	m_nMaxVel = nPlayerVel;
	m_nMinVel = -nPlayerVel;

	//player velocity starts at 0
	m_nXVel = 0;
	m_nYVel = 0;

	//set the rate of acceleration
	m_nAccelRate = nPlayerAccelRate;
}

//calculate the width and height of the box when turned at a 45 degree angle

inline int CalcBoxAt45()
{
	return (int)(((nPlayerW + nPlayerH) * sqrt(2.0))/2);
}

void Player::Render(SDL_Renderer* renderer, Direction eDir, int nFrame)
{
	using namespace std;

	//clip of the texture we will use
	SDL_Rect clip;

	//set clip dimensions
	clip.w = nPlayerW;
	clip.h = nPlayerH;
	clip.x = (nFrame - 1) * nPlayerW;
	clip.y = 0;
	
	SDL_Rect box = m_Box;
	box.x = nScreenW / 2;
	box.y = nScreenH / 2;

	//up
	if (eDir == Direction::up)
	{
		cCarSprites_texture.Render(renderer, clip, box, 0.0);
	}
	//up & left
	if (eDir == Direction::up_left)
	{
		box.x += (CalcBoxAt45() / 2) - (nPlayerW / 2);
		box.y += (CalcBoxAt45() / 2) - (nPlayerH / 2);

		cCarSprites_texture.Render(renderer, clip, box, -45.0);
	}
	//left
	if (eDir == Direction::left)
	{
		//add to the x and y positions(subtract y because y axis is flipped) to compensate for the amount subtracted when rotating from the original sprite
		box.x += (int)((nPlayerH/2) - (nPlayerW/2));
		box.y -= (int)((nPlayerH / 2) - (nPlayerW / 2));

		cCarSprites_texture.Render(renderer, clip, box, -90.0);
	}
	//down & left
	if (eDir == Direction::down_left)
	{
		box.x += (CalcBoxAt45() / 2) - (nPlayerW / 2);
		box.y += (CalcBoxAt45() / 2) - (nPlayerH / 2);

		cCarSprites_texture.Render(renderer, clip, box, -135.0);
	}
	//down
	if (eDir == Direction::down)
	{
		cCarSprites_texture.Render(renderer, clip, box, -180.0);
	}
	//down & right
	if (eDir == Direction::down_right)
	{
		box.x += (CalcBoxAt45() / 2) - (nPlayerW / 2);
		box.y += (CalcBoxAt45() / 2) - (nPlayerH / 2);

		cCarSprites_texture.Render(renderer, clip, box, 135.0);
	}
	//right
	if (eDir == Direction::right)
	{
		//add to the x and y positions(subtract y because y axis is flipped) to compensate for the amount subtracted when rotating from the original sprite
		box.x += (int)((nPlayerH / 2) - (nPlayerW / 2));
		box.y -= (int)((nPlayerH / 2) - (nPlayerW / 2));

		cCarSprites_texture.Render(renderer, clip, box, 90.0);
	}
	//up & right
	if (eDir == Direction::up_right)
	{
		box.x += (CalcBoxAt45() / 2) - (nPlayerW / 2);
		box.y += (CalcBoxAt45() / 2) - (nPlayerH / 2);

		cCarSprites_texture.Render(renderer, clip, box, 45.0);
	}
}

void HandleCollisions(std::vector<std::vector<int>> MapData, SDL_Rect &box, int &xVel, int &yVel, Direction &eDir, int nPlayerW, int nPlayerH)
{
	using namespace std;

	//declare player variables
	//player:
	int nL_player = box.x;				//left side of the tile
	int nR_player = box.x + box.w - 1;		//right side of the tile
	int nT_player = box.y;				//top side of the tile
	int nB_player = box.y + box.h - 1;		//bottom side of the tile
	
	int x = 0;
	int y = 0;

	for (vector<vector<int>>::iterator itMapY = MapData.begin(); itMapY != MapData.end(); ++itMapY, ++y)
	{
		for (vector<int>::iterator itMapX = (*itMapY).begin(); itMapX != (*itMapY).end(); ++itMapX, ++x)
		{
			//declare and initialize tile variables
			const int L_TILE = x * nTileW;			//left side of the tile
			const int R_TILE = x * nTileW + nTileW - 1;	//right side of the tile
			const int T_TILE = y * nTileH;			//top side of the tile
			const int B_TILE = y * nTileH + nTileH - 1;	//bottom side of the tile
			const int XMID_TILE = (x * nTileW) + (nTileW / 2);//middle of the tile onn the x axis
			const int YMID_TILE = (y * nTileH) + (nTileH / 2);//middle of the tile on the y axis

			if ((*itMapX) == TILE_wall || (*itMapX) == TILE_unbreakable_wall)
			{
				//if the player's in the tile
				if ((const int)nR_player >= L_TILE && (const int)nL_player <= R_TILE && (const int)nB_player >= T_TILE && (const int)nT_player <= B_TILE)
				{
					//left side of tile
					if ((const int)nR_player >= L_TILE && (const int)nR_player <= XMID_TILE && (const int)nB_player > T_TILE && (const int)nT_player < B_TILE && (eDir == Direction::up_right || eDir == Direction::right || eDir == Direction::down_right))
					{
						//down and right
						if (eDir == Direction::down_right)
						{
							//turn car
							eDir = Direction::down;

							//set the hit box for the turned car
							box.w = nPlayerW;
							box.h = nPlayerH;

							//stop x velocity
							xVel = 0;

							//place the car outside of the wall
							box.x = L_TILE - box.w + 1;
						}
						//up and right
						if (eDir == Direction::up_right)
						{
							//turn car
							eDir = Direction::up;

							//set the hit box for the turned car
							box.w = nPlayerW;
							box.h = nPlayerH;

							//stop x velocity
							xVel = 0;

							//place the car outside of the wall
							box.x = L_TILE - box.w + 1;
						}
						//not going into the wall (not moving to the right)
						if (eDir == Direction::right)
						{
							//place the car outside of the wall
							box.x = L_TILE - nPlayerH + 1;

							//stop player
							xVel = 0;
							yVel = 0;
						}
						//update player
						nL_player = box.x;
						nR_player = box.x + box.w - 1;
						nT_player = box.y;
						nB_player = box.y + box.h - 1;
					}

					//top side
					if ((const int)nB_player >= T_TILE && (const int)nB_player <= YMID_TILE && (const int)nR_player > L_TILE && (const int)nL_player < R_TILE && (eDir == Direction::down_left || eDir == Direction::down || eDir == Direction::down_right))
					{
						//turn car
						if (eDir == Direction::down_right)
						{
							eDir = Direction::right;

							//set the hit box for the turned car
							box.w = nPlayerH;
							box.h = nPlayerW;

							//stop y velocity
							yVel = 0;

							//place the car outside of the wall
							box.y = T_TILE - box.h + 1;
						}
						if (eDir == Direction::down_left)
						{
							eDir = Direction::left;

							//set the hit box for the turned car
							box.w = nPlayerH;
							box.h = nPlayerW;

							//stop y velocity
							yVel = 0;

							//place the car outside of the wall
							box.y = T_TILE - box.h + 1;
						}
						if (eDir == Direction::down)
						{
							//place the car outside of the wall
							box.y = T_TILE - nPlayerH + 1;

							//stop player
							xVel = 0;
							yVel = 0;
						}
						//update player
						nL_player = box.x;
						nR_player = box.x + box.w - 1;
						nT_player = box.y;
						nB_player = box.y + box.h - 1;
					}

					//right side
					if ((const int)nL_player <= R_TILE && (const int)nL_player >= XMID_TILE && (const int)nB_player > T_TILE && (const int)nT_player < B_TILE && (eDir == Direction::up_left || eDir == Direction::left || eDir == Direction::down_left))
					{
						//turn car
						if (eDir == Direction::down_left)
						{
							eDir = Direction::down;

							//set the hit box for the turned car
							box.w = nPlayerW;
							box.h = nPlayerH;

							//stop x velocity
							xVel = 0;

							//place the car outside of the wall
							box.x = R_TILE;
						}
						if (eDir == Direction::up_left)
						{
							eDir = Direction::up;

							//set the hit box for the turned car
							box.w = nPlayerW;
							box.h = nPlayerH;

							//stop x velocity
							xVel = 0;

							//place the car outside of the wall
							box.x = R_TILE;
						}
						//not going into the wall (not moving to the left)
						if (eDir == Direction::left)
						{
							//place the car outside of the wall
							box.x = R_TILE;

							//stop player
							xVel = 0;
							yVel = 0;
						}
						//update player
						nL_player = box.x;
						nR_player = box.x + box.w - 1;
						nT_player = box.y;
						nB_player = box.y + box.h - 1;
					}

					//bottom side
					if ((const int)nT_player <= B_TILE && (const int)nT_player >= YMID_TILE && (const int)nR_player > L_TILE && (const int)nL_player < R_TILE && (eDir == Direction::up_left || eDir == Direction::up || eDir == Direction::up_right))
					{
						//turn car
						if (eDir == Direction::up_right)
						{
							eDir = Direction::right;

							//set the hit box for the turned car
							box.w = nPlayerH;
							box.h = nPlayerW;

							//stop y velocity
							yVel = 0;

							//place the car outside of the wall
							box.y = B_TILE;
						}
						if (eDir == Direction::up_left)
						{
							eDir = Direction::left;

							//set the hit box for the turned car
							box.w = nPlayerH;
							box.h = nPlayerW;

							//stop y velocity
							yVel = 0;

							//place the car outside of the wall
							box.y = B_TILE;
						}
						if (eDir == Direction::up)
						{
							//place the car outside of the wall
							box.y = B_TILE;

							//stop player
							xVel = 0;
							yVel = 0;
						}
						//update player
						nL_player = box.x;
						nR_player = box.x + box.w - 1;
						nT_player = box.y;
						nB_player = box.y + box.h - 1;
					}
				}
			}
		}
	}
}

void Player::Move(std::vector<std::vector<int>> MapData, Direction &ePlayerDirection, Direction ePreviousPlayerDir, bool bIsPlayerStopped)
{
	using namespace std;

	//exit the function if the player is not moving
	if (bIsPlayerStopped)
	{
		return;
	}

	//change velocities and coordinates (when moving e.g. from right to down, the player is rotated and the coordinates need to change along with the rotation)
	if (ePlayerDirection == Direction::up_right)
	{
		//restart acceleration if the car was not going in that direction
		if (ePreviousPlayerDir != Direction::up && ePreviousPlayerDir != Direction::up_right && ePreviousPlayerDir != Direction::right)
		{
			m_nXVel = 0;
			m_nYVel = 0;
		}
		else
		{
		/*	CURRENTLY ISN'T WORKING: if the player is going diagonal they go faster by a factor of root 2
			if (ePreviousPlayerDir == Direction::right)
			{
				m_nYVel = (int) ( (double) (-m_nXVel / sqrt(2)) );
			}
			if (ePreviousPlayerDir == Direction::up)
			{
				m_nXVel = (int) ( (double) (-m_nYVel / sqrt(2)) );
			}
		*/	
		}

		//add the extra speed from acceleration to the player's velocity
		m_nXVel += m_nAccelRate;
		m_nYVel -= m_nAccelRate;

		//if the current velocity is higher than the maximum velocity
		if (m_nXVel > m_nMaxVel)
			m_nXVel = m_nMaxVel;

		if (m_nYVel < m_nMinVel)
			m_nYVel = m_nMinVel;

		m_Box.w = nPlayerH;
		m_Box.h = nPlayerH;
	}

	if (ePlayerDirection == Direction::right)
	{
		//restart acceleration if the car was not going in that direction
		if (ePreviousPlayerDir != Direction::up_right && ePreviousPlayerDir != Direction::right && ePreviousPlayerDir != Direction::down_right)
		{
			m_nXVel = 0;
			m_nYVel = 0;
		}

		//make sure that the car doesn't move up or down
		m_nYVel = 0;

		//add the extra speed from acceleration to the player's velocity
		m_nXVel += m_nAccelRate;

		//if the current velocity is higher than the maximum velocity
		if (m_nXVel > m_nMaxVel)
			m_nXVel = m_nMaxVel;

		//update the collision box:
		// this needs to be done because the width and height are different and when the car turns, the texture turns. The collision box needs to do the same.
		m_Box.w = nPlayerH;
		m_Box.h = nPlayerW;
	}

	if (ePlayerDirection == Direction::down_right)
	{
		//restart acceleration if the car was not going in that direction
		if (ePreviousPlayerDir != Direction::right && ePreviousPlayerDir != Direction::down_right && ePreviousPlayerDir != Direction::down)
		{
			m_nXVel = 0;
			m_nYVel = 0;
		}
		else
		{
			if (ePreviousPlayerDir == Direction::right)
			{
				m_nYVel = m_nXVel;
			}
			if (ePreviousPlayerDir == Direction::down)
			{
				m_nXVel = m_nYVel;
			}

		}
		

		//add the extra speed from acceleration to the player's velocity
		m_nXVel += m_nAccelRate;
		m_nYVel += m_nAccelRate;

		//if the current velocity is higher than the maximum velocity
		if (m_nXVel > m_nMaxVel)
			m_nXVel = m_nMaxVel;

		if (m_nYVel > m_nMaxVel)
			m_nYVel = m_nMaxVel;

		m_Box.w = nPlayerH;
		m_Box.h = nPlayerH;
	}

	if (ePlayerDirection == Direction::down)
	{
		//restart acceleration if the car was not going in that direction
		if (ePreviousPlayerDir != Direction::down_right && ePreviousPlayerDir != Direction::down && ePreviousPlayerDir != Direction::down_left)
		{
			m_nXVel = 0;
			m_nYVel = 0;
		}

		//make sure that the car doesn't move right or left
		m_nXVel = 0;

		//add the extra speed from acceleration to the player's velocity
		m_nYVel += m_nAccelRate;

		//if the current velocity is higher than the maximum velocity
		if (m_nYVel > m_nMaxVel)
			m_nYVel = m_nMaxVel;

		//update the collision box:
		// this needs to be done because the width and height are different and when the car turns, the texture turns. The collision box needs to do the same.
		m_Box.w = nPlayerW;
		m_Box.h = nPlayerH;
	}

	if (ePlayerDirection == Direction::down_left)
	{
		//restart acceleration if the car was not going in that direction
		if (ePreviousPlayerDir != Direction::down && ePreviousPlayerDir != Direction::down_left && ePreviousPlayerDir != Direction::left)
		{
			m_nXVel = 0;
			m_nYVel = 0;
		}
		else
		{
			if (ePreviousPlayerDir == Direction::left)
			{
				m_nYVel = -m_nXVel;
			}
			if (ePreviousPlayerDir == Direction::down)
			{
				m_nXVel = -m_nYVel;
			}
		}
		

		//add the extra speed from acceleration to the player's velocity
		m_nXVel -= m_nAccelRate;
		m_nYVel += m_nAccelRate;

		//if the current velocity is higher than the maximum velocity
		if (m_nXVel < m_nMinVel)
			m_nXVel = m_nMinVel;

		if (m_nYVel > m_nMaxVel)
			m_nYVel = m_nMaxVel;

		m_Box.w = nPlayerH;
		m_Box.h = nPlayerH;
	}

	if (ePlayerDirection == Direction::left)
	{
		//restart acceleration if the car was not going in that direction
		if (ePreviousPlayerDir != Direction::down_left && ePreviousPlayerDir != Direction::left && ePreviousPlayerDir != Direction::up_left)
		{
			m_nXVel = 0;
			m_nYVel = 0;
		}

		//make sure that the car doesn't move up or down
		m_nYVel = 0;

		//add the extra speed from acceleration to the player's velocity
		m_nXVel -= m_nAccelRate;

		//if the current velocity is higher than the maximum velocity
		if (m_nXVel < m_nMinVel)
			m_nXVel = m_nMinVel;

		//update the collision box:
		// this needs to be done because the width and height are different and when the car turns, the texture turns. The collision box needs to do the same.
		m_Box.w = nPlayerH;
		m_Box.h = nPlayerW;
	}

	if (ePlayerDirection == Direction::up_left)
	{
		//restart acceleration if the car was not going in that direction
		if (ePreviousPlayerDir != Direction::left && ePreviousPlayerDir != Direction::up_left && ePreviousPlayerDir != Direction::up)
		{
			m_nXVel = 0;
			m_nYVel = 0;
		}
		else
		{
			if (ePreviousPlayerDir == Direction::left)
			{
				m_nYVel = m_nXVel;
			}
			if (ePreviousPlayerDir == Direction::up)
			{
				m_nXVel = m_nYVel;
			}
		}
		

		//add the extra speed from acceleration to the player's velocity
		m_nXVel -= m_nAccelRate;
		m_nYVel -= m_nAccelRate;

		//if the current velocity is higher than the maximum velocity
		if (m_nXVel < m_nMinVel)
			m_nXVel = m_nMinVel;

		if (m_nYVel < m_nMinVel)
			m_nYVel = m_nMinVel;

		m_Box.w = nPlayerH;
		m_Box.h = nPlayerH;
	}

	if (ePlayerDirection == Direction::up)
	{
		//restart acceleration if the car was not going in that direction
		if (ePreviousPlayerDir != Direction::up_left && ePreviousPlayerDir != Direction::up && ePreviousPlayerDir != Direction::up_right)
		{
			m_nXVel = 0;
			m_nYVel = 0;
		}

		//make sure that the car doesn't move right or left
		m_nXVel = 0;

		//add the extra speed from acceleration to the player's velocity
		m_nYVel -= m_nAccelRate;

		//if the current velocity is higher than the maximum velocity
		if (m_nYVel < m_nMinVel)
			m_nYVel = m_nMinVel;

		//update the collision box:
		// this needs to be done because the width and height are different and when the car turns, the texture turns. The collision box needs to do the same.
		m_Box.w = nPlayerW;
		m_Box.h = nPlayerH;
	}

	//HandleCollisions(MapData, m_Box, m_nXVel, m_nYVel, ePlayerDirection, nPlayerW, nPlayerH);

	Entity::Move();
}