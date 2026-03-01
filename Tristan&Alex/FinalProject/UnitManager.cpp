#include "UnitManager.h"
#include "GraphicsBuffer.h"
#include "Unit.h"
#include "Game.h"
#include "GameEvent.h"
#include "SubtractPointsEvent.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"
#include "HUD.h"
#include "Font.h"
#include <fstream>
#include "ParticlePool.h"
#include "World.h"
#include "AnimationManager.h"
#include "GraphicsBufferManager.h"

UnitManager::UnitManager()
{

}

UnitManager::~UnitManager()
{
	cleanup();
}

void UnitManager::init()
{
	loadDataFromFile();
}

void UnitManager::cleanup()
{
	clearBullets();

	clearEnemies();

	delete mpWorld;
	mpWorld = nullptr;

	delete mpPlayer;
	mpPlayer = nullptr;
}

void UnitManager::resetUnits()
{
	cleanup();
}

void UnitManager::createPlayer(Player* thePlayer)
{
	if (mpPlayer == nullptr)
	{
		mpPlayer = thePlayer;
	}
	else if (mpPlayer != thePlayer)
	{
		delete mpPlayer;
		mpPlayer = thePlayer;
	}
}

void UnitManager::addEnemy(Enemy* enemyToAdd)
{
	mDeactiveEnemyList.push_back(enemyToAdd);
	addEnemyToActiveList(enemyToAdd);
}

void UnitManager::addBullet(Bullet* bulletToAdd)
{
	mBulletList.push_back(bulletToAdd);
}

void UnitManager::createWorld(vector<Animation> animations, Vector2D location, float speed, float animationSpeed, int startingAnimationIndex)
{
	if (mpWorld != nullptr)
	{
		delete mpWorld;
		mpWorld = nullptr;
	}

	vector<SDL_Rect> worldCollisionRects = {};
	worldCollisionRects.push_back({0, 0, 280, 650});
	worldCollisionRects.push_back({209, 0, 250, 580});
	worldCollisionRects.push_back({459, 0, 100, 620});
	worldCollisionRects.push_back({559, 0, 500, 550});
	worldCollisionRects.push_back({1280, 0, 200, 200});
	worldCollisionRects.push_back({1490, 0, 920, 570});
	worldCollisionRects.push_back({775, 865, 20, 190});
	worldCollisionRects.push_back({755, 875, 20, 180});
	worldCollisionRects.push_back({700, 900, 20, 140});
	worldCollisionRects.push_back({840, 900, 20, 140});
	worldCollisionRects.push_back({750, 1100, 180, 90});
	worldCollisionRects.push_back({990, 1030, 180, 50});
	worldCollisionRects.push_back({1200, 1050, 100, 20});
	worldCollisionRects.push_back({1300, 880, 730, 560});
	worldCollisionRects.push_back({2030, 920, 355, 525});

	mpWorld = new World(worldCollisionRects, animations, location, speed, animationSpeed, startingAnimationIndex);

	if (mpPlayer != nullptr)
	{
		handleWorldPlayerMovement();
	}
}

void UnitManager::deleteEnemy(Enemy* enemyToDelete)
{
	removeEnemyFromActiveList(enemyToDelete);

	for (unsigned int i = 0; i < mDeactiveEnemyList.size(); i++) 
	{
		if (mDeactiveEnemyList[i] == enemyToDelete) 
		{
			delete mDeactiveEnemyList[i];
			mDeactiveEnemyList[i] = nullptr;
			mDeactiveEnemyList.erase(mDeactiveEnemyList.begin() + i);
			break;
		}
	}
}

void UnitManager::removeEnemyFromActiveList(Enemy* enemyToDelete)
{
	auto iterator = find(mActiveEnemyList.begin(), mActiveEnemyList.end(), enemyToDelete);

	if (iterator != mActiveEnemyList.end())
	{
		mActiveEnemyList.erase(iterator);
	}
}

void UnitManager::addEnemyToActiveList(Enemy* enemyToDelete)
{
	mActiveEnemyList.push_back(enemyToDelete);
}

bool UnitManager::deleteBullet(Bullet* bulletToDelete)
{
	auto iterator = find(mBulletList.begin(), mBulletList.end(), bulletToDelete);

	//This is done here to delete a bullet even if it is not in the bullet list
	delete bulletToDelete;
	bulletToDelete = nullptr;

	if (iterator != mBulletList.end())
	{
		mBulletList.erase(iterator);
		return true;
	}

	return false;
}

void UnitManager::setAllUnitsAnimationSpeed(float speed)
{
	for (Enemy* enemy : mActiveEnemyList)
	{
		enemy->setUnitAnimationSpeed(speed);
	}

	for (Bullet* bullet : mBulletList)
	{
		bullet->setUnitAnimationSpeed(speed);
	}

	mpPlayer->setUnitAnimationSpeed(speed);
}

void UnitManager::toggleAllUnitsAnimationsPaused()
{
	//Toggles all units paused value and sets all units to new value
	mAllUnitsAnimationsPaused = !mAllUnitsAnimationsPaused;

	for (Enemy* enemy : mActiveEnemyList)
	{
		enemy->setAllAnimationsPaused(mAllUnitsAnimationsPaused);
	}

	for (Bullet* bullet : mBulletList)
	{
		bullet->setAllAnimationsPaused(mAllUnitsAnimationsPaused);
	}

	mpPlayer->setAllAnimationsPaused(mAllUnitsAnimationsPaused);
}

void UnitManager::setAllUnitsAnimationsPaused(bool paused)
{
	//Toggles all units paused value and sets all units to new value
	mAllUnitsAnimationsPaused = paused;

	for (Enemy* enemy : mActiveEnemyList)
	{
		enemy->setAllAnimationsPaused(mAllUnitsAnimationsPaused);
	}

	for (Bullet* bullet : mBulletList)
	{
		bullet->setAllAnimationsPaused(mAllUnitsAnimationsPaused);
	}

	mpPlayer->setAllAnimationsPaused(mAllUnitsAnimationsPaused);
}

void UnitManager::setStartingPlayerHealth()
{
	mStartingBattlePlayerHealth = mpPlayer->getHealth();
}

void UnitManager::clearBullets()
{
	for (unsigned int i = 0; i < mBulletList.size(); i++)
	{
		if (deleteBullet(mBulletList[i]))
		{
			i--;
		}
	}

	mBulletList.clear();
}

void UnitManager::clearEnemies()
{
	for (unsigned int i = 0; i < mDeactiveEnemyList.size(); i++)
	{
		deleteEnemy(mDeactiveEnemyList[i]);
		i--;
	}

	mActiveEnemyList.clear();
	mDeactiveEnemyList.clear();
}

bool UnitManager::getAllEnemiesEmpty()
{
	for (Enemy* enemy : mActiveEnemyList)
	{
		if (!enemy->getIsEmpty())
		{
			return false;
		}
	}

	return true;
}

void UnitManager::decideCurrEnemyActions()
{
	for (Enemy* enemy : mActiveEnemyList)
	{
		enemy->decideAction();
	}

	EventSystem::getInstance()->fireEvent((EventType)GameEvent::ENTER_SHOOTING_MODE);
}

vector<Vector2D> UnitManager::getEnemyBattleLocations()
{
	Vector2D screenDimensions = Game::getDisplayDimensions();
	vector<Vector2D> vectorOfEnemyLocations;
	int numOfEnemies = mActiveEnemyList.size();
	int currEnemyIndex = 0;
	const Sprite* currSprite;

	switch (numOfEnemies)
	{
	case 1:
		currSprite = mActiveEnemyList[currEnemyIndex]->getCurrAnimation()->getCurrentSprite();
		vectorOfEnemyLocations.push_back(Vector2D(screenDimensions.getX() / 1.25f - currSprite->getWidth() / 2, screenDimensions.getY() / 2 - currSprite->getHeight() / 2));
		break;
	case 2:
		currSprite = mActiveEnemyList[currEnemyIndex]->getCurrAnimation()->getCurrentSprite();
		vectorOfEnemyLocations.push_back(Vector2D(screenDimensions.getX() / 1.25f - currSprite->getWidth() / 2, screenDimensions.getY() / 3.5f - currSprite->getHeight() / 2));

		currEnemyIndex++;
		currSprite = mActiveEnemyList[currEnemyIndex]->getCurrAnimation()->getCurrentSprite();
		vectorOfEnemyLocations.push_back(Vector2D(screenDimensions.getX() / 1.25f - currSprite->getWidth() / 2, screenDimensions.getY() - screenDimensions.getY() / 3.5f - currSprite->getHeight() / 2));
		break;
	case 3:
		currSprite = mActiveEnemyList[currEnemyIndex]->getCurrAnimation()->getCurrentSprite();
		vectorOfEnemyLocations.push_back(Vector2D(screenDimensions.getX() / 1.5f - currSprite->getWidth() / 2, screenDimensions.getY() / 2 - currSprite->getHeight() / 2));

		currEnemyIndex++;
		currSprite = mActiveEnemyList[currEnemyIndex]->getCurrAnimation()->getCurrentSprite();
		vectorOfEnemyLocations.push_back(Vector2D(screenDimensions.getX() / 1.125f - currSprite->getWidth() / 1.5f, screenDimensions.getY() / 3.5f - currSprite->getHeight() / 2));

		currEnemyIndex++;
		currSprite = mActiveEnemyList[currEnemyIndex]->getCurrAnimation()->getCurrentSprite();
		vectorOfEnemyLocations.push_back(Vector2D(screenDimensions.getX() / 1.125f - currSprite->getWidth() / 1.5f, screenDimensions.getY() - screenDimensions.getY() / 3.5f - currSprite->getHeight() / 2));
		break;
	}

	return vectorOfEnemyLocations;
}

void UnitManager::haveEnemiesAttack()
{
	for (Enemy* enemy : mActiveEnemyList)
	{
		const Sprite* playerSprite = mpPlayer->getCurrAnimation()->getCurrentSprite();
		Vector2D playerPos(mpPlayer->getPosition() + Vector2D(playerSprite->getWidth() / 2.0f, playerSprite->getHeight() / 2.0f));
		enemy->attack(playerPos, mActiveEnemyList, Game::getDisplayDimensions());
	}
}

bool UnitManager::checkForBulletCollisionsAndDestroyBullet(Bullet* bullet)
{
	if (!bullet->getShotByPlayer())
	{
		if (SDL_HasIntersection(&bullet->getRect(), &mpPlayer->getRect()))
		{
			mpPlayer->addToPlayerHealth(-mEnemyBulletDamage);

			if (mpPlayer->getHealth() <= 0)
			{
				EventSystem::getInstance()->fireEvent((EventType)GameEvent::PLAYER_DIES);
			}

			if (deleteBullet(bullet))
			{
				return true;
			}
		}
	}
	else 
	{
		for (Enemy* enemy : mActiveEnemyList)
		{
			if (SDL_HasIntersection(&bullet->getRect(), &enemy->getRect()))
			{
				enemy->addToHealth(-mPlayerBulletDamage);

				if (enemy->getHealth() <= 0)
				{
					removeEnemyFromActiveList(enemy);
				}

				if (deleteBullet(bullet))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool UnitManager::relocateBattleUnits(float speedToRelocate)
{
	bool allHaveBeenRelocated = true;
	Vector2D screenDimentions = Game::getDisplayDimensions();
	const Sprite* currSprite = mpPlayer->getCurrAnimation()->getCurrentSprite();
	Vector2D locationToMoveUnit(screenDimentions.getX() / 6 - currSprite->getWidth() / 2, screenDimentions.getY() / 2 - currSprite->getHeight() / 2);

	if (!mpPlayer->moveTo(locationToMoveUnit, speedToRelocate))
	{
		allHaveBeenRelocated = false;
	}

	vector<Vector2D> enemyLocations = getEnemyBattleLocations();
	for (unsigned int i = 0; i < mActiveEnemyList.size(); i++)
	{
		if (!mActiveEnemyList[i]->moveTo(enemyLocations[i], speedToRelocate))
		{
			allHaveBeenRelocated = false;
		}
	}

	return allHaveBeenRelocated;
}

void UnitManager::setBattleUnitLocations()
{
	Vector2D screenDimentions = Game::getDisplayDimensions();
	const Sprite* currSprite = mpPlayer->getCurrAnimation()->getCurrentSprite();
	Vector2D locationToMoveUnit(screenDimentions.getX() / 6 - currSprite->getWidth() / 2, screenDimentions.getY() / 2 - currSprite->getHeight() / 2);

	mpPlayer->setPosition(locationToMoveUnit);

	for (Enemy* enemy : mActiveEnemyList)
	{
		currSprite = enemy->getCurrAnimation()->getCurrentSprite();
		locationToMoveUnit = Vector2D(screenDimentions.getX() / 1.25f - currSprite->getWidth() / 2, screenDimentions.getY() / 2 - currSprite->getHeight() / 2);
		enemy->setPosition(locationToMoveUnit);
	}
}

void UnitManager::resetBattle()
{
	clearBullets();
	mpPlayer->resetPlayer(mStartingBattlePlayerHealth);

	for (Enemy* enemy : mDeactiveEnemyList)
	{
		enemy->resetEnemy();
		auto iterator = find(mActiveEnemyList.begin(), mActiveEnemyList.end(), enemy);
		if (iterator == mActiveEnemyList.end())
		{
			addEnemyToActiveList(enemy);
		}
	}

	Game::getInstance()->getPlayerParticlePool()->resetParticles();
}

void UnitManager::handleWorldPlayerMovement()
{
	mpWorld->setWorldSpeed(-mpPlayer->getSpeed());
	Vector2D windowDimensions = Game::getInstance()->getDisplayDimensions();
	Vector2D playerPos = mpPlayer->getPosition();
	Vector2D worldPos = mpWorld->getPosition();
	const Sprite* playerSprite = mpPlayer->getCurrAnimation()->getCurrentSprite();
	const Sprite* worldSprite = mpWorld->getCurrAnimation()->getCurrentSprite();
	float minDistenceFromLeftAndRightWalls = windowDimensions.getX() / 2.0f;
	float minDistenceFromTopAndBottomWalls = windowDimensions.getY() / 2.0f;

	float playerDIstenceFromLeftWall = (playerPos + Vector2D(playerSprite->getWidth() / 2.0f, 0.0f) - Vector2D(worldPos.getX(), playerPos.getY())).getLength();
	float playerDIstenceFromRightWall = (playerPos + Vector2D(playerSprite->getWidth() / 2.0f, 0.0f) - Vector2D(worldPos.getX() + worldSprite->getWidth(), playerPos.getY())).getLength();
	float playerDIstenceFromTopWall = (playerPos + Vector2D(0.0f, playerSprite->getHeight() / 2.0f) - Vector2D(playerPos.getX(), worldPos.getY())).getLength();
	float playerDIstenceFromBottomWall = (playerPos + Vector2D(0.0f, playerSprite->getHeight() / 2.0f) - Vector2D(playerPos.getX(), worldPos.getY() + worldSprite->getHeight())).getLength();

	if (mpPlayer->getPlayerCanMove())
	{
		if (playerDIstenceFromLeftWall < minDistenceFromLeftAndRightWalls || playerDIstenceFromRightWall < minDistenceFromLeftAndRightWalls)
		{
			mpPlayer->setCanMoveOnXAxis(true);
			mpWorld->setCanMoveOnXAxis(false);

			if (playerDIstenceFromLeftWall < playerDIstenceFromRightWall)
			{
				mpWorld->setPosition(Vector2D(0.0f, worldPos.getY()));
			}
			else
			{
				mpWorld->setPosition(Vector2D(windowDimensions.getX() - worldSprite->getWidth(), worldPos.getY()));
			}
		}
		else
		{
			mpPlayer->setCanMoveOnXAxis(false);
			mpWorld->setCanMoveOnXAxis(true);
			mpPlayer->setPosition(Vector2D(minDistenceFromLeftAndRightWalls - playerSprite->getWidth() / 2.0f, playerPos.getY()));
		}

		playerPos = mpPlayer->getPosition();
		worldPos = mpWorld->getPosition();
		if (playerDIstenceFromTopWall < minDistenceFromTopAndBottomWalls || playerDIstenceFromBottomWall < minDistenceFromTopAndBottomWalls)
		{
			mpPlayer->setCanMoveOnYAxis(true);
			mpWorld->setCanMoveOnYAxis(false);

			if (playerDIstenceFromTopWall < playerDIstenceFromBottomWall)
			{
				mpWorld->setPosition(Vector2D(worldPos.getX(), 0.0f));
			}
			else
			{
				mpWorld->setPosition(Vector2D(worldPos.getX(), windowDimensions.getY() - worldSprite->getHeight()));
			}
		}
		else
		{
			mpPlayer->setCanMoveOnYAxis(false);
			mpWorld->setCanMoveOnYAxis(true);
			mpPlayer->setPosition(Vector2D(playerPos.getX(), minDistenceFromTopAndBottomWalls - playerSprite->getHeight() / 2.0f));
		}
	}
}

void UnitManager::loadDataFromFile()
{
	ifstream file;
	file.open("..\\..\\Tristan&Alex\\assets\\gameValues.txt");
	string data;

	if (file.is_open())
	{
		//Gets data from file from any location in the file
		while (!file.eof())
		{
			getline(file, data, ' ');
			auto iterator = data.find("Player_Bullet_Damage");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mPlayerBulletDamage = stoi(data);
			}

			iterator = data.find("Enemy_Bullet_Damage");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyBulletDamage = stoi(data);
			}
		}

		file.close();
	}
	else
	{
		cout << "Error opening gameValues file" << endl;
	}
}

void UnitManager::loadUnitDataFromFile(string& data)
{
	string splitter;
	int stringPos;
	while (data.length() > 0)
	{
		splitter = '\n';
		stringPos = data.find(splitter);
		string subStrData = data.substr(0, stringPos);
		data.erase(0, stringPos + splitter.length());

		if (subStrData.find("Player_Data:") != string::npos)
		{
			if (mpPlayer == nullptr && data != "\n\n") //This is so that if the player still exists in the current game, it doesn't destroy and remake it
			{
				Game::getInstance()->createPlayer();
			}

			splitter = "\n\n";
			int stringPos = data.find(splitter);
			string playerData = data.substr(0, stringPos);
			mpPlayer->loadData(playerData);
			data.erase(0, stringPos + splitter.length());
		}
		else if (subStrData.find("Enemy_Data:") != string::npos)
		{
			splitter = "Enemy:\n";
			stringPos = data.find(splitter);
			while (stringPos != string::npos)
			{
				splitter = "\n\n";
				stringPos = data.find(splitter);
				string enemyData = data.substr(0, stringPos);

				Enemy* enemy = new Enemy();
				if (enemy->loadDataAndGetIsActive(enemyData))
				{
					addEnemy(enemy);
				}
				else
				{
					mDeactiveEnemyList.push_back(enemy);
				}

				data.erase(0, stringPos + splitter.length());
				splitter = "Enemy:\n";
				stringPos = data.find(splitter);
			}
		}
		else if (subStrData.find("World_Data:") != string::npos)
		{
			if (mpWorld == nullptr && data != "\n\n")
			{
				Game::getInstance()->createWorld();
			}

			splitter = "\n\n\n";
			int stringPos = data.find(splitter);
			string worldData = data.substr(0, stringPos);
			mpWorld->loadData(worldData);
			data.erase(0, stringPos + splitter.length());
		}
		else if (subStrData.find("Bullet_Data:") != string::npos)
		{
			splitter = "Bullet:\n";
			stringPos = data.find(splitter);
			while (stringPos != string::npos)
			{
				splitter = "\n\n";
				stringPos = data.find(splitter);
				string bulletData = data.substr(0, stringPos);

				Bullet* bullet = new Bullet();
				bullet->loadData(bulletData);
				mBulletList.push_back(bullet);

				data.erase(0, stringPos + splitter.length());
				splitter = "Bullet:\n";
				stringPos = data.find(splitter);
			}
		}
	}
}

string UnitManager::getUnitSaveData()
{
	string data = "";

	data += "World_Data:\n";

	if (mpPlayer != nullptr)
	{
		data += mpWorld->getSaveData();
	}

	data += "Player_Data:\n";

	if (mpPlayer != nullptr)
	{
		data += mpPlayer->getSaveData();
	}

	data += "Enemy_Data:\n";

	for (Enemy* enemy : mDeactiveEnemyList)
	{
		data += "\nEnemy:\n";
		data += enemy->getSaveData();
		data += "\nIs_Active: " + to_string((find(mActiveEnemyList.begin(), mActiveEnemyList.end(), enemy) != mActiveEnemyList.end()));
		data += "\n";
	}

	data += "\nBullet_Data:\n";

	for (Bullet* bullet : mBulletList)
	{
		data += "\nBullet:";
		data += bullet->getSaveData() + "\n";
	}

	data += "\n\n";

	return data;
}

void UnitManager::loadWorldNPCs(float animationSpeed, vector<vector<string>> npcDiologue, vector<vector<string>> npcEnemyDiologue)
{
	if (mpWorld)
	{
		//This is to make sure that the number of messages matches the number of enemies
		SDL_assert(npcEnemyDiologue.size() == 3);
		SDL_assert(npcDiologue.size() == 4);

		vector<tuple<Unit*, vector<string>>> worldEnemyUnits;
		vector<Animation> unitAnimations = { Animation(Game::getInstance()->getAnimationManager()->getAnimationData("enemyAnimation"), true) };
		worldEnemyUnits.push_back(tuple<Unit*, vector<string>>(new Unit(unitAnimations, animationSpeed, 0, Vector2D(650, 700)), npcEnemyDiologue[0]));
		worldEnemyUnits.push_back(tuple<Unit*, vector<string>>(new Unit(unitAnimations, animationSpeed, 0, Vector2D(1448, 670)), npcEnemyDiologue[1]));
		worldEnemyUnits.push_back(tuple<Unit*, vector<string>>(new Unit(unitAnimations, animationSpeed, 0, Vector2D(2180, 540)), npcEnemyDiologue[2]));

		vector<tuple<Unit*, vector<string>>> worldNPCUnits;
		unitAnimations = { Animation(Game::getInstance()->getAnimationManager()->getAnimationData("westerner1Animation"), true) };
		worldNPCUnits.push_back(tuple<Unit*, vector<string>>(new Unit(unitAnimations, animationSpeed, 0, Vector2D(1200, 1050)), npcDiologue[0]));
		worldNPCUnits.push_back(tuple<Unit*, vector<string>>(new Unit(unitAnimations, animationSpeed, 0, Vector2D(810, 1075)), npcDiologue[1]));
		unitAnimations = { Animation(Game::getInstance()->getAnimationManager()->getAnimationData("westerner2Animation"), true) };
		worldNPCUnits.push_back(tuple<Unit*, vector<string>>(new Unit(unitAnimations, animationSpeed, 0, Vector2D(220, 630)), npcDiologue[2]));
		worldNPCUnits.push_back(tuple<Unit*, vector<string>>(new Unit(unitAnimations, animationSpeed, 0, Vector2D(1350, 185)), npcDiologue[3]));
		mpWorld->addWorldUnits(worldEnemyUnits, worldNPCUnits);

		vector<SDL_Rect> worldBaracadeRects = {};
		worldBaracadeRects.push_back({ 750, 630, 100, 200 });
		worldBaracadeRects.push_back({ 1500, 630, 100, 200 });
		mpWorld->setWorldBaracades(worldBaracadeRects);
	}
}

void UnitManager::update(float deltaTime)
{
	if (mpWorld != nullptr && Game::getInstance()->getCurrScreen() == Game::OVERWORLD_SCREEN)
	{
		mpWorld->update(deltaTime, mpPlayer);
	}

	for (unsigned int i = 0; i < mBulletList.size(); i++)
	{
		mBulletList[i]->update(deltaTime);

		//Gets values necessary to calculate distence to screen edge
		Vector2D displayDimensions = Game::getDisplayDimensions();
		Vector2D unitPos = mBulletList[i]->getPosition();
		Animation* currUnitAnimation = mBulletList[i]->getCurrAnimation();
		const Sprite* currSprite = currUnitAnimation->getCurrentSprite();

		//Checks if unit is offscreen
		bool hasCrossedTop = unitPos.getY() + currSprite->getHeight() < 0;
		bool hasCrossedBottom = unitPos.getY() > displayDimensions.getY();
		bool hasCrossedRight = unitPos.getX() > displayDimensions.getX();
		bool hasCrossedLeft = unitPos.getX() + currSprite->getWidth() < 0;

		if (hasCrossedTop || hasCrossedBottom || hasCrossedRight || hasCrossedLeft)
		{
			deleteBullet(mBulletList[i]);
			i--;
		}
		else
		{
			if (checkForBulletCollisionsAndDestroyBullet(mBulletList[i]))
			{
				i--;
			}
		}
	}

	for (Enemy* enemy : mActiveEnemyList)
	{
		enemy->update(deltaTime);
	}

	if (mpPlayer != nullptr)
	{
		mpPlayer->update(deltaTime);
	}

	if (Game::getInstance()->getCurrScreen() == Game::OVERWORLD_SCREEN)
	{
		handleWorldPlayerMovement();
	}
}

void UnitManager::drawAll(GraphicsBuffer* destBuffer, GraphicsBuffer* hud)
{
	drawWorld(destBuffer, hud);

	drawTempUnits(destBuffer);
}

void UnitManager::drawWorld(GraphicsBuffer* destBuffer, GraphicsBuffer* hud)
{
	if (mpWorld != nullptr)
	{
		mpWorld->draw(destBuffer, hud);
	}
}

void UnitManager::drawTempUnits(GraphicsBuffer* destBuffer)
{
	for (Enemy* enemy : mActiveEnemyList)
	{
		enemy->draw(destBuffer);
	}

	for (Bullet* bullet : mBulletList)
	{
		bullet->draw(destBuffer);
	}

	if (mpPlayer != nullptr)
	{
		mpPlayer->draw(destBuffer);
	}
}

void UnitManager::displayUnitInfo(HUD* hud, Font* font)
{
	Sprite& textBGSprite = Game::getInstance()->getTextBGSprite();
	hud->draw(Vector2D(mpPlayer->getPosition() + Vector2D(mpPlayer->getCurrAnimation()->getCurrentSprite()->getWidth() / 2.0f, -10.0f) - Vector2D(textBGSprite.getWidth() * 0.4f / 2.0f, 25.0f)), textBGSprite, Vector2D(0.4f, 0.8f));
	hud->writeHUDText(getPlayer()->getPosition() + Vector2D(getPlayer()->getCurrAnimation()->getCurrentSprite()->getWidth() / 2.0f, -15.0f), *font, Color(1.0f, 1.0f, 1.0f), Game::getInstance()->getCurrHealthDisplayText() + ": " + to_string(mpPlayer->getHealth()), font->CENTER);

	for each (Enemy * enemy in mActiveEnemyList)
	{
		hud->draw(Vector2D(enemy->getPosition() + Vector2D(enemy->getCurrAnimation()->getCurrentSprite()->getWidth() / 2.0f, -10.0f) - Vector2D(textBGSprite.getWidth() * 0.4f / 2.0f, 25.0f)), textBGSprite, Vector2D(0.4f, 0.8f));
		hud->writeHUDText(enemy->getPosition() + Vector2D(enemy->getCurrAnimation()->getCurrentSprite()->getWidth() / 2.0f, -15.0f), *font, Color(1.0f, 1.0f, 1.0f), Game::getInstance()->getCurrHealthDisplayText() + ": " + to_string(enemy->getHealth()), font->CENTER);
	}

	hud->draw(Vector2D(-40.0f, 0.0f), textBGSprite);
	hud->writeHUDText(Vector2D(5, 5), *font, Color(1.0f, 1.0f, 1.0f), Game::getInstance()->getCurrBulletsRemainingText() + ": " + to_string(getPlayer()->getNumBullets()));
}
