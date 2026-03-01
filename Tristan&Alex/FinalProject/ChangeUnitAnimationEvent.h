#pragma once
#include "GameEvent.h"

class ChangeUnitAnimationEvent : public GameEvent
{
public:
	ChangeUnitAnimationEvent(unsigned int animationIndex);
	~ChangeUnitAnimationEvent();

	unsigned int getAnimationIndex() const;

private:
	unsigned int mAnimationIndex = 0;
};