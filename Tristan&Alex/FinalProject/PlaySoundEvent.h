#pragma once
#include "GameEvent.h"

class PlaySoundEvent : public GameEvent
{
public:
	PlaySoundEvent(string soundClipKey);
	~PlaySoundEvent();

	string getSoundKey() const;

private:
	string mSoundKey;
};

