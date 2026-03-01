#pragma once

#include <Trackable.h>
#include <Vector2D.h>

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_image.h>

#include "Color.h"
#include "Font.h"

class GraphicsBuffer;
class Sprite;

class GraphicsSystem :public Trackable
{
public:
	GraphicsSystem();
	~GraphicsSystem();

	bool init(unsigned int width, unsigned int height);
	
	void cleanup();

	unsigned int getDisplayWidth() const { return mWidth; };
	unsigned int getDisplayHeight() const { return mHeight; };
	
	static GraphicsBuffer* getBackBuffer() { return mpBackBuffer; };

	void render();

private:
	SDL_Window* mpWindow = nullptr;
	SDL_Renderer* mpRenderer = nullptr;
	static GraphicsBuffer* mpBackBuffer;
	SDL_Texture* mpRenderTexture = nullptr;
	SDL_Rect mRenderTextureRect = {};

	unsigned int mWidth = 0;
	unsigned int mHeight = 0;

	bool initAddOns();
	SDL_Window* getWindow() const { return mpWindow; };


};
