#pragma once
#include <SDL/SDL.h>

class Blood
{
private:
	SDL_Rect m_Box;

	//the number of frames the blood has been on screen for
	int m_nCount = 0;

public:
	Blood(SDL_Rect box) { m_Box = box; };

	void Blood::Render(SDL_Renderer* renderer);

	void IncrementCount() { ++m_nCount; }

	int GetCount() { return m_nCount; }
};