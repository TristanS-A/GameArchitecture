#include "KeyPressEvent.h"

KeyPressEvent::KeyPressEvent(const unsigned int keycode) : InputEvent ( KEY_PRESSED ), mKeyCode(keycode)
{
}

KeyPressEvent::~KeyPressEvent()
{
}

unsigned int KeyPressEvent::getKeyCode() const
{
	return mKeyCode;
}
