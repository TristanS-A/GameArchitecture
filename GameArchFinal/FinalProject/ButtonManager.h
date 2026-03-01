#pragma once
#include <Trackable.h>
#include <string>
#include <vector>
#include <SDL.h>
#include "SoundManager.h"
#include "Button.h"
#include "Sprite.h"

using namespace std;

class Button;

class ButtonManager : public Trackable
{
public:
	ButtonManager();
	~ButtonManager();

	void init() {};
	void cleanup();

	Button* createAndAddButton(const Button::Buttons& type, Vector2D location, Sprite* buttonSprite, Sprite* hoveredSprite, vector<string> textList, Font* font, SoundClipKey clipKey, Vector2D scale = Vector2D(1, 1));
	vector<Button*> getButtonsOfType(Button::Buttons type);
	Button* getButtonAtPos(Vector2D pos);
	void updateButtons(Vector2D mousePos);
	void setAllButtonLanguage(Button::LanguageOptions language);

private:
	vector<Button*> mButtonList;
	vector<Sprite*> mButtonSpriteList;
};

