#pragma once
#include "GameEvent.h"
#include <Vector2D.h>

class MoveMouseEvent : public GameEvent
{
public:
	MoveMouseEvent(Vector2D mouseLocation);
	~MoveMouseEvent();

	Vector2D getMouseLocation() const;

private:
	Vector2D mMouseLocation;
};