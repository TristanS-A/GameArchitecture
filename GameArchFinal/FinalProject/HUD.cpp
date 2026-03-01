#include "HUD.h"
#include "Color.h"
#include "Font.h"
#include "GraphicsBuffer.h"
#include "Sprite.h"

HUD::HUD()
{
}

HUD::~HUD()
{
	cleanup();
}

void HUD::initGraphicsBuffer(unsigned int width, unsigned int height)
{
	mpHUDGraphicsBuffer = new GraphicsBuffer(width, height);
	clearHUDBuffer();
}

void HUD::cleanup()
{
	delete mpHUDGraphicsBuffer;
	mpHUDGraphicsBuffer = nullptr;
}

void HUD::draw(const Vector2D& destLoc, const Sprite& sprite, double scale)
{
	mpHUDGraphicsBuffer->draw(destLoc, sprite, scale);
}

void HUD::draw(const Vector2D& destLoc, const Sprite& sprite, Vector2D scale)
{
	mpHUDGraphicsBuffer->draw(destLoc, sprite, scale);
}

void HUD::writeHUDText(const Vector2D& destLoc, const Font& font, const Color& color, const std::string& text, Font::Alignment align)
{
	mpHUDGraphicsBuffer->writeText(destLoc, font, color, text, align);
}

void HUD::renderHUD(GraphicsBuffer* backbuffer)
{
	backbuffer->draw(Vector2D(0, 0), *mpHUDGraphicsBuffer);
	clearHUDBuffer();
}

void HUD::clearHUDBuffer()
{
	const Color aColor(0.0f, 0.0f, 0.0f, 0.0f);
	mpHUDGraphicsBuffer->setToColor(aColor);
}

GraphicsBuffer* HUD::getBuffer()
{
	return mpHUDGraphicsBuffer;
}
