#pragma once
#include "Sprite.h"
#include <Trackable.h>
#include <vector>

class AnimationData;

using namespace std;

class Animation : public Trackable
{
public:
	enum Animation_Speed
	{
		FAST = 30,
		MEDIUM = 15,
		SLOW = 5
	};

	Animation(AnimationData* animationData, bool loop);
	~Animation();

	void update(float deltaTime);
	const Sprite* getCurrentSprite() const;
	float getAnimationSpeed() { return mPlaySpeed * 1000; };
	void setSpeed(float fps);
	void setPaused(bool paused);
	void resetAnimation();
	AnimationData* getAnimationData() { return mpAnimationData; };
	bool getIsLooping() { return mLoop; };
private:
	AnimationData* mpAnimationData;
	unsigned int mCurrSpriteIndex = 0;
	float mPlaySpeed = 0;
	float mTimeSinceLastFrame = 0.0;
	bool mLoop = false;
	bool mIsPaused = false;
};