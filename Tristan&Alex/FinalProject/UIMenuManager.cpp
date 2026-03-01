#include "UIMenuManager.h"
#include <EventSystem.h>
#include "GameEvent.h"
#include "ClickButtonEvent.h"
#include "MoveMouseEvent.h"
#include "AddToMessageListEvent.h"
#include "PlaySoundEvent.h"
#include "ChangeLanguageEvent.h"
#include "Game.h"

UIMenuManager::UIMenuManager()
{
	init();
}

UIMenuManager::~UIMenuManager()
{
	cleanup();
}

void UIMenuManager::init()
{
	EventSystem* theEventSystem = EventSystem::getInstance();
	theEventSystem->addListener((EventType)GameEvent::CLICK_BUTTON, this);
	theEventSystem->addListener((EventType)GameEvent::MOVE_MOUSE, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_BATTLE, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_PLAYER_TURN, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_ENEMY_TURN, this);
	theEventSystem->addListener((EventType)GameEvent::READ_TEXT, this);
	theEventSystem->addListener((EventType)GameEvent::ADD_TO_MESSAGES, this);
	theEventSystem->addListener((EventType)GameEvent::CHANGE_LANGUAGE, this);
	theEventSystem->addListener((EventType)GameEvent::TOGGLE_SOUND, this);

	mpButtonManager = new ButtonManager();
}

void UIMenuManager::cleanup()
{
	for (auto iterator : mMenuMap)
	{
		delete iterator.second;
		iterator.second = nullptr;
	}

	mMenuMap.clear();

	delete mpButtonManager;
	mpButtonManager = nullptr;
}

void UIMenuManager::createAndAddNewMenu(UIMenu::MenuType type)
{
	auto iterator = mMenuMap.find(type);

	if (iterator == mMenuMap.end())
	{
		mMenuMap[type] = new UIMenu(type);
	}
}

void UIMenuManager::addButtonToCurrMenu(const Button::Buttons& key, Vector2D location, Sprite* buttonSprite, Sprite* hoveredSprite, Font* font, SoundClipKey clipKey, vector<string> textList, vector<string> alTextList, Vector2D scale)
{
	Button* newButton = mpButtonManager->createAndAddButton(key, location, buttonSprite, hoveredSprite, textList, font, clipKey, scale);

	newButton->setAltText(alTextList);

	mpCurrMenu->addButton(newButton);
}

void UIMenuManager::addUISpriteToCurrMenu(Sprite sprite, string spriteKey)
{
	mpCurrMenu->addUISprite(sprite, spriteKey);
}

void UIMenuManager::deleteMenu(UIMenu::MenuType type)
{
	auto iterator = mMenuMap.find(type);

	if (iterator != mMenuMap.end())
	{
		delete iterator->second;
		mMenuMap.erase(iterator);
	}
}

void UIMenuManager::setCurrMenu(UIMenu::MenuType type)
{
	if (type == UIMenu::NO_MENU)
	{
		mpCurrMenu = nullptr;
		return;
	}

	auto iterator = mMenuMap.find(type);

	if (iterator != mMenuMap.end())
	{
		mpCurrMenu = iterator->second;
	}
}

UIMenu::MenuType UIMenuManager::getCurrMenu()
{
	if (mpCurrMenu != nullptr)
	{
		return mpCurrMenu->getType();
	}

	return UIMenu::NO_MENU;
}

void UIMenuManager::addToMessageList(string message) //Convert this to event
{
	if (mMessageList.size() == 0)
	{
		if (mpCurrMenu != nullptr && mpCurrMenu->getType() == UIMenu::BATTLE_MENU)
		{
			setPlayerAttackMenuEnabled(false);
		}
	}
	mMessageList.push_back(message);
}

void UIMenuManager::removeFirstFromMessageList()
{
	if (getIsMessages())
	{
		auto iterator = find(mMessageList.begin(), mMessageList.end(), mMessageList[0]);

		if (iterator != mMessageList.end())
		{
			mMessageList.erase(iterator);
		}

		if (!getIsMessages())
		{
			if (mpCurrMenu != nullptr && mpCurrMenu->getType() == UIMenu::BATTLE_MENU)
			{
				setPlayerAttackMenuEnabled(true);
			}
		}
	}
}

string UIMenuManager::getCurrMessage()
{
	if (mMessageList.size() > 0)
	{
		return mMessageList[0];
	}

	return "";
}

bool UIMenuManager::getIsMessages()
{
	return mMessageList.size() > 0;
}

void UIMenuManager::renderCurrMenu(GraphicsBuffer* renderBuffer)
{
	if (mpCurrMenu != nullptr)
	{
		mpCurrMenu->renderMenu(renderBuffer);
	}
}

string UIMenuManager::getSaveData()
{
	string data = "";
	UIMenu* battleMenu = getMenu(UIMenu::BATTLE_MENU);

	data += "\n\nUI_Data:\n\n";
	data += "Curr_UIMenu: " + to_string(getCurrMenu());
	data += "\n\nBattle_Menu_Data:\n" + battleMenu->getSaveData() + "\n\n"; //This is to save what buttons are enabled. If other menus disable things then that will have to be saved like this too
	data += "Curr_Messages:\n" + getMessagesSaveData();
	data += "\n\n";
	return data;
}

void UIMenuManager::loadData(string& fileData)
{
	while (fileData.length() > 0)
	{
		string splitter;
		splitter = '\n';
		int stringPos = fileData.find(splitter);
		string subStrData = fileData.substr(0, stringPos);

		if (stringPos != string::npos)
		{
			fileData.erase(0, stringPos + splitter.length());
		}
		else
		{
			fileData.erase(0);
		}

		if (subStrData.find("Curr_UIMenu: ") != string::npos)
		{
			UIMenu::MenuType currUIMenu = static_cast<UIMenu::MenuType>(stoi(Game::getInstance()->extractDataFromDataString("Curr_UIMenu: ", subStrData)));
			setCurrMenu(currUIMenu);
		}
		else if (subStrData.find("Battle_Menu_Data:") != string::npos)
		{
			string battleMenuData;

			splitter = "\n\n";
			stringPos = fileData.find(splitter);
			battleMenuData = fileData.substr(0, stringPos);
			getMenu(UIMenu::BATTLE_MENU)->loadData(battleMenuData);
			fileData.erase(0, stringPos + splitter.length());
		}
		else if (subStrData.find("Curr_Messages:") != string::npos)
		{
			string messages = "";
			splitter = "\n\n";
			stringPos = fileData.find(splitter);
			messages += fileData.substr(0, stringPos);

			string message;
			splitter = "\n";
			stringPos = messages.find(splitter);

			while (messages.length() > 1)
			{
				message = messages.substr(0, stringPos);
				mMessageList.push_back(message);

				if (stringPos != string::npos)
				{
					messages.erase(0, stringPos + splitter.length());
				}
				else
				{
					messages.erase(0);
				}

				stringPos = messages.find(splitter);
			}
		}
	}
}

string UIMenuManager::getMessagesSaveData()
{
	string messageData = "";
	for (unsigned int i = 0; i < mMessageList.size(); i++)
	{
		messageData += mMessageList[i] += "\n";
	}

	return messageData;
}

void UIMenuManager::handleEvent(const Event& theEvent)
{
	EventType theType = theEvent.getType();
	if (theType == GameEvent::CLICK_BUTTON && mpCurrMenu != nullptr )
	{
		const ClickButtonEvent& clickButtonEvent = static_cast<const ClickButtonEvent&>(theEvent);

		Button* button = mpCurrMenu->getButtonAtPoint(clickButtonEvent.getClickPos());
		if (button != nullptr)
		{
			EventSystem::getInstance()->fireEvent(PlaySoundEvent(button->getClickSound())); //Plays button sound

			//Handles the button press on the specific menu that the button is on
			if (mpCurrMenu->getType() == UIMenu::BATTLE_MENU)
			{
				if (button->getType() == Button::ATTACK_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::BEGIN_ENEMY_TURN);
				}
				else if (button->getType() == Button::RELOAD_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::PLAYER_RELOADS);
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::BEGIN_ENEMY_TURN);
					EventSystem::getInstance()->fireEvent(PlaySoundEvent("chargeSound"));
				}
				else if (button->getType() == Button::HEAL_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::PLAYER_HEALS);
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::BEGIN_ENEMY_TURN);
					EventSystem::getInstance()->fireEvent(PlaySoundEvent("healSound"));
				}
			}
			else if (mpCurrMenu->getType() == UIMenu::TITLE_MENU)
			{
				if (button->getType() == Button::START_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::START_GAME);
				}
				else if (button->getType() == Button::LOAD_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::LOAD_GAME);
				}
				else if (button->getType() == Button::OPTIONS_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::GO_TO_OPTIONS);
				}
				else if (button->getType() == Button::QUIT_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::QUIT_GAME);
				}
			}
			else if (mpCurrMenu->getType() == UIMenu::OPTIONS_MENU)
			{
				if (button->getType() == Button::TO_TITLE_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::GO_TO_TITLE_SCREEN);
				}
				else if (button->getType() == Button::TOGGLE_SOUNDS_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::TOGGLE_SOUND);
				}
				else if (button->getType() == Button::EASY_BUTTON)
				{
					Game::getInstance()->switchDifficulty(Game::EASY);
				}
				else if (button->getType() == Button::MEDIUM_BUTTON)
				{
					Game::getInstance()->switchDifficulty(Game::MEDIUM);
				}
				else if (button->getType() == Button::HARD_BUTTON)
				{
					Game::getInstance()->switchDifficulty(Game::HARD);
				}
				else if (button->getType() == Button::ENGLISH_BUTTON)
				{
					EventSystem::getInstance()->fireEvent(ChangeLanguageEvent(Button::ENGLISH));
				}
				else if (button->getType() == Button::SPANISH_BUTTON)
				{
					EventSystem::getInstance()->fireEvent(ChangeLanguageEvent(Button::SPANISH));
				}
				else if (button->getType() == Button::FRENCH_BUTTON)
				{
					EventSystem::getInstance()->fireEvent(ChangeLanguageEvent(Button::FRENCH));
				}
			}
			else if (mpCurrMenu->getType() == UIMenu::GAME_OVER_MENU)
			{
				if (button->getType() == Button::RETRY_BUTTON)
				{
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::RESTART_BATTLE);
				}
			}
			else if (mpCurrMenu->getType() == UIMenu::PAUSE_MENU)
			{
				if (button->getType() == Button::PAUSE_BUTTON)
				{
					if (Game::getInstance()->getGameIsPaused())
					{
						EventSystem::getInstance()->fireEvent((EventType)GameEvent::UNPAUSE);
					}
				}
				else if (button->getType() == Button::TO_TITLE_BUTTON)
				{
					Game::getInstance()->saveGame();
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::GO_TO_TITLE_SCREEN);
				}
			}
			else if (mpCurrMenu->getType() == UIMenu::WIN_GAME_MENU)
			{
				if (button->getType() == Button::TO_TITLE_BUTTON)
				{
					Game::getInstance()->saveGame();
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::GO_TO_TITLE_SCREEN);
				}
			}
		}
	}
	else if (theType == GameEvent::MOVE_MOUSE && mpCurrMenu != nullptr)
	{
		const MoveMouseEvent& moveMouseEvent = static_cast<const MoveMouseEvent&>(theEvent);

		mpCurrMenu->updateMenu(moveMouseEvent.getMouseLocation());
	}
	else if (theType == GameEvent::BEGIN_BATTLE)
	{
		setCurrMenu(UIMenu::BATTLE_MENU);
	}
	else if (theType == GameEvent::BEGIN_PLAYER_TURN)
	{
		setPlayerAttackMenuEnabled(true);
	}
	else if (theType == GameEvent::BEGIN_ENEMY_TURN)
	{
		setPlayerAttackMenuEnabled(false);
	}
	else if (theType == GameEvent::READ_TEXT)
	{
		removeFirstFromMessageList();
	}
	else if (theType == GameEvent::ADD_TO_MESSAGES)
	{
		const AddToMessageListEvent& messageAddEvent = static_cast<const AddToMessageListEvent&>(theEvent);

		addToMessageList(messageAddEvent.getMessage());
	}
	else if (theType == GameEvent::CHANGE_LANGUAGE)
	{
		const ChangeLanguageEvent& changeLanguageEvent = static_cast<const ChangeLanguageEvent&>(theEvent);
		mpButtonManager->setAllButtonLanguage(changeLanguageEvent.getLanguageToChangeTo());
	}
	else if (theType == GameEvent::TOGGLE_SOUND)
	{
		vector<Button*> soundButtons = mpButtonManager->getButtonsOfType(Button::TOGGLE_SOUNDS_BUTTON);
		for (Button* soundButton : soundButtons)
		{
			soundButton->toggleAltText();
		}
	}
}

UIMenu* UIMenuManager::getMenu(UIMenu::MenuType type)
{
	auto iterator = mMenuMap.find(type);

	if (iterator != mMenuMap.end())
	{
		return iterator->second;
	}

	return nullptr;
}

void UIMenuManager::setPlayerAttackMenuEnabled(bool enabled)
{
	UIMenu* battleMenu = getMenu(UIMenu::BATTLE_MENU);

	if (battleMenu != nullptr)
	{
		Button* attackButton = battleMenu->getButtonbyType(Button::ATTACK_BUTTON);
		if (attackButton != nullptr)
		{
			attackButton->setIsEnabled(enabled);
		}

		Button* reloadButton = battleMenu->getButtonbyType(Button::RELOAD_BUTTON);
		if (reloadButton != nullptr)
		{
			reloadButton->setIsEnabled(enabled);
		}

		Button* healButton = battleMenu->getButtonbyType(Button::HEAL_BUTTON);
		if (healButton != nullptr)
		{
			healButton->setIsEnabled(enabled);
		}

		battleMenu->getSprite("textBox")->setIsEnabled(enabled);
	}
}
