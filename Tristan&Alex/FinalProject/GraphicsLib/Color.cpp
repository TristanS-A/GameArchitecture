#include "Color.h"

Color::Color(int r, int g, int b, int a /*= 255*/)
{
	mRed = r;
	mGreen = g;
	mBlue = b;
	mAlpha = a;
}

Color::Color(float r, float g, float b, float a /*= 1.0f*/)
{
	mRed = (int)(r * 255);
	mGreen = (int)(g * 255);
	mBlue = (int)(b * 255);
	mAlpha = (int)(a * 255);
}

SDL_Color Color::getSDLColorFromColor(const Color& color)
{
	//call al_map_rgba or al_map_rgba_f
	const SDL_Color aColor = { (Uint8)color.mRed, (Uint8)color.mGreen, (Uint8)color.mBlue, (Uint8)color.mAlpha };
	return aColor;
}

Uint32 Color::getUint32ColorFromColor(const Color& color)
{
	SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
	return SDL_MapRGBA(format, color.mRed, color.mGreen, color.mBlue, color.mAlpha);
}


