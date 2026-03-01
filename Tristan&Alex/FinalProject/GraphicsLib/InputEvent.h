#pragma once
#include <Event.h>

class InputEvent : public Event
{
public:
	enum InputEventTypes
	{
		KEY_PRESSED = NUM_BASE_EVENT_TYPES,
		MOUSE_INPUT_EVENT,
		NUM_INPUT_EVENTS
	};

	InputEvent(InputEventTypes type);
	~InputEvent();
private:
};