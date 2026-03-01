#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <Trackable.h>
#include <Vector2D.h>

#include <string>
#include "Color.h"
#include "Font.h"

class GraphicsSystem;
class Vector2D;
class Sprite;

class GraphicsBuffer : public Trackable
{
public:
	GraphicsBuffer(const std::string& filename);
	GraphicsBuffer(unsigned int width, unsigned int height, Color color = Color());
	~GraphicsBuffer();

	GraphicsBuffer(SDL_Window* pWindow);//to be called by GraphicsSystem only

	GraphicsBuffer* clone() const;//return a new (dynamically allocated) buffer with the same contents as this

	SDL_Rect getDimentions() const { return mpSurface != NULL ? SDL_Rect{ 0, 0, mpSurface->w, mpSurface->h } : SDL_Rect{}; };

	void draw(const Vector2D& destLoc, const GraphicsBuffer& src, double scale = 1.0);
	void draw(const Vector2D& destLoc, const Sprite& sprite, double scale = 1.0);
	void draw(const Vector2D& destLoc, const Sprite& sprite, Vector2D scale = Vector2D(1, 1));
	void writeText(const Vector2D& destLoc, const Font& font, const Color& color, const std::string& text, Font::Alignment align = Font::LEFT);
	void setToColor(const Color& color);
	void saveToFile(const std::string& filename) const;

	SDL_Surface* getSurface() { return mpSurface; };

private:
	SDL_Surface* mpSurface = nullptr;
	bool mOwnsBitmap = true;

	//invalidate copy constructor and assignment operator - why?
	GraphicsBuffer(GraphicsBuffer&)=delete;
	GraphicsBuffer operator=(GraphicsBuffer&)=delete;
};
