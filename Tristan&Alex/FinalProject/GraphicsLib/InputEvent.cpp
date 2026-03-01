#include "InputEvent.h"


InputEvent::InputEvent(InputEventTypes type) : Event ( (EventType)type )
{
}

InputEvent::~InputEvent()
{
}
