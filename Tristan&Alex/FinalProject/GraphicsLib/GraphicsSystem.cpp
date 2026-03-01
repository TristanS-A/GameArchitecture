#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "Sprite.h"
#include "Color.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <iostream>
#include <cassert>

using namespace std;

GraphicsBuffer* GraphicsSystem::mpBackBuffer=nullptr;


GraphicsSystem::GraphicsSystem()
{
	//No need to init member variables as that is done in class decloration and above for mpBackBuffer
}

GraphicsSystem::~GraphicsSystem()
{
	cleanup();
}

bool GraphicsSystem::init(unsigned int width, unsigned int height)
{
	cleanup();

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		cout << "SDL could not be initialized!" << endl
			<< "SDL_Error: " << SDL_GetError() << endl;
		return false;
	}

	if (!initAddOns()) 
	{
		cout << "ERROR - Add ons not initted\n";
		return false;
	}

	mWidth = width;
	mHeight = height;
	
	// Create window
	mpWindow = SDL_CreateWindow("Game Arch Final Project!!",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

	//Test for if window wasn't created
	if (!mpWindow) {
		cout << "Window could not be created!" << endl
			<< "SDL_Error: " << SDL_GetError() << endl;
		return false;
	}

	// Create renderer
	mpRenderer = SDL_CreateRenderer(mpWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!mpRenderer) {
		cout << "Renderer could not be created!" << endl
			<< "SDL_Error: " << SDL_GetError() << endl;
		return false;
	}

	//Creates backbuffer surface that will be used for blitting
	mpBackBuffer = new GraphicsBuffer(mpWindow);

	//Creates texture from windowSurf surface to render windowSurf with the renderer
	mpRenderTexture = SDL_CreateTextureFromSurface(mpRenderer, mpBackBuffer->getSurface());

	//Creates rect with location and size of render texture
	mRenderTextureRect = { 0, 0, (int)width, (int)height };

	//return true if everything is initted properly
	return true;
}

void GraphicsSystem::cleanup()
{
	//Deletes backbuffer
	delete mpBackBuffer;
	mpBackBuffer = nullptr;

	//Destroys textures
	SDL_DestroyTexture(mpRenderTexture);

	//Destroy renderer
	SDL_DestroyRenderer(mpRenderer);
	mpRenderer = nullptr;

	// Destroy window
	SDL_DestroyWindow(mpWindow);
	mpWindow = nullptr;

	//Quit SDL_ttf
	TTF_Quit();

	// Quit SDL
	SDL_Quit();
}

void GraphicsSystem::render()
{
	//Updates windowTexture, so it can be rendered with new blit info
	SDL_UpdateTexture(mpRenderTexture, nullptr, mpBackBuffer->getSurface()->pixels, mpBackBuffer->getSurface()->pitch);

	// Clear screen
	SDL_RenderClear(mpRenderer);

	//Prepares windowTexture to be rendered at the dest location and size
	SDL_RenderCopy(mpRenderer, mpRenderTexture, nullptr, &mRenderTextureRect);

	// Update screen
	SDL_RenderPresent(mpRenderer);
}

bool GraphicsSystem::initAddOns()
{
	//Initializes sdl_image
	if (IMG_Init(IMG_INIT_PNG) == 0) {
		cout << "SDL_Image could not be created!" << endl
			<< "SDL_Error: " << SDL_GetError() << endl;
		return false;
	}

	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		return false;
	}

	return true;
}
