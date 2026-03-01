#include "MouseEvent.h"

MouseEvent::MouseEvent(unsigned int mouseButtonCode, Vector2D mouseLocation) : InputEvent(MOUSE_INPUT_EVENT), mMouseButtonCode(mouseButtonCode), mMouseLocation(mouseLocation)
{
}

MouseEvent::~MouseEvent()
{
}

unsigned int MouseEvent::getMouseButtonCode() const
{
	return mMouseButtonCode;
}

Vector2D MouseEvent::getMouseLocation() const
{
	return mMouseLocation;
}
