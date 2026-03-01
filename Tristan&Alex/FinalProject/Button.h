#pragma once
#include <Trackable.h>
#include <SDL.h>
#include <Vector2D.h>
#include "SoundManager.h"
#include <vector>

using namespace std;

class Sprite;
class GraphicsBuffer;
class Font;

class Button : public Trackable
{
public:
	enum Buttons
	{
		ATTACK_BUTTON,
		RELOAD_BUTTON,
		HEAL_BUTTON,
		START_BUTTON,
		TO_TITLE_BUTTON,
		LOAD_BUTTON,
		OPTIONS_BUTTON,
		ENGLISH_BUTTON,
		SPANISH_BUTTON,
		FRENCH_BUTTON,
		EASY_BUTTON,
		MEDIUM_BUTTON,
		HARD_BUTTON,
		TOGGLE_SOUNDS_BUTTON,
		RETRY_BUTTON,
		PAUSE_BUTTON,
		QUIT_BUTTON
	};

	enum LanguageOptions
	{
		ENGLISH,
		SPANISH,
		FRENCH
	};

	Button(Buttons buttonType, Vector2D location, Sprite* buttonSprite, Sprite* hoveredSprite, vector<string> textList, Font* font, SoundClipKey clipKey, Vector2D scale = Vector2D(1, 1));
	~Button();

	void update(Vector2D clickLocation);
	void render(GraphicsBuffer* renderBuffer);
	Sprite* getCurrSprite();
	Vector2D getPosition();
	Vector2D getDimensions();
	Buttons getType();
	void setCurrText(LanguageOptions language);
	void setAltText(vector<string> altTextList);
	void toggleAltText();
	void setIsEnabled(bool enabled) { mEnabled = enabled; };
	bool isEnabled() const { return mEnabled; };
	SoundClipKey getClickSound();

private:
	Sprite* mpButtonSprite = nullptr;
	Sprite* mpHoverSprite = nullptr;
	SDL_FRect mLocationScaleRect;
	SoundClipKey mClipKey;
	Buttons mButtonType;
	vector<string> mTextList;
	vector<string> mAltTextList;
	int mCurrTextIndex;
	Font* mpButtonTextFont = nullptr;
	bool mEnabled = true;
	Vector2D mDisplayScale;
	float mOriginalX;
	float mOriginalWidth;
	Vector2D mOriginalScale;

	Sprite* mpCurrSprite = nullptr;
};

