#include "UIMenu.h"
#include "GraphicsBuffer.h"
#include "Sprite.h"
#include "Game.h"

UIMenu::UIMenu(MenuType type) : mType(type)
{
}

UIMenu::~UIMenu()
{
	cleanup();
}

void UIMenu::init()
{
}

void UIMenu::cleanup()
{
}

void UIMenu::addButton(Button* button)
{
	mButtonList.push_back(button);
}

void UIMenu::addUISprite(Sprite sprite, string spriteKey)
{
	if (mSpriteList.find(spriteKey) == mSpriteList.end())
	{
		mSpriteList[spriteKey] = sprite;
	}
}

Button* UIMenu::getButtonAtPoint(Vector2D point)
{
	for (Button* button : mButtonList) 
	{
		if (button->isEnabled())
		{
			//Gets values necessary for determining bounds and location for unit
			Vector2D buttonPos = button->getPosition();
			Vector2D buttonDimensions = button->getDimensions();

			//Checks if point is inside unit bounds
			bool isInXBounds = point.getX() > buttonPos.getX() && point.getX() < buttonPos.getX() + buttonDimensions.getX();
			bool isInYBounds = point.getY() > buttonPos.getY() && point.getY() < buttonPos.getY() + buttonDimensions.getY();

			if (isInXBounds && isInYBounds)
			{
				return button;
			}
		}
	}

	return nullptr;
}

UIMenu::MenuType UIMenu::getType()
{
	return mType;
}

Button* UIMenu::getButtonbyType(Button::Buttons buttonType)
{
	for (Button* button : mButtonList)
	{
		if (button->getType() == buttonType)
		{
			return button;
		}
	}

	return nullptr;
}

Sprite* UIMenu::getSprite(string key)
{
	if (mSpriteList.find(key) != mSpriteList.end())
	{
		return &mSpriteList[key];
	}

	return nullptr;
}

void UIMenu::updateMenu(Vector2D mousePos)
{
	for (Button* button : mButtonList) 
	{
		button->update(mousePos);
	}
}

void UIMenu::renderMenu(GraphicsBuffer* renderBuffer)
{
	for (auto i = mSpriteList.begin(); i != mSpriteList.end(); i++)
	{
		if (i->second.isEnabled())
		{
			renderBuffer->draw(i->second.getDisplayLocation(), i->second, 1.0);
		}
	}

	for (Button* button : mButtonList)
	{
		if (button->isEnabled())
		{
			button->render(renderBuffer);
		}
	}
}

string UIMenu::getSaveData()
{
	string data = "";

	data += "UI_Sprites_Enabled_Data:\n";
	for (auto i = mSpriteList.begin(); i != mSpriteList.end(); i++)
	{
		data += "Sprite: " + i->first;
		data += "\nEnabled: " + to_string(i->second.isEnabled());
		data += "\n";
	}

	data += "UI_Buttons_Enabled_Data:";
	for (Button* button : mButtonList)
	{
		data += " " + to_string(button->isEnabled());
	}

	return data;
}

void UIMenu::loadData(string& fileData)
{
	string battleMenuSpritesData = Game::getInstance()->extractDataFromDataString("UI_Sprites_Enabled_Data: ", fileData);

	string splitter = "Sprite: ";
	int stringPos = fileData.find(splitter);
	while (stringPos != string::npos)
	{
		string spriteKey = Game::getInstance()->extractDataFromDataString("Sprite: ", fileData);
		bool spriteEnabled = static_cast<bool>(stoi(Game::getInstance()->extractDataFromDataString("Enabled: ", fileData)));

		getSprite(spriteKey)->setIsEnabled(spriteEnabled);
		stringPos = fileData.find(splitter);
	}

	string battleMenuButtonsData = Game::getInstance()->extractDataFromDataString("UI_Buttons_Enabled_Data: ", fileData);

	splitter = " ";
	stringPos = battleMenuButtonsData.find(splitter);
	int index = 0;

	while (stringPos != string::npos)
	{
		string subStrData = battleMenuButtonsData.substr(0, stringPos);
		mButtonList[index]->setIsEnabled(static_cast<bool>(stoi(subStrData)));
		index++;

		battleMenuButtonsData.erase(0, stringPos + splitter.length());
		stringPos = battleMenuButtonsData.find(splitter);
	}

	if (battleMenuButtonsData.length() > 0) //This gets the last value in the string since the while loop checks for the \n afteer the value
	{
		string subStrData = battleMenuButtonsData.substr(0, stringPos);
		mButtonList[index]->setIsEnabled(static_cast<bool>(stoi(subStrData)));
	}
}
