#include "stdafx.h"
#include "Blood.h"
#include "Texture.h"

extern Texture cBlood_texture;

extern const int nBloodW;
extern const int nBloodH;

void Blood::Render(SDL_Renderer* renderer)
{
	SDL_Rect clip = { 0, 0, nBloodW, nBloodH };

	cBlood_texture.Render(renderer, clip, m_Box, 0.0);
}