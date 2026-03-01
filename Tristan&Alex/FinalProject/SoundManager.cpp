#include "SoundManager.h"
#include "GameEvent.h"
#include "ChangeUnitAnimationEvent.h"
#include "ChangeUnitAnimationSpeedEvent.h"
#include "ClickButtonEvent.h"
#include "MoveMouseEvent.h"
#include "SubtractPointsEvent.h"
#include "PlaySoundEvent.h"
#include "Game.h"
#include "UnitManager.h"

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
	cleanup();
}

bool SoundManager::init()
{
	//Sets up SDL_Mixer Audio
	int audio_rate = 22050;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 1000;

	//Initializes/Opens mixer audio
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		return false;
	}

	//Allocates audio channels to make sure that all sounds play
	Mix_AllocateChannels(30);

	//Sets mixer volume to max
	Mix_MasterVolume(MIX_MAX_VOLUME);

	EventSystem* theEventSystem = EventSystem::getInstance();
	theEventSystem->addListener((EventType)GameEvent::START_GAME, this);
	theEventSystem->addListener((EventType)GameEvent::LOAD_GAME, this);
	theEventSystem->addListener((EventType)GameEvent::GO_TO_TITLE_SCREEN, this);
	theEventSystem->addListener((EventType)GameEvent::GO_TO_OVERWORLD, this);
	theEventSystem->addListener((EventType)GameEvent::CLICK_BUTTON, this);
	theEventSystem->addListener((EventType)GameEvent::PAUSE, this);
	theEventSystem->addListener((EventType)GameEvent::UNPAUSE, this);
	theEventSystem->addListener((EventType)GameEvent::SUBTRACT_POINTS, this);
	theEventSystem->addListener((EventType)GameEvent::PLAY_SOUND, this);
	theEventSystem->addListener((EventType)GameEvent::TOGGLE_SOUND, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_DASH, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_BATTLE, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_DIES, this);

	return true;
}

void SoundManager::cleanup()
{
	for (auto iterator : mClipList)
	{
		Mix_Chunk* sample = iterator.second;
		Mix_FreeChunk(sample);
		iterator.second = nullptr;
	}

	mClipList.clear();

	Mix_CloseAudio();
}

void SoundManager::handleEvent(const Event& theEvent)
{
	EventType theType = theEvent.getType();
	if (theType == GameEvent::START_GAME || theType == GameEvent::LOAD_GAME) 
	{
 		stopSound("titleMusic");
	}
	else if (theType == GameEvent::GO_TO_TITLE_SCREEN)
	{
		if (Game::getInstance()->getCurrScreen() != Game::OPTIONS_SCREEN)
		{
			stopAllSounds();
			playSound("titleMusic", 1.0, 1.0, true);
		}
	}
	else if (theType == GameEvent::BEGIN_BATTLE)
	{
		stopAllSounds();
		playSound("battleMusic", 1.0, 1.0, true);
	}
	else if (theType == GameEvent::GO_TO_OVERWORLD)
	{
		stopAllSounds();
		playSound("worldMusic", 1.0, 1.0, true);
	}
	else if (theType == GameEvent::PAUSE)
	{
		playSound("pauseSound");
	}
	else if (theType == GameEvent::UNPAUSE)
	{
		playSound("unpauseSound");
	}
	else if (theType == GameEvent::PLAY_SOUND)
	{
		const PlaySoundEvent& soundEvent = static_cast<const PlaySoundEvent&>(theEvent);
		playSound(soundEvent.getSoundKey());
	}
	else if (theType == GameEvent::TOGGLE_SOUND)
	{
		mSoundOn = !mSoundOn;

		if (!mSoundOn)
		{
			stopAllSounds();
		}
	}
	else if (theType == GameEvent::PLAYER_DIES)
	{
		stopAllSounds();
		playSound("badSound");
	}
}

	void SoundManager::loadAndAddSound(SoundClipKey key, string filename)
	{
		auto iterator = mClipList.find(key);

		if (iterator == mClipList.end())
		{
			mClipList[key] = Mix_LoadWAV((filename).c_str());
			mClipChannelList[key] = 0;
		}
	}

	void SoundManager::playSound(SoundClipKey key, float volume, float speed, bool loop)
	{
		if (mSoundOn)
		{
			auto iterator = mClipList.find(key);

			if (iterator != mClipList.end())
			{
				if (loop)
				{
					mClipChannelList[key] = Mix_PlayChannel(-1, mClipList[key], -1);
				}
				else
				{
					mClipChannelList[key] = Mix_PlayChannel(-1, mClipList[key], 0);
				}
			}
		}
	}

	//This is a bit dangerous because it could stop other sounds on that channel
	void SoundManager::stopSound(SoundClipKey key)
	{
		auto iterator = mClipList.find(key);

		if (iterator != mClipList.end())
		{
			Mix_FadeOutChannel(mClipChannelList[key], 0);
		}
	}

	void SoundManager::stopAllSounds()
	{
		for (unsigned int i = 0; i < 2; i++) 
		{
			Mix_FadeOutChannel(-1, 0);
		}
	}
