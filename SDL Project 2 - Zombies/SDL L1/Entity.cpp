#include "stdafx.h"
#include "Entity.h"

//screen dimensions
extern int nScreenW;
extern int nScreenH;

//player speed
extern const int nPlayerVel;

//map dimensions (tiles)
extern const int nMapW;
extern const int nMapH;

//tile dimensions
extern const int nTileW;
extern const int nTileH;

//map data
int GetMapData(int x, int y);

//return whether or not the tiletype is a wall tile
bool IsWallTile(int);

//collision detection function
bool CheckCollision(SDL_Rect, SDL_Rect);

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

Entity::Entity()
{
	m_Box = { 0, 0, 0, 0 };
	m_nXVel = 0;
	m_nYVel = 0;
}


void Entity::SetVel(int xVel, int yVel)
{
	//set velocities
	m_nXVel = xVel;
	m_nYVel = yVel;
}

void Entity::Move()
{
	//move the entity's collision box
	m_Box.x += m_nXVel;
	m_Box.y += m_nYVel;
}