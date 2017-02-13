#include "stdafx.h"
#include "Tile.h"
#include "SDL/SDL.h"
#include "Texture.h"

extern const int nTileW;
extern const int nTileH;

extern Texture cTileSprites_texture;

Tile::Tile(SDL_Rect box, int nTileType)
{
	m_nTileType = nTileType;
	m_Box = box;
	m_srect = { (m_nTileType * nTileW), 0, nTileW, nTileH };
}

void Tile::Render(SDL_Renderer* renderer)
{
	cTileSprites_texture.Render(renderer, m_srect, m_Box, 0.0);
}

void Tile::SetTileType(int nType)
{
	m_nTileType = nType;
	m_srect.w = nType * 100;
}