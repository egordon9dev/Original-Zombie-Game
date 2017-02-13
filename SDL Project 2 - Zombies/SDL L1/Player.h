#pragma once

#include "stdafx.h"
#include <iostream>
#include "Entity.h"
#include "SDL/SDL.h"
#include "SDL/SDL.h"
#include "Texture.h"
#include <vector>

extern const unsigned char MaxPlayerHealth;

//shows direction
enum class Direction;

class Player : public Entity
{
private:
	int m_nMaxVel;
	int m_nMinVel;

	int m_nAccelRate;
	unsigned char m_Health = MaxPlayerHealth;
public:

	Player(int spawnX, int spawnY);

	//move 
	void Move(std::vector<std::vector<int>> MapData, Direction &ePlayerDirection, Direction ePreviousPlayerDir, bool bIsPlayerStopped);

	//render the player to the screen
	void Render(SDL_Renderer* renderer, Direction eDir, int nFrame);

	//get and set the entity's collision box
	void SetBox(int x, int y, int w, int h) { m_Box = { x, y, w, h }; };

	//get and set player health
	void SetHealth(unsigned char health) { m_Health = health; }
	unsigned char GetHealth() { return m_Health; }
};