#pragma once
#include "GameEvent.h"

class AddToMessageListEvent : public GameEvent
{
public:
	AddToMessageListEvent(string message);
	~AddToMessageListEvent();

	string getMessage() const;
private:
	string mMessage;
};

