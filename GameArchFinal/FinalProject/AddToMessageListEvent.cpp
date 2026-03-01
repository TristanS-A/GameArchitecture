#include "AddToMessageListEvent.h"

AddToMessageListEvent::AddToMessageListEvent(string message) : GameEvent( ADD_TO_MESSAGES ), mMessage(message)
{
}

AddToMessageListEvent::~AddToMessageListEvent()
{
}

string AddToMessageListEvent::getMessage() const
{
	return mMessage;
}
