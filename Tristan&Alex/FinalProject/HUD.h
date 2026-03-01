#pragma once
#include <Trackable.h>
#include <Vector2D.h>
#include "Font.h"

class Color;
class Font;
class GraphicsBuffer;
class Sprite;

class HUD : public Trackable
{
public:
	HUD();
	~HUD();

	void initGraphicsBuffer(unsigned int width, unsigned int height);
	void cleanup();

	void draw(const Vector2D& destLoc, const Sprite& sprite, double scale = 1.0);
	void draw(const Vector2D& destLoc, const Sprite& sprite, Vector2D scale);
	void writeHUDText(const Vector2D& destLoc, const Font& font, const Color& color, const std::string& text, Font::Alignment align = Font::LEFT);
	void renderHUD(GraphicsBuffer* backbuffer);
	void clearHUDBuffer();
	GraphicsBuffer* getBuffer();

private:
	GraphicsBuffer* mpHUDGraphicsBuffer = nullptr;
};

