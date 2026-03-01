#pragma once
#include "GameEvent.h"
class SubtractPointsEvent : public GameEvent
{
public:
	SubtractPointsEvent(int pointsToSubtract, int gamePoints);
	~SubtractPointsEvent();

	int getPointAmount() const;
	int getCurrGamePointAmount() const;

private:
	int mPointsToSubtract = 0;
	int mGamePoints = 0;
};

