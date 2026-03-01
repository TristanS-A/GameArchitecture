#include "Button.h"
#include "Sprite.h"
#include "GraphicsBuffer.h"
#include "Font.h"

Button::Button(Buttons buttonType, Vector2D location, Sprite* buttonSprite, Sprite* hoveredSprite, vector<string> textList, Font* font, SoundClipKey clipKey, Vector2D scale) :
	mButtonType(buttonType), mpButtonSprite(buttonSprite), mpHoverSprite(hoveredSprite), mTextList(textList), mpButtonTextFont(font), mClipKey(clipKey), mpCurrSprite(mpButtonSprite)
{
	mLocationScaleRect = SDL_FRect{ location.getX(), location.getY(), (float)buttonSprite->getWidth(), (float)buttonSprite->getHeight()};
	mCurrTextIndex = 0;

	mOriginalWidth = mLocationScaleRect.w * scale.getX();
	mOriginalX = mLocationScaleRect.x - (mOriginalWidth - mLocationScaleRect.w) / 2;
	mOriginalScale = scale;
	mDisplayScale = scale;

	float originalYPos = mLocationScaleRect.h;
	mLocationScaleRect.h = mLocationScaleRect.h * scale.getY();
	mLocationScaleRect.y -= (mLocationScaleRect.h - originalYPos) / 2;
}

Button::~Button()
{
}

void Button::update(Vector2D clickLocation)
{
	//Checks if point is inside button bounds
	bool isInXBounds = clickLocation.getX() > mLocationScaleRect.x && clickLocation.getX() < mLocationScaleRect.x + mLocationScaleRect.w;
	bool isInYBounds = clickLocation.getY() > mLocationScaleRect.y && clickLocation.getY() < mLocationScaleRect.y + mLocationScaleRect.h;

	if (isInXBounds && isInYBounds)
	{
		mpCurrSprite = mpHoverSprite;
	}
	else
	{
		mpCurrSprite = mpButtonSprite;
	}
}

void Button::render(GraphicsBuffer* renderBuffer)
{

	Vector2D buttonPos(mLocationScaleRect.x, mLocationScaleRect.y);
	renderBuffer->draw(buttonPos, *getCurrSprite(), mDisplayScale);

	Vector2D textPos(buttonPos.getX() + mLocationScaleRect.w / 2, buttonPos.getY() + mLocationScaleRect.h / 2);
	renderBuffer->writeText(textPos, *mpButtonTextFont, Color(1.0f, 1.0f, 1.0f), mTextList[mCurrTextIndex], Font::CENTER);
}

Sprite* Button::getCurrSprite()
{
	return mpCurrSprite;
}

Vector2D Button::getPosition()
{
	return Vector2D(mLocationScaleRect.x, mLocationScaleRect.y);
}

Vector2D Button::getDimensions()
{
	return Vector2D(mLocationScaleRect.w, mLocationScaleRect.h);
}

Button::Buttons Button::getType()
{
	return mButtonType;
}

void Button::setCurrText(LanguageOptions language)
{
	mCurrTextIndex = language;

	mLocationScaleRect.x = mOriginalX;
	mLocationScaleRect.w = mOriginalWidth;
	mDisplayScale = mOriginalScale;

	//This is supposed to get rid of empty space on the left and right of the buttons when 
	// resizing (due to font size being wierd)
	float scaleOffset = 0.75f; 
	float currTextWidth = mTextList[mCurrTextIndex].length() * mpButtonTextFont->getSize() * scaleOffset;

	if (currTextWidth > mLocationScaleRect.w)
	{
		mDisplayScale.setX(((currTextWidth / mLocationScaleRect.w) * mOriginalScale.getX()));
		mLocationScaleRect.x -= (currTextWidth - mLocationScaleRect.w) / 2.0f;
		mLocationScaleRect.w = currTextWidth;
	}
}

void Button::setAltText(vector<string> altTextList)
{
	mAltTextList = altTextList;
}

void Button::toggleAltText()
{
	vector<string> prevList = mTextList;
	mTextList = mAltTextList;
	mAltTextList = prevList;
}

SoundClipKey Button::getClickSound()
{
	return mClipKey;
}
