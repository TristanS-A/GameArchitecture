#include "Player.h"
#include <EventSystem.h>
#include "Animation.h"
#include <Event.h>
#include "GameEvent.h"
#include "FireGunEvent.h"
#include "ClickButtonEvent.h"
#include "PlaySoundEvent.h"
#include "ParticlePool.h"
#include "Game.h"

Player::Player(vector<Animation> animations, float animationSpeed, int startingAniIndex, Vector2D location) :
	Unit(animations, animationSpeed, startingAniIndex, location)
{
	mCanMoveOnX = true;
	mCanMoveOnY = true;
	mCanDash = true;
	mPlayerMotionLocked = false;

	init();
}

Player::~Player()
{
	cleanup();
}

void Player::init()
{
	EventSystem* theEventSystem = EventSystem::getInstance();
	theEventSystem->addListener((EventType)GameEvent::PLAYER_UP, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_RIGHT, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_DOWN, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_LEFT, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_DASH, this);
	theEventSystem->addListener((EventType)GameEvent::PAUSE, this);
	theEventSystem->addListener((EventType)GameEvent::UNPAUSE, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_PLAYER_TURN, this);
	theEventSystem->addListener((EventType)GameEvent::ENTER_SHOOTING_MODE, this);
	theEventSystem->addListener((EventType)GameEvent::RELOCATE_BATTLING_UNITS, this);
	theEventSystem->addListener((EventType)GameEvent::CLICK_BUTTON, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_DIES, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_RELOADS, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_HEALS, this);
	theEventSystem->addListener((EventType)GameEvent::LOCK_PLAYER_MOVEMENT, this);
	theEventSystem->addListener((EventType)GameEvent::UNLOCK_PLAYER_MOVEMENT, this);

	loadPlayerData();

	srand((unsigned)time(NULL));
}

void Player::cleanup()
{

}

void Player::handleEvent(const Event& theEvent)
{
	EventType theType = theEvent.getType();

	if (Game::getInstance()->getCurrScreen() == Game::GAME_SCREEN || Game::getInstance()->getCurrScreen() == Game::GAME_OVER_SCEEN || Game::getInstance()->getCurrScreen() == Game::OVERWORLD_SCREEN)
	{
		if (theType == GameEvent::RELOCATE_BATTLING_UNITS || theType == GameEvent::PLAYER_DIES)
		{
			mCanMoveOnX = false;
			mCanMoveOnY = false;
			mCanDash = false;
			mCanShoot = false;
		}
		else if (theType == GameEvent::BEGIN_PLAYER_TURN)
		{
			mNumberOfBullets += mNumberOfBulletsPerTurn;
			mSkipNextTurn = false; //This resets the skip turn so that it always starts as the player turn
		}
		else if (theType == GameEvent::ENTER_SHOOTING_MODE)
		{
			mCanMoveOnX = true;
			mCanMoveOnY = true;
			mCanDash = true;

			if (!mSkipNextTurn)
			{
				mCanShoot = true;
			}
			else
			{
				mCanShoot = false;
			}
		}
		else if (theType == GameEvent::PLAYER_RELOADS)
		{
			mNumberOfBullets += mNumberOfBulletsPerTurn;
			mSkipNextTurn = true;
		}
		else if (theType == GameEvent::PLAYER_HEALS)
		{
			mHealth += 5;
			if (mHealth > mMaxHealth)
			{
				mHealth = mMaxHealth;
			}

			mSkipNextTurn = true;
		}
		else if (theType == GameEvent::LOCK_PLAYER_MOVEMENT)
		{
			mCanMoveOnX = false;
			mCanMoveOnY = false;
			mPlayerMotionLocked = true;
		}
		else if (theType == GameEvent::UNLOCK_PLAYER_MOVEMENT)
		{
			mCanMoveOnX = true;
			mCanMoveOnY = true;
			mPlayerMotionLocked = false;
		}

		if (!Game::getGameIsPaused())
		{
			if (theType == GameEvent::PLAYER_UP || theType == GameEvent::PLAYER_DOWN || theType == GameEvent::PLAYER_RIGHT || theType == GameEvent::PLAYER_LEFT)
			{
				switch (theType)
				{
				case GameEvent::PLAYER_UP:
					playerMoveUp();
					break;
				case GameEvent::PLAYER_DOWN:
					playerMoveDown();
					break;
				case GameEvent::PLAYER_RIGHT:
					playerMoveRight();
					break;
				case GameEvent::PLAYER_LEFT:
					playerMoveLeft();
					break;
				}

				handleStayOnScreen(Game::getDisplayDimensions());
			}
			else if (theType == GameEvent::PLAYER_DASH)
			{
				playerDash();
			}

			if (mCanShoot)
			{
				if (theType == GameEvent::CLICK_BUTTON)
				{
					fire(theEvent);
				}
			}
		}
	}
}

void Player::playerMoveUp()
{
	mVelocity = Vector2D(0.0f, -mSpeed);
	if (mCanMoveOnY)
	{
		mLocation += mVelocity;
	}
}

void Player::playerMoveRight()
{
	mVelocity = Vector2D(mSpeed, 0.0f);
	if (mCanMoveOnX)
	{
		mLocation += mVelocity;
	}
}

void Player::playerMoveDown()
{
	mVelocity = Vector2D(0.0f, mSpeed);
	if (mCanMoveOnY)
	{
		mLocation += mVelocity;
	}
}

void Player::playerMoveLeft()
{
	mVelocity = Vector2D(-mSpeed, 0.0f);
	if (mCanMoveOnX)
	{
		mLocation += mVelocity;
	}
}

void Player::playerDash()
{
	if (mCanDash)
	{
		EventSystem::getInstance()->fireEvent(PlaySoundEvent("teleportSound"));
		spawnTeleportParticles(Game::getInstance()->getPlayerParticlePool());
		mVelocity.normalize();
		mVelocity = mVelocity * (float)mDashDistence;
		mLocation += mVelocity;
		handleStayOnScreen(Game::getDisplayDimensions());
		spawnTeleportParticles(Game::getInstance()->getPlayerParticlePool());
	}
}

void Player::handleStayOnScreen(Vector2D gameWindowDimension)
{
	float xPos = mLocation.getX();
	float yPos = mLocation.getY();
	float xRightScreenBorderPos = gameWindowDimension.getX() - getCurrAnimation()->getCurrentSprite()->getWidth();
	float yBottomScreenBorderPos = gameWindowDimension.getY() - getCurrAnimation()->getCurrentSprite()->getHeight();

	if (xPos < 0)
	{
		mLocation.setX(0);
	}
	else if (xPos > xRightScreenBorderPos)
	{
		mLocation.setX(xRightScreenBorderPos);
	}

	if (yPos < 0)
	{
		mLocation.setY(0);
	}
	else if (yPos > yBottomScreenBorderPos)
	{
		mLocation.setY(yBottomScreenBorderPos);
		}
}

bool Player::getIsPlayerEmpty()
{
	return mNumberOfBullets <= 0 || mSkipNextTurn;
}

void Player::fire(const Event& theEvent)
{
	if (mNumberOfBullets > 0)
	{
		EventSystem::getInstance()->fireEvent(PlaySoundEvent("zapSound"));
		const Sprite* currSprite = getCurrAnimation()->getCurrentSprite();
		Vector2D shotFromPos = mLocation + Vector2D((float)currSprite->getWidth(), currSprite->getHeight() / 2.0f);
		const ClickButtonEvent& clickButtonEvent = static_cast<const ClickButtonEvent&>(theEvent);
		EventSystem::getInstance()->fireEvent(FireGunEvent(clickButtonEvent.getClickPos(), shotFromPos, true));
		mNumberOfBullets--;
	}
}

void Player::addToPlayerHealth(int amountToAdd)
{
	mHealth += amountToAdd;
}

void Player::resetPlayer(int health = 0)
{
	mNumberOfBullets = 0;
	if (health != 0)
	{
		mHealth = health;
	}
	else 
	{
		mHealth = mMaxHealth;
	}
}

void Player::spawnTeleportParticles(ParticlePool* particlePool)
{
	for (int i = 0; i < mNumParticlesPerTeleportation; i++)
	{
		//Gets random direction
		int dirX = (rand() % (mParticleDirXHigh - mParticleDirXLow + 1)) + mParticleDirXLow;
		int dirY = (rand() % (mParticleDirYHigh - mParticleDirYLow + 1)) + mParticleDirYLow;
		float distence = mMaxParticleDisFromPlayer * (((rand() % (mParticleDisHigh + 1)) + mParticleDisLow) / (float)mParticleDisHigh);
		float speedOffset = mMaxParticleSpeedOffset * (((rand() % (mParticleDisHigh + 1)) + mParticleDisLow) / (float)mParticleDisHigh);

		Vector2D direction(dirX, dirY);
		direction.normalize();

		const Sprite* currSprite = getCurrAnimation()->getCurrentSprite();
		Vector2D newPos = (mLocation + Vector2D(currSprite->getWidth() / 2.0f, currSprite->getHeight() / 2.0f) - particlePool->getParticleSpriteDimensions() / 2.0f) + direction * (float)distence;

		//Checks to make sure orb moves in a direction
		if (dirX == 0 && dirY == 0)
		{
			dirX = 1;
		}

		particlePool->createParticle(newPos, direction, mParticleSpeed + speedOffset, mParticleLifeTime);

	}
}

string Player::getSaveData()
{
	string data = "";
	data += "Player_Health: " + to_string(mHealth);
	data += "\nPlayer_Num_Of_Bullets: " + to_string(mNumberOfBullets);
	data += "\nPlayer_Can_Move_X: " + to_string(mCanMoveOnX);
	data += "\nPlayer_Can_Move_Y: " + to_string(mCanMoveOnY);
	data += "\nPlayer_Motion_Locked: " + to_string(mPlayerMotionLocked);
	data += "\nPlayer_Can_Dash: " + to_string(mCanDash);
	data += "\nPlayer_Can_Shoot: " + to_string(mCanShoot);
	data += "\nPlayer_Skip_Next_Turn: " + to_string(mSkipNextTurn);

	data += Unit::getSaveData();
	data += "\n\n";

	return data;
}

void Player::loadData(string& fileData)
{
	string splitter;
	int stringPos;
	while (fileData.length() > 0)
	{
		splitter = '\n';
		stringPos = fileData.find(splitter);
		string subStrData = fileData.substr(0, stringPos);
		
		if (stringPos != string::npos)
		{
			fileData.erase(0, stringPos + splitter.length());
		}
		else
		{
			fileData.erase(0);
		}

		if (subStrData.find("Player_Health: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Player_Health: ", subStrData);
			mHealth = stoi(data);
		}
		else if (subStrData.find("Position: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Position: ", subStrData);

			splitter = " ";
			stringPos = subStrData.find(splitter);
			string playerXPos = subStrData.substr(0, stringPos);
			subStrData.erase(0, stringPos + splitter.length());

			mLocation.setX((float)stoi(playerXPos));
			mLocation.setY((float)stoi(subStrData));

		}
		else if (subStrData.find("Velocity: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Velocity: ", subStrData);

			splitter = " ";
			stringPos = subStrData.find(splitter);
			string playerXVel = subStrData.substr(0, stringPos);
			subStrData.erase(0, stringPos + splitter.length());

			mVelocity.setX((float)stoi(playerXVel));
			mVelocity.setY((float)stoi(subStrData));

		}
		else if (subStrData.find("Player_Num_Of_Bullets: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Player_Num_Of_Bullets: ", subStrData);
			mNumberOfBullets = stoi(data);
		}
		else if (subStrData.find("Player_Can_Move_X: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Player_Can_Move_X: ", subStrData);
			mCanMoveOnX = static_cast<bool>(stoi(data));
		}
		else if (subStrData.find("Player_Can_Dash: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Player_Can_Dash: ", subStrData);
			mCanDash = static_cast<bool>(stoi(data));
		}
		else if (subStrData.find("Player_Can_Move_Y: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Player_Can_Move_Y: ", subStrData);
			mCanMoveOnY = static_cast<bool>(stoi(data));
		}
		else if (subStrData.find("Player_Motion_Locked: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Player_Motion_Locked: ", subStrData);
			mPlayerMotionLocked = static_cast<bool>(stoi(data));
		}
		else if (subStrData.find("Player_Can_Shoot: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Player_Can_Shoot: ", subStrData);
			mCanShoot = static_cast<bool>(stoi(data));
		}
		else if (subStrData.find("Player_Skip_Next_Turn: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Player_Skip_Next_Turn: ", subStrData);
			mSkipNextTurn = static_cast<bool>(stoi(data));
		}
	}
}

void Player::loadPlayerData()
{
	Game* gameInst = Game::getInstance();
	mSpeed = (float)gameInst->getPlayerData("Player Speed");
	mNumberOfBulletsPerTurn = gameInst->getPlayerData("Player Bullets Per Turn");
	mNumParticlesPerTeleportation = gameInst->getPlayerData("Number Of Teleport Particles");
	mParticleSpeed = gameInst->getPlayerData("Teleport Particles Speed");
	mDashDistence = gameInst->getPlayerData("Player Dash Distence");
	mMaxHealth = gameInst->getPlayerData("Player Health");
	mHealth = mMaxHealth;
}
