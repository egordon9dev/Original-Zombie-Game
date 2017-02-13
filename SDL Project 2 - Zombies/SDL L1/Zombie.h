#pragma once
#include "stdafx.h"
#include "Entity.h"
#include "SDL/SDL.h"
#include "Texture.h"
#include <vector>

struct Circle{ int x, y, r, d; };

class Zombie: public Entity
{
private:
	//get rid of the GetBox function because the zombie is a circle
	SDL_Rect GetBox();

	//frame count after death of zombie
	int nCountAfterDeath = 0;

	//zombie spawn rate: 1 = 1 zombie per available tile, 2 = zombies per available tile
	int m_nZombieSpawnRate;

	//health
	unsigned char m_health;

	//the spot that the zombie is lured to
	SDL_Point m_AttractionPoint;

	//circle of collision
	//this circle is used as the x and y co-ordinate for the zombie
	//the m_Box variable inherited from Entity has an x and y, but they are not used and are unitialized
	Circle m_Circle;

public:
	//param x and y are the coordinates of the spawn point
	Zombie(std::vector<std::vector<int>> MapData);

	void Zombie::Move(std::vector<std::vector<int>> MapData, SDL_Rect PlayerBox, SDL_Point AttractionPoint);

	void Render(SDL_Renderer* renderer);

	//get circle holding the dimensions of the zombie
	Circle GetCircle(){ return m_Circle; }
};