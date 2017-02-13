#pragma once
#include "stdafx.h"
#include <SDL/SDL.h>
#include <iostream>

//shows direction
enum class Direction;

class Entity
{
protected:
	//velocity
	int m_nXVel;
	int m_nYVel;

	//location
	SDL_Rect m_Box;
public:
	Entity();

	//set velocity
	void SetVel(int x, int y);

	//move
	void Move();

	//get velocity
	int GetXVel() { return m_nXVel; }
	int GetYVel() { return m_nYVel; }

	SDL_Rect GetBox() { return m_Box; }
};