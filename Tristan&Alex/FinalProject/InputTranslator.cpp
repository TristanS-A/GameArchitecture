#include "InputTranslator.h"
#include <EventSystem.h>
#include "InputSystem.h"
#include "InputEvent.h"
#include "KeyPressEvent.h"
#include "MouseEvent.h"
#include "GameEvent.h"
#include "ClickButtonEvent.h"
#include "MoveMouseEvent.h"
#include "ChangeUnitAnimationEvent.h"
#include "ChangeUnitAnimationSpeedEvent.h"
#include "Animation.h"
#include "Game.h"

InputTranslator::InputTranslator()
{
	EventSystem* pEventSystemInstance = EventSystem::getInstance();
	pEventSystemInstance->addListener((EventType)InputEvent::KEY_PRESSED, this);
	pEventSystemInstance->addListener((EventType)InputEvent::MOUSE_INPUT_EVENT, this);
}

InputTranslator::~InputTranslator()
{

}

void InputTranslator::handleEvent(const Event& theEvent)
{
	if (theEvent.getType() == InputEvent::KEY_PRESSED) 
	{
		const KeyPressEvent& keyEvent = static_cast<const KeyPressEvent&>(theEvent);
		
		switch (keyEvent.getKeyCode()) 
		{
		case InputSystem::ESCAPE_KEY:
			if ((Game::getInstance()->getCurrScreen() == Game::GAME_SCREEN || Game::getInstance()->getCurrScreen() == Game::OVERWORLD_SCREEN || Game::getInstance()->getCurrScreen() == Game::GAME_OVER_SCEEN) && !Game::getGameIsPaused())
			{
				EventSystem::getInstance()->fireEvent((EventType)GameEvent::PAUSE);
			}
			else
			{
				EventSystem::getInstance()->fireEvent((EventType)GameEvent::QUIT_GAME);
			}
			break;
		case InputSystem::W_KEY:
			EventSystem::getInstance()->fireEvent((EventType)GameEvent::PLAYER_UP); // Move up
			break;
		case InputSystem::A_KEY:
			EventSystem::getInstance()->fireEvent((EventType)GameEvent::PLAYER_LEFT); // Move left
			break;
		case InputSystem::S_KEY:
			EventSystem::getInstance()->fireEvent((EventType)GameEvent::PLAYER_DOWN); // Move down
			break;
		case InputSystem::D_KEY:
			EventSystem::getInstance()->fireEvent((EventType)GameEvent::PLAYER_RIGHT); // Move right
			break;
		case InputSystem::NUM_1_KEY:
			EventSystem::getInstance()->fireEvent( ChangeUnitAnimationEvent(0) );
			break;
		case InputSystem::NUM_2_KEY:
			EventSystem::getInstance()->fireEvent( ChangeUnitAnimationEvent(1) );
			break;
		case InputSystem::SPACE_KEY:
			if (!Game::getInstance()->getIsMessages())
			{
				EventSystem::getInstance()->fireEvent((EventType)GameEvent::BEGIN_READ_TEXT);
			}
			else
			{
				EventSystem::getInstance()->fireEvent((EventType)GameEvent::READ_TEXT);
			}
		case InputSystem::ENTER_KEY:
			EventSystem::getInstance()->fireEvent( (EventType)GameEvent::TOGGLE_UNIT_ANIMATION );
			break;
		case InputSystem::LSHIFT_KEY:
			EventSystem::getInstance()->fireEvent((EventType)GameEvent::PLAYER_DASH);
			break;
		}
	}
	if (theEvent.getType() == InputEvent::MOUSE_INPUT_EVENT)
	{
		const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(theEvent);
		switch (mouseEvent.getMouseButtonCode()) 
		{
		case 1:
			EventSystem::getInstance()->fireEvent(ClickButtonEvent(mouseEvent.getMouseLocation()));
			break;
		default:
			EventSystem::getInstance()->fireEvent(MoveMouseEvent(mouseEvent.getMouseLocation()));
			break;
		}
	}
}
