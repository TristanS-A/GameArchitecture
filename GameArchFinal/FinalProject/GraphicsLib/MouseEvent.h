#pragma once
#include "InputEvent.h"
#include <Vector2D.h>

class MouseEvent : public InputEvent
{
public:
	MouseEvent(unsigned int mouseButtonCode, Vector2D mouseLocation);
	~MouseEvent();

	unsigned int getMouseButtonCode() const;
	Vector2D getMouseLocation() const;

private:
	unsigned int mMouseButtonCode;
	Vector2D mMouseLocation;
};