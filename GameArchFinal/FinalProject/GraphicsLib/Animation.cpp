#include "Animation.h"
#include "AnimationData.h"

Animation::Animation(AnimationData* animationData, bool loop)
{
	mpAnimationData = animationData;
	this->mLoop = loop;
}

Animation::~Animation()
{
}

void Animation::update(float deltaTime)
{
	//Checks for paused
	if (mIsPaused) 
	{
		return;
	}

	//Updates current sprite based on time past
	mTimeSinceLastFrame += deltaTime;
	if (mTimeSinceLastFrame >= mPlaySpeed)
	{
		mTimeSinceLastFrame = 0;

		if ((int)mCurrSpriteIndex < mpAnimationData->getNumOfSprites() - 1)
		{
			mCurrSpriteIndex++;
		}
		else if (mLoop)
		{
			resetAnimation();
		}
	}
}

const Sprite* Animation::getCurrentSprite() const
{
	return mpAnimationData->getSprite(mCurrSpriteIndex);
}

void Animation::setSpeed(float fps)
{
	mPlaySpeed = 1000.0f / fps;
}

void Animation::setPaused(bool paused)
{
	mIsPaused = paused;
}

void Animation::resetAnimation()
{
	mCurrSpriteIndex = 0;
}
