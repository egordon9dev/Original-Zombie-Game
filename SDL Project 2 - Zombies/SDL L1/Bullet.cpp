#include "stdafx.h"
#include "Bullet.h"
#include "Texture.h"
#include <SDL/SDL.h>
#include <vector>

//bullet texture
extern Texture cBullet_texture;

//renderer
extern SDL_Renderer* gRenderer;

//bullet dimensions
extern const int nBulletW;
extern const int nBulletH;

//map dimensions
extern const int nMapW;
extern const int nMapH;

//tile dimensions
extern const int nTileW;
extern const int nTileH;

//function to check if the tile is a wall
extern bool IsWallTile(int nTile);

//tile types
extern const int TILE_brick;
extern const int TILE_wall;
extern const int TILE_unbreakable_wall;

//check collision
bool CheckCollision(SDL_Rect a, SDL_Rect b);

//map
//extern int GetMapData(int x, int y);

Bullet::Bullet(int x, int y, int xVel, int yVel)
{
	m_Box = { 0, 0, 0, 0 };
	m_nXVel = 0;
	m_nYVel = 0;

	//set velocity
	m_nXVel = xVel;
	m_nYVel = yVel;

	//set location
	m_Box.x = x;
	m_Box.y = y;
	m_Box.w = nBulletW;
	m_Box.h = nBulletH;

	//set direction

	////////////////////////////////////////   Diagonal   /////////////////////////////////////

	//down and right
	if (m_nXVel > 0 && m_nYVel > 0)
	{
		m_eDir = Direction::down_right;
		return;
	}
	//down and left
	if (m_nXVel < 0 && m_nYVel > 0)
	{
		m_eDir = Direction::down_left;
		return;
	}
	//up and left
	if (m_nXVel < 0 && m_nYVel < 0)
	{
		m_eDir = Direction::up_left;
		return;
	}
	//up and right
	if (m_nXVel > 0 && m_nYVel < 0)
	{
		m_eDir = Direction::up_right;
		return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////   Not Diagonal   //////////////////////////////////

	//right
	if (m_nXVel > 0)
	{
		m_eDir = Direction::right;
	}

	//left
	if (m_nXVel < 0)
	{
		m_eDir = Direction::left;
	}

	//up
	if (m_nYVel > 0)
	{
		m_eDir = Direction::down;
	}

	//down
	if (m_nYVel < 0)
	{
		m_eDir = Direction::up;
	}
}

void Bullet::Move(std::vector<std::vector<int>> &vMapData)
{
	m_Box.x += m_nXVel;
	m_Box.y += m_nYVel;

	using namespace std;
	
	int x = 0;
	int y = 0;
	
	for (vector<vector<int>>::iterator itMapY = vMapData.begin(); itMapY != vMapData.end(); ++itMapY, ++y)
	{
		for (vector<int>::iterator itMapX = (*itMapY).begin(); itMapX != (*itMapY).end(); ++itMapX, ++x)
		{
			if (IsWallTile(*itMapX))
			{
				SDL_Rect nTileRect = { x * nTileW, y * nTileH, nTileW, nTileH };
				if (CheckCollision(m_Box, nTileRect))
				{
					if (!m_bDespawn && (*itMapX) != TILE_unbreakable_wall)
						vMapData[x][y] = TILE_brick;

					m_bDespawn = true;
				}
			}
		}
	}
}
void Bullet::Render()
{
	if (!m_bDespawn)
	{
		//angle that the bullet is at
		double angle;

		//set angle of the bullet based on the direction that the bullet's going
		if (m_eDir == Direction::up)
			angle = 0.0;
		if (m_eDir == Direction::up_right)
			angle = 45.0;
		if (m_eDir == Direction::right)
			angle = 90.0;
		if (m_eDir == Direction::down_right)
			angle = 135.0;
		if (m_eDir == Direction::down)
			angle = 180.0;
		if (m_eDir == Direction::down_left)
			angle = 225.0;
		if (m_eDir == Direction::left)
			angle = 270.0;
		if (m_eDir == Direction::up_left)
			angle = 315.0;

		//source rectangle
		SDL_Rect clip = { 0, 0, nBulletW, nBulletH };

		cBullet_texture.Render(gRenderer, clip, m_Box, angle);
	}
}