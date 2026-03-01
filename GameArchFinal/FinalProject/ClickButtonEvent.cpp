#include "ClickButtonEvent.h"

ClickButtonEvent::ClickButtonEvent(Vector2D clickPos) : GameEvent ( CLICK_BUTTON ), mClickPos(clickPos)
{
}

ClickButtonEvent::~ClickButtonEvent()
{
}

Vector2D ClickButtonEvent::getClickPos() const
{
	return mClickPos;
}
