#include "InputSystem.h"
#include <EventSystem.h>
#include "InputEvent.h"
#include "KeyPressEvent.h"
#include "MouseEvent.h"

using namespace std;

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	cleanup();
}

bool InputSystem::init()
{
	mKeystates = { SDL_GetKeyboardState(nullptr) };
	return true;
}

void InputSystem::cleanup()
{
	mKeyMap.clear();
	mKeyList.clear();
}

void InputSystem::updateEventQueue()
{
	SDL_Event theEvent;

	while (SDL_PollEvent(&theEvent)) 
	{
		if (theEvent.type == SDL_KEYDOWN)
		{
			if (find(mKeyList.begin(), mKeyList.end(), theEvent.key.keysym.sym) == mKeyList.end())
			{
				mKeyList.push_back(theEvent.key.keysym.sym);
			}
		}
		else if (theEvent.type == SDL_KEYUP)
		{
			auto iterator = find(mKeyList.begin(), mKeyList.end(), theEvent.key.keysym.sym);

			if (iterator != mKeyList.end())
			{
				mKeyList.erase(iterator);
			}
		}
		else if (theEvent.type == SDL_MOUSEBUTTONDOWN)
		{
			Vector2D mousePos(theEvent.motion.x, theEvent.motion.y);
			EventSystem::getInstance()->fireEvent(MouseEvent(theEvent.button.button, mousePos));
		}
		else if (theEvent.type == SDL_MOUSEMOTION)
		{
			Vector2D mousePos(theEvent.motion.x, theEvent.motion.y);
			EventSystem::getInstance()->fireEvent(MouseEvent(0, mousePos)); //There is a zero here so it only tracks the movement
		}
	}
	if (mKeyList.size() > 0)
	{
		for (unsigned int key : mKeyList)
		{
			EventSystem::getInstance()->fireEvent(KeyPressEvent(key));

			//This gives a pause before regestering the key as down (important so the game doesn't imediately close instead of just pausing)
			if (key == ESCAPE_KEY) 
			{
				auto iterator = find(mKeyList.begin(), mKeyList.end(), key);

				if (iterator != mKeyList.end())
				{
					mKeyList.erase(iterator);
				}
			}
			else if (key == LSHIFT_KEY)
			{
				auto iterator = find(mKeyList.begin(), mKeyList.end(), key);

				if (iterator != mKeyList.end())
				{
					mKeyList.erase(iterator);
				}
			}
			else if (key == SPACE_KEY)
			{
				auto iterator = find(mKeyList.begin(), mKeyList.end(), key);

				if (iterator != mKeyList.end())
				{
					mKeyList.erase(iterator);
				}
			}
		}
	}
}

void InputSystem::getKeyState()
{
	//Nothing in sdl
}

bool InputSystem::getKeyPressed(int keyCode)
{
	//Updates key state
	getKeyState();
	return mKeystates[keyCode];
}

bool InputSystem::getKeyDown(KeyScanCodes keyCode)
{
	//Uses unordered_map to store pressed values 
	auto iterator = mKeyMap.find(keyCode);

	//Creates unordered_map element for new key if the key doesn't exist
	if (iterator == mKeyMap.end())
	{
		mKeyMap[keyCode] = false;
		iterator = mKeyMap.find(keyCode);
	}

	//Checks key value for pressed
	if (!iterator->second)
	{
		bool hasPressed = getKeyPressed(keyCode);
		iterator->second = hasPressed;
		return hasPressed;
	}
	else
	{
		iterator->second = getKeyPressed(keyCode);
	}

	return false;
}

Uint32 InputSystem::getMouseState()
{
	int mouseX, mouseY;

	Uint32 mouseButton = SDL_GetMouseState(&mouseX, &mouseY);

	mMousePos = Vector2D(mouseX, mouseY);

	return mouseButton;
}

bool InputSystem::getMousePressed(Mouse_Buttons button)
{
	//Updates mouse state
	return getMouseState() & button;
}

Vector2D InputSystem::getMousePos()
{
	return mMousePos;
}
