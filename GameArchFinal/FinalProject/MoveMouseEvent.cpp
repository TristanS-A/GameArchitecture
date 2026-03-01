#include "MoveMouseEvent.h"

MoveMouseEvent::MoveMouseEvent(Vector2D mouseLocation) : GameEvent( MOVE_MOUSE ), mMouseLocation(mouseLocation)
{
}

MoveMouseEvent::~MoveMouseEvent()
{
}

Vector2D MoveMouseEvent::getMouseLocation() const
{
	return mMouseLocation;
}
