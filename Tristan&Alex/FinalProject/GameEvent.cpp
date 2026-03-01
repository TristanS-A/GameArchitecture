#include "GameEvent.h"

GameEvent::GameEvent(GameEventTypes type) : Event ( (EventType)type )
{
}

GameEvent::~GameEvent()
{
}
