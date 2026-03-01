#include "ButtonManager.h"

ButtonManager::ButtonManager()
{
}

ButtonManager::~ButtonManager()
{
	cleanup();
}

void ButtonManager::cleanup()
{
	for (Button* button : mButtonList)
	{
		delete button;
		button = nullptr;
	}

	mButtonList.clear();

	for (Sprite* spriteRef : mButtonSpriteList)
	{
		delete spriteRef;
		spriteRef = nullptr;
	}

	mButtonSpriteList.clear();
}

Button* ButtonManager::createAndAddButton(const Button::Buttons& type, Vector2D location, Sprite* buttonSprite, Sprite* hoveredSprite, vector<string> textList, Font* font, SoundClipKey clipKey, Vector2D scale)
{
	Button* newButton = new Button(type, location, buttonSprite, hoveredSprite, textList, font, clipKey, scale);
	mButtonList.push_back(newButton);

	bool foundButtonSprite = false;
	bool foundHoverSprite = false;
	for (Sprite* spriteRef : mButtonSpriteList)
	{
		if (spriteRef == buttonSprite)
		{
			foundButtonSprite = true;
		}

		if (spriteRef == hoveredSprite)
		{
			foundHoverSprite = true;
		}
	}

	if (!foundButtonSprite)
	{
		mButtonSpriteList.push_back(buttonSprite);
	}
	if (!foundHoverSprite && buttonSprite != hoveredSprite)
	{
		mButtonSpriteList.push_back(hoveredSprite);
	}

	return newButton;
}

vector<Button*> ButtonManager::getButtonsOfType(Button::Buttons type)
{
	vector<Button*> foundButtons = {};
	for (Button* button : mButtonList)
	{
		if (button->getType() == type)
		{
			foundButtons.push_back(button);
		}
	}

	return foundButtons;
}

Button* ButtonManager::getButtonAtPos(Vector2D pos)
{
	for (Button* button : mButtonList)
	{
		//Gets values necessary for determining bounds and location for unit
		Vector2D buttonPos = button->getPosition();
		Sprite* currButtonSprite = button->getCurrSprite();
		Vector2D currSpriteDimensions(static_cast<int>(currButtonSprite->getWidth()), static_cast<int>(currButtonSprite->getHeight()));

		//Checks if point is inside unit bounds
		bool isInXBounds = pos.getX() > buttonPos.getX() && pos.getX() < buttonPos.getX() + currSpriteDimensions.getX();
		bool isInYBounds = pos.getY() > buttonPos.getY() && pos.getY() < buttonPos.getY() + currSpriteDimensions.getY();

		if (isInXBounds && isInYBounds)
		{
			return button;
		}
	}

	return nullptr;
}

void ButtonManager::updateButtons(Vector2D mousePos)
{
	for (Button* button : mButtonList)
	{
		button->update(mousePos);
	}
}

void ButtonManager::setAllButtonLanguage(Button::LanguageOptions language)
{
	for (Button* button : mButtonList)
	{
		button->setCurrText(language);
	}
}
