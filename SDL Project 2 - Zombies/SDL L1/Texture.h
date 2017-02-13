#pragma once

#include <string>
#include <SDL/SDL.h>

class Texture
{
public:
	//initializes variables and sets m_Texture to null
	Texture();

	//destroys texture and sets it to null
	~Texture();

	//load texture from a given path
	bool LoadFromFile(SDL_Renderer* Renderer, std::string path);

	//render texture
	void Render(SDL_Renderer* Renderer, SDL_Rect srect, SDL_Rect drect, double angle);
	
	//free variables
	void Free();

	//get texture
	SDL_Texture* GetTexture() { return m_Texture; }
	//set texture
	void SetTexture(SDL_Texture* texture) { m_Texture = texture; }

	bool LoadText(std::string text, SDL_Color color);

	void SetBox(int x, int y, int w, int h);
	SDL_Rect GetBox() { return m_Box; }


private:
	SDL_Texture* m_Texture;
	SDL_Rect m_Box;
};