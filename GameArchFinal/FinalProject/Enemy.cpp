#include "Enemy.h"
#include "Animation.h"
#include "GameEvent.h"
#include "FireGunEvent.h"
#include "EventSystem.h"
#include "ClickButtonEvent.h"
#include <fstream>
#include "Game.h"
#include "AnimationManager.h"
#include "PlaySoundEvent.h"

Enemy::Enemy(vector<Animation> animations, float animationSpeed, int startingAnimationIndex, Vector2D location) :
	Unit(animations, animationSpeed, startingAnimationIndex, location)
{
	init();
}

Enemy::Enemy() : Unit()
{
	init();
}

Enemy::~Enemy()
{
	cleanup();
}

void Enemy::init()
{
	srand((unsigned)time(NULL));

	EventSystem* theEventSystem = EventSystem::getInstance();
	theEventSystem->addListener((EventType)GameEvent::BEGIN_ENEMY_TURN, this);

	loadEnemyData();
}

void Enemy::cleanup()
{
}

void Enemy::loadEnemyData()
{
	Game* gameInst = Game::getInstance();
	mSpeed = (float)gameInst->getEnemyData("Enemy Speed");
	mDistenceFromOtherEnemies = gameInst->getEnemyData("Enemy Distence From Other Enemies");
	mDistenceFromPlayer = gameInst->getEnemyData("Enemy Distence From Player");
	mShootChancePerFrame = gameInst->getEnemyData("Enemy Shoot Chance Per Frame");
	mNumberOfBulletsPerTurn = gameInst->getEnemyData("Enemy Bullets Per Turn");

	Game::Difficulty difficulty = Game::getInstance()->getCurrentDifficulty();
	switch (difficulty)
	{
	case Game::EASY:
		mMaxHealth = gameInst->getEnemyData("Enemy Health Easy");
		break;
	case Game::MEDIUM:
		mMaxHealth = gameInst->getEnemyData("Enemy Health Medium");
		break;
	case Game::HARD:
		mMaxHealth = gameInst->getEnemyData("Enemy Health Hard");
		break;
	}
	mHealth = mMaxHealth;

}

void Enemy::handleEvent(const Event& theEvent)
{
	EventType theType = theEvent.getType();

	if (theType == GameEvent::BEGIN_ENEMY_TURN)
	{
		mNumberOfBullets += mNumberOfBulletsPerTurn;
	}
}

void Enemy::decideAction()
{
	//Add probobly some random choice between reloading and attacking 
}

void Enemy::attack(Vector2D playerPos, vector<Enemy*> otherEnemies, Vector2D gameWindowDimension)
{
	Vector2D playerToEnemyVec = playerPos - mLocation;
	Vector2D moveFromPlayerVec;
	if (abs(playerToEnemyVec.getLength() - mDistenceFromPlayer) > mSpeed)
	{
		if (playerToEnemyVec.getLength() < mDistenceFromPlayer)
		{
			playerToEnemyVec.normalize();
			moveFromPlayerVec = playerToEnemyVec * -mSpeed;
		}
		else if ((int)playerToEnemyVec.getLength() > mDistenceFromPlayer)
		{
			playerToEnemyVec.normalize();
			moveFromPlayerVec = playerToEnemyVec * mSpeed;
		}
	}
	else if (playerToEnemyVec.getLength() - mDistenceFromPlayer != 0)
	{
		Vector2D copyVec = playerToEnemyVec;
		copyVec.normalize();
		moveFromPlayerVec = ((copyVec * (float)mDistenceFromPlayer) - playerToEnemyVec) * -1;
	}

	for (Enemy* enemy : otherEnemies)
	{
		if (enemy != this)
		{
			Vector2D enemyToOtherEnemyVec = enemy->getPosition() - mLocation;
			if (abs(enemyToOtherEnemyVec.getLength() - mDistenceFromOtherEnemies) > mSpeed)
			{
				if (enemyToOtherEnemyVec.getLength() < mDistenceFromOtherEnemies)
				{
					Vector2D copyVec = enemyToOtherEnemyVec;
					copyVec.normalize();
					moveFromPlayerVec += (((copyVec * (float)mDistenceFromOtherEnemies) - enemyToOtherEnemyVec) - copyVec * mSpeed) * -1; //Bro I don't even know what I did here, I just messed around and this seemd right lol
				}
			}
		}
	}

	mLocation += moveFromPlayerVec;

	handleStayOnScreen(gameWindowDimension);

	//Gets chance to shoot
	int randomSpawnNumber = (rand() % mShootChancePerFrame);
	if (randomSpawnNumber == 0)
	{
		fire(playerPos);
	}
}

void Enemy::fire(Vector2D playerPos)
{
	if (mNumberOfBullets > 0)
	{
		EventSystem::getInstance()->fireEvent(PlaySoundEvent("gunshotSound"));
		const Sprite* currSprite = getCurrAnimation()->getCurrentSprite();
		Vector2D shotFromPos = mLocation + Vector2D(0.0f, currSprite->getHeight() / 2.0f);
		EventSystem::getInstance()->fireEvent(FireGunEvent(playerPos, shotFromPos, false));
		mNumberOfBullets--;
	}
}

void Enemy::addToHealth(int amountToAdd)
{
	mHealth += amountToAdd;
}

void Enemy::resetEnemy()
{
	mNumberOfBullets = 0;
	setHealth(mMaxHealth);
}

void Enemy::handleStayOnScreen(Vector2D gameWindowDimension)
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

string Enemy::getSaveData()
{
	string data = "";
	data += "Enemy_Health: " + to_string(mHealth);
	data += "\nEnemy_Num_Of_Bullets: " + to_string(mNumberOfBullets);
	data += "\nNum_Bullets_Per_Turn: " + to_string(mNumberOfBulletsPerTurn);
	data += "\nShoot_Chance_Per_Frame: " + to_string(mShootChancePerFrame);
	data += "\nDistence_From_Player: " + to_string(mDistenceFromPlayer);
	data += "\nDistence_From_Other_Enemies: " + to_string(mDistenceFromOtherEnemies);
	data += "\nEnemy_Speed: " + to_string(mSpeed);

	data += Unit::getSaveData();

	return data;
}

bool Enemy::loadDataAndGetIsActive(string& fileData)
{
	bool isActive;
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

		if (subStrData.find("Enemy_Health: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Enemy_Health: ", subStrData);
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
			string playerXPos = subStrData.substr(0, stringPos);
			subStrData.erase(0, stringPos + splitter.length());

			mVelocity.setX((float)stoi(playerXPos));
			mVelocity.setY((float)stoi(subStrData));

		}
		else if (subStrData.find("Enemy_Speed: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Enemy_Speed: ", subStrData);
			mSpeed = (float)stoi(data);
		}
		else if (subStrData.find("Animations:") != string::npos)
		{
			splitter = "\n";
			stringPos = fileData.find(splitter);
			subStrData = fileData.substr(0, stringPos);
			while (subStrData.find("Animation: ") != string::npos)
			{
				string animationKey = Game::getInstance()->extractDataFromDataString("Animation: ", fileData);
				bool isLooping = static_cast<bool>(stoi(Game::getInstance()->extractDataFromDataString("Is_Looping: ", fileData)));
				mAnimations.push_back(Animation(Game::getInstance()->getAnimationManager()->getAnimationData(animationKey), isLooping));

				stringPos = fileData.find(splitter);
				subStrData = fileData.substr(0, stringPos);
			}

			string currAnimation = Game::getInstance()->extractDataFromDataString("Curr_Animation_Index: ", fileData);

			stringPos = fileData.find(splitter);
			subStrData = fileData.substr(0, stringPos);

			string animationSpeed = Game::getInstance()->extractDataFromDataString("Animation_Speed: ", subStrData);
			setUnitAnimation(stoi(currAnimation), static_cast<float>(stoi(animationSpeed)));

		}
		else if (subStrData.find("Num_Bullets_Per_Turn: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Num_Bullets_Per_Turn: ", subStrData);
			mNumberOfBulletsPerTurn = stoi(data);
		}
		else if (subStrData.find("Shoot_Chance_Per_Frame: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Shoot_Chance_Per_Frame: ", subStrData);
			mShootChancePerFrame = stoi(data);
		}
		else if (subStrData.find("Distence_From_Player: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Distence_From_Player: ", subStrData);
			mDistenceFromPlayer = stoi(data);
		}
		else if (subStrData.find("Distence_From_Other_Enemies: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Distence_From_Other_Enemies: ", subStrData);
			mDistenceFromOtherEnemies = stoi(data);
		}
		else if (subStrData.find("Is_Active: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Is_Active: ", subStrData);
			isActive = static_cast<bool>(stoi(data));
		}
		else if (subStrData.find("Enemy_Num_Of_Bullets: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Enemy_Num_Of_Bullets: ", subStrData);
			mNumberOfBullets = stoi(data);
		}
	}

	return isActive;
}
