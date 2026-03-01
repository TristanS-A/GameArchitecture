#pragma once
#include <EventListener.h>
#include <unordered_map>
#include <SDL_mixer.h>

using namespace std;

typedef string SoundClipKey;

class SoundManager : public EventListener
{
public:
	SoundManager();
	~SoundManager();

	void loadAndAddSound(SoundClipKey key, string filename);
	bool init();
	void cleanup();

	bool getSoundOn() { return mSoundOn; };

	void handleEvent(const Event& theEvent);

private:
	void playSound(SoundClipKey key, float volume = 1.0, float speed = 1.0, bool loop = false);
	void stopSound(SoundClipKey key);
	void stopAllSounds();

	unordered_map<SoundClipKey, Mix_Chunk*> mClipList;
	unordered_map<SoundClipKey, int> mClipChannelList;

	bool mSoundOn = true;
};

