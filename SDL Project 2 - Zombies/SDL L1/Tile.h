#pragma once
#include "SDL/SDL.h"

class Tile
{
private:
	//type of tile
	int m_nTileType;
	SDL_Rect m_srect; // source rectangle - represents the area on the tile sprite sheet that the tile is on
	SDL_Rect m_Box;
public:
	Tile(SDL_Rect box, int nTileType);

	//render tile to screen
	void Render(SDL_Renderer* renderer);

	//set the type of tile
	void SetTileType(int nTileType);
	//get the type of tile
	int GetTileType() { return m_nTileType; }

	//get the collision box of the tile
	SDL_Rect GetBox() { return m_srect; }

	void SetBox(SDL_Rect box) { m_Box = box; }
};