#include "ChangeUnitAnimationSpeedEvent.h"

ChangeUnitAnimationSpeedEvent::ChangeUnitAnimationSpeedEvent(int animationspeed) : GameEvent ( CHANGE_UNIT_ANIMATION_SPEED ), mAnimationSpeed(animationspeed)
{
}

ChangeUnitAnimationSpeedEvent::~ChangeUnitAnimationSpeedEvent()
{
}

int ChangeUnitAnimationSpeedEvent::getAnimationSpeed() const
{
	return mAnimationSpeed;
}
