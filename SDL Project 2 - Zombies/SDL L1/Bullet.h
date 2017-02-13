#pragma once
#include "Entity.h"
#include <SDL/SDL.h>
#include <vector>

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

class Bullet: public Entity
{
private:
	Direction m_eDir;
private:
	bool m_bDespawn = false;
public:
	//constructor: param x and y: initial location, param xVel and yVel: velocity
	Bullet(int x, int y, int xVel, int yVel);

	void Move(std::vector<std::vector<int>> &vMapData);

	//Render
	void Render();

	bool isNotSpawned() { return m_bDespawn; }
};