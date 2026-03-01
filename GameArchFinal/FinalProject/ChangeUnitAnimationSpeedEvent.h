#pragma once
#include "GameEvent.h"

class ChangeUnitAnimationSpeedEvent : public GameEvent
{
public:
	ChangeUnitAnimationSpeedEvent(int animationspeed);
	~ChangeUnitAnimationSpeedEvent();

	int getAnimationSpeed() const;

private:
	int mAnimationSpeed;
};