#include "GraphicsBuffer.h"
#include "GraphicsSystem.h"
#include "Color.h"
#include "Sprite.h"
#include "Font.h"

GraphicsBuffer::GraphicsBuffer(SDL_Window* pWindow)
{
	//Gets surface from window (The backbufer)
	mpSurface = SDL_GetWindowSurface(pWindow);
	mOwnsBitmap = false;
}

GraphicsBuffer::GraphicsBuffer(const std::string& filename)
{
	//Loads Surface from the given file
	mpSurface = IMG_Load(filename.c_str());
}

GraphicsBuffer::GraphicsBuffer(unsigned int width, unsigned int height, Color color)
{
	//Creates surface from height and width
	mpSurface = SDL_CreateRGBSurface(0, width, height, 32, 0xff, 0xff00, 0xff0000, 0xff000000);

	setToColor(color);
}

GraphicsBuffer::~GraphicsBuffer()
{
	//make sure you destroy the bitmap (if appropriate)
	if (mOwnsBitmap)
	{
		SDL_FreeSurface(mpSurface);
		mpSurface = nullptr;
	}
}

GraphicsBuffer * GraphicsBuffer::clone() const
{
	SDL_Rect dimensions = getDimentions();
	//Create new surface and copy all of "this" into it
	GraphicsBuffer* pNewBuffer = new GraphicsBuffer(dimensions.w, dimensions.h);
	pNewBuffer->draw(Vector2D(0, 0), *this, 1.0);
	return pNewBuffer;
}

void GraphicsBuffer::draw(const Vector2D& destLoc, const GraphicsBuffer& src, double scale)
{
	SDL_Surface* pTarget = mpSurface;//set the target bitmap
	
	SDL_assert(pTarget != src.mpSurface);//make sure we are not trying to draw to and from the same bitmap
	if (pTarget != src.mpSurface)
	{
		//Makes Rects needed for blit
		SDL_Rect srcRect = src.getDimentions();
		SDL_Rect destRect = SDL_Rect{ (int)destLoc.getX(), (int)destLoc.getY(), (int)(srcRect.w * scale), (int)(srcRect.h * scale) };

		//call al_draw_scaled_bitmap to do the actual drawing
		SDL_BlitScaled(src.mpSurface, &srcRect, pTarget, &destRect);
	}
}


void GraphicsBuffer::draw(const Vector2D& destLoc, const Sprite& sprite, double scale /*= 1.0*/)
{
	SDL_Surface* pTarget = mpSurface;
	GraphicsBuffer* spriteBuff = sprite.getBuffer();

	SDL_assert(pTarget != spriteBuff->mpSurface);//make sure we are not trying to draw to and from the same bitmap
	if (pTarget != spriteBuff->mpSurface)
	{
		int spriteW = sprite.getWidth();
		int spriteH = sprite.getHeight();

		//Makes Rects needed for blit
		SDL_Rect srcRect = SDL_Rect{ (int)sprite.getSourceLoc().getX(), (int)sprite.getSourceLoc().getY(), spriteW, spriteH };
		SDL_Rect destRect = SDL_Rect{ (int)destLoc.getX(), (int)destLoc.getY(), (int)(spriteW * scale), (int)(spriteH * scale) };

		//do the same steps as in the other draw function - 
		//  make sure we are not drawing from and to the same bitmap
		SDL_BlitScaled(spriteBuff->mpSurface, &srcRect, pTarget, &destRect);
	}
}

void GraphicsBuffer::draw(const Vector2D& destLoc, const Sprite& sprite, Vector2D scale)
{
	SDL_Surface* pTarget = mpSurface;
	GraphicsBuffer* spriteBuff = sprite.getBuffer();

	SDL_assert(pTarget != spriteBuff->mpSurface);//make sure we are not trying to draw to and from the same bitmap
	if (pTarget != spriteBuff->mpSurface)
	{
		int spriteW = sprite.getWidth();
		int spriteH = sprite.getHeight();

		//Makes Rects needed for blit
		SDL_Rect srcRect = SDL_Rect{ (int)sprite.getSourceLoc().getX(), (int)sprite.getSourceLoc().getY(), spriteW, spriteH };
		SDL_Rect destRect = SDL_Rect{ (int)destLoc.getX(), (int)destLoc.getY(), (int)(spriteW * scale.getX()), (int)(spriteH * scale.getY()) };

		//do the same steps as in the other draw function - 
		//  make sure we are not drawing from and to the same bitmap
		SDL_BlitScaled(spriteBuff->mpSurface, &srcRect, pTarget, &destRect);
	}
}

void GraphicsBuffer::writeText(const Vector2D& destLoc, const Font& font, const Color& color, const std::string& text, Font::Alignment align)
{
	//call al_draw_text - 
	//  use helper functions Color::getAllegroColorFromColor and
	//  Font::getAllegroFontAlignFlag
	SDL_Color newColor = { (Uint8)color.getR(),(Uint8)color.getG(), (Uint8)color.getB()};
	SDL_Surface* textSurface = TTF_RenderText_Solid(font.mpFont, text.c_str(), newColor);

	Vector2D newDestLoc = destLoc;

	switch (align) 
	{
	case Font::Alignment::CENTER:
		newDestLoc = Vector2D(destLoc.getX() - (textSurface->w / 2.0f), destLoc.getY() - (textSurface->h / 2.0f));
		break;
	case Font::Alignment::RIGHT:
		newDestLoc = Vector2D(destLoc.getX() - (textSurface->w), destLoc.getY());
	}

	//Makes Rects needed for blit
	SDL_Rect destRect = SDL_Rect{ (int)newDestLoc.getX(), (int)newDestLoc.getY(), textSurface->w, textSurface->h };

	SDL_BlitSurface(textSurface, nullptr, mpSurface, &destRect);
}

void GraphicsBuffer::setToColor(const Color& color)
{
	SDL_FillRect(mpSurface, NULL, Color::getUint32ColorFromColor(color));
}

void GraphicsBuffer::saveToFile(const std::string& filename) const
{
	//Saves Surface to a file
	IMG_SavePNG(mpSurface, filename.c_str());
}

