#pragma once
#include "InputEvent.h"

class KeyPressEvent : public InputEvent
{
public:
	KeyPressEvent(const unsigned int keycode);
	~KeyPressEvent();

	unsigned int getKeyCode() const;

private:
	unsigned int mKeyCode;
};