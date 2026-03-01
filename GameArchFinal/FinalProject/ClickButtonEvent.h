#pragma once
#include "GameEvent.h"
#include <Vector2D.h>

class ClickButtonEvent : public GameEvent
{
public:
	ClickButtonEvent(Vector2D clickPos);
	~ClickButtonEvent();

	Vector2D getClickPos() const;

private:
	Vector2D mClickPos;
};