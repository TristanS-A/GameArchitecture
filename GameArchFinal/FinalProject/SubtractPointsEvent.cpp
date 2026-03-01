#include "SubtractPointsEvent.h"

SubtractPointsEvent::SubtractPointsEvent(int pointsToSubtract, int gamePoints) : GameEvent(SUBTRACT_POINTS), mPointsToSubtract(pointsToSubtract), mGamePoints(gamePoints)
{
}

SubtractPointsEvent::~SubtractPointsEvent()
{
}

int SubtractPointsEvent::getPointAmount() const
{
	return mPointsToSubtract;
}

int SubtractPointsEvent::getCurrGamePointAmount() const
{
	return mGamePoints;
}
