#include "AnimationData.h"
#include "GraphicsBuffer.h"
#include "Sprite.h"

AnimationData::AnimationData(GraphicsBuffer* spriteSheetBuffer, int spriteWidth, int spriteHeight, int rows, int cols, int startingIndex, int endIndex)
{
	SDL_assert(startingIndex < rows * cols && endIndex < rows * cols && startingIndex <= endIndex && cols > 0 && rows > 0);

	int numberOfSPritesMade = 0;
	int numberOfSpritesToMake = endIndex - startingIndex + 1;
	int startingRow = startingIndex / cols;
	int startingCol = (startingIndex - startingRow * cols) + startingRow * cols;

	for (int i = startingRow; i < rows; i++)
	{
		for (int j = startingCol; j < cols; j++)
		{
			mSpriteList.push_back(Sprite(spriteSheetBuffer, Vector2D(spriteWidth * j, spriteHeight * i), spriteWidth, spriteHeight));
			numberOfSPritesMade++;

			if (numberOfSPritesMade == numberOfSpritesToMake)
			{
				break;
			}
		}
	}
}

AnimationData::~AnimationData()
{
}

const Sprite* AnimationData::getSprite(unsigned int index) const
{
	SDL_assert(index < mSpriteList.size());
	return &mSpriteList[index];
}

int AnimationData::getNumOfSprites()
{
	return mSpriteList.size();
}
