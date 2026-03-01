#include "PlaySoundEvent.h"

PlaySoundEvent::PlaySoundEvent(string soundClipKey) : GameEvent( PLAY_SOUND ), mSoundKey(soundClipKey)
{
}

PlaySoundEvent::~PlaySoundEvent()
{
}

string PlaySoundEvent::getSoundKey() const
{
	return mSoundKey;
}
