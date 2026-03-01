#include "ChangeUnitAnimationEvent.h"

ChangeUnitAnimationEvent::ChangeUnitAnimationEvent(unsigned int animationIndex) : GameEvent( CHANGE_UNIT_ANIMATION ), mAnimationIndex(animationIndex)
{
}

ChangeUnitAnimationEvent::~ChangeUnitAnimationEvent()
{
}

unsigned int ChangeUnitAnimationEvent::getAnimationIndex() const
{
	return mAnimationIndex;
}
