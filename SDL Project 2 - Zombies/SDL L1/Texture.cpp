#include "stdafx.h"
#include "Texture.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string>
#include <iostream>

extern SDL_Renderer* gRenderer;

Texture::Texture()
{
	m_Texture = NULL;
	m_Box.w = 0;
	m_Box.h = 0;
	m_Box.x = 0;
	m_Box.y = 0;
}

Texture::~Texture()
{
	SDL_DestroyTexture(m_Texture);
	m_Texture = NULL;
}

//free old texture
void Texture::Free()
{
	m_Texture = NULL;
}

bool Texture::LoadFromFile(SDL_Renderer* Renderer, std::string path)
{
	using namespace std;

	//clear preexisting texture
	Free();

	//final texture
	SDL_Texture* newTexture = NULL;

	//load image at path to surface
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		cout << "SDL Error: " << SDL_GetError << "Can't load image at " << path.c_str() << endl;
	}
	else
	{
		//set the color key to get rid of the backround of the image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 255, 255));

		//create new texture from loaded surface
		newTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
		if (newTexture == NULL)
		{
			cout << "SDL Error: " << SDL_GetError() << " couldn't create new texture from loaded surface" << endl;
		}
		else
		{
			//get the width and height of the texture before we destroy it
			loadedSurface->w = m_Box.w;
			loadedSurface->h = m_Box.h;
		}
		//get rid of old surface pixels
		SDL_FreeSurface(loadedSurface);
	}

	m_Texture = newTexture;

	if (m_Texture == NULL)
		cout << "FAILED to load" << endl;
	return (m_Texture != NULL);
}
/*
bool Texture::LoadText(std::string text, SDL_Color color)
{
	using namespace std;

	//get rid of old texture
	Free();

	//success flag
	bool success = true;

	//Load text to surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
	if (textSurface == NULL)
	{
		cout << "surface couldn't be loaded, ttf error: " << TTF_GetError() << endl;
		success = false;
	}
	else
	{
		m_Texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (m_Texture == NULL)
		{
			cout << "texture couldn't be created from the loaded surface, ttf error: " << TTF_GetError() << endl;
			success = false;
		}
		else
		{
			m_Box.w = textSurface->w;
			m_Box.h = textSurface->h;
		}

		//free old surface
		SDL_FreeSurface(textSurface);
	}
	return success;
}
*/
void Texture::Render(SDL_Renderer* Renderer, const SDL_Rect srect, const SDL_Rect drect, double angle)
{
	using namespace std;
	//render texture to the screen
	if (SDL_RenderCopyEx(Renderer, m_Texture, &srect, &drect, angle, NULL, SDL_FLIP_NONE) != 0)
	{
		cout << "Error: FAILED to render texture, SDL Error: " << SDL_GetError() << endl;
		if (m_Texture == NULL)
		{
			cout << "warning: m_Texture is null" << endl;
		}
	}
}

void Texture::SetBox(int x, int y, int w, int h)
{
	m_Box.x = x;
	m_Box.y = y;
	m_Box.w = w;
	m_Box.h = h;
}