#include "World.h"
#include "Game.h"
#include "GameEvent.h"
#include "AddToMessageListEvent.h"
#include "GraphicsBuffer.h"

World::World(vector<SDL_Rect> collisionRects, vector<Animation> animations, Vector2D location, float speed, float animationSpeed, int startingAnimationIndex) :
	Unit(animations, location, speed, animationSpeed, startingAnimationIndex), mCollisionRects(collisionRects)
{
	init();
}

World::~World()
{
	cleanup();
}

void World::init()
{
	EventSystem* theEventSystem = EventSystem::getInstance();
	theEventSystem->addListener((EventType)GameEvent::PLAYER_UP, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_RIGHT, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_DOWN, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_LEFT, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_READ_TEXT, this);
	theEventSystem->addListener((EventType)GameEvent::READ_TEXT, this);
	theEventSystem->addListener((EventType)GameEvent::LOCK_PLAYER_MOVEMENT, this);

	mCanMoveOnX = true;
	mCanMoveOnY = true;
}

void World::cleanup()
{
	for (tuple<Unit*, vector<string>> unit : mWorldEnemys)
	{
		delete get<0>(unit);
		get<0>(unit) = nullptr;
	}

	for (tuple<Unit*, vector<string>> unit : mWorldNPCUnits)
	{
		delete get<0>(unit);
		get<0>(unit) = nullptr;
	}

	mWorldEnemys.clear();
	mWorldNPCUnits.clear();
}

void World::addWorldUnits(vector<tuple<Unit*, vector<string>>> worldEnemyUnits, vector<tuple<Unit*, vector<string>>> worldNPCUnits)
{
	for (tuple<Unit*, vector<string>> unit : worldEnemyUnits)
	{
		auto iterator = find(mWorldEnemys.begin(), mWorldEnemys.end(), unit);

		if (iterator == mWorldEnemys.end())
		{
			mWorldEnemys.push_back(unit);
		}
	}

	for (tuple<Unit*, vector<string>> unit : worldNPCUnits)
	{
		auto iterator = find(mWorldNPCUnits.begin(), mWorldNPCUnits.end(), unit);

		if (iterator == mWorldNPCUnits.end())
		{
			mWorldNPCUnits.push_back(unit);
		}
	}

	mTotalEnemies = mWorldEnemys.size();
}

void World::setWorldBaracades(vector<SDL_Rect> baracadeRects)
{
	mBaracadeRects = baracadeRects;
}

void World::removeAndDeleteEnemy(tuple<Unit*, vector<string>> unitToDelete)
{
	auto iterator = find(mWorldEnemys.begin(), mWorldEnemys.end(), mCurrUnitOverlappingPlayer);
	if (iterator != mWorldEnemys.end())
	{
		delete get<0>(unitToDelete);
		get<0>(unitToDelete) = nullptr;
		mWorldEnemys.erase(iterator);
	}
}

void World::removeNextBaracade()
{
	if (mBaracadeRects.size() > 0)
	{
		mBaracadeRects.erase(mBaracadeRects.begin());
	}
}

void World::handleEvent(const Event& theEvent)
{
	EventType theType = theEvent.getType();

	if (!Game::getGameIsPaused() && Game::getInstance()->getCurrScreen() == Game::OVERWORLD_SCREEN)
	{
		if (theType == GameEvent::PLAYER_UP || theType == GameEvent::PLAYER_DOWN || theType == GameEvent::PLAYER_RIGHT || theType == GameEvent::PLAYER_LEFT)
		{
			switch (theType)
			{
			case GameEvent::PLAYER_UP:
				worldMoveUp();
				break;
			case GameEvent::PLAYER_DOWN:
				worldMoveDown();
				break;
			case GameEvent::PLAYER_RIGHT:
				worldMoveRight();
				break;
			case GameEvent::PLAYER_LEFT:
				worldMoveLeft();
				break;
			}
		}
		else if (theType == GameEvent::BEGIN_READ_TEXT)
		{
			if (get<0>(mCurrUnitOverlappingPlayer) != nullptr)
			{
				for (string message : get<1>(mCurrUnitOverlappingPlayer))
				{
					EventSystem::getInstance()->fireEvent(AddToMessageListEvent(message));
				}
				get<1>(mCurrUnitOverlappingPlayer) = {};
				EventSystem::getInstance()->fireEvent((EventType)GameEvent::LOCK_PLAYER_MOVEMENT);
			}
		}
		else if (theType == GameEvent::READ_TEXT)
		{
			if (!Game::getInstance()->getIsMessages())
			{
				EventSystem::getInstance()->fireEvent((EventType)GameEvent::UNLOCK_PLAYER_MOVEMENT);
				mCanMoveOnX = true;
				mCanMoveOnY = true;

				auto iterator = find(mWorldEnemys.begin(), mWorldEnemys.end(), mCurrUnitOverlappingPlayer);
				if (iterator != mWorldEnemys.end())
				{
					removeAndDeleteEnemy(mCurrUnitOverlappingPlayer);
					for (unsigned int i = 0; i < mTotalEnemies - mWorldEnemys.size(); i++)
					{
						Game::getInstance()->createEnemy();
					}
					EventSystem::getInstance()->fireEvent((EventType)GameEvent::BEGIN_BATTLE);
				}
			}
		}
		else if (theType == GameEvent::LOCK_PLAYER_MOVEMENT)
		{
			mCanMoveOnX = false;
			mCanMoveOnY = false;
		}
	}
}

void World::worldMoveUp()
{
	if (mCanMoveOnY)
	{
		mVelocity = Vector2D(0.0f, -mSpeed);
		mLocation += mVelocity;
	}
}

void World::worldMoveRight()
{
	if (mCanMoveOnX)
	{
		mVelocity = Vector2D(mSpeed, 0.0f);
		mLocation += mVelocity;
	}
}

void World::worldMoveDown()
{
	if (mCanMoveOnY)
	{
		mVelocity = Vector2D(0.0f, mSpeed);
		mLocation += mVelocity;
	}
}

void World::worldMoveLeft()
{
	if (mCanMoveOnX)
	{
		mVelocity = Vector2D(-mSpeed, 0.0f);
		mLocation += mVelocity;
	}
}

void World::handleCollision(Unit* unitToHandleCollisionFor)
{
	for (const SDL_Rect& collisionRect : mCollisionRects)
	{
		handleCollisionResponse(collisionRect, unitToHandleCollisionFor);
	}

	for (const SDL_Rect& collisionRect : mBaracadeRects)
	{
		handleCollisionResponse(collisionRect, unitToHandleCollisionFor);
	}
}

void World::handleCollisionResponse(SDL_Rect collisionRect, Unit* unitToHandleCollisionFor)
{
	SDL_Rect worldSpaceRect = { collisionRect.x + (int)mLocation.getX(), collisionRect.y + (int)mLocation.getY(), collisionRect.w, collisionRect.h };
	if (SDL_HasIntersection(&worldSpaceRect, &unitToHandleCollisionFor->getRect()))
	{
		Vector2D unitPosition = unitToHandleCollisionFor->getPosition();
		const Sprite* unitSprite = unitToHandleCollisionFor->getCurrAnimation()->getCurrentSprite();

		float distFromTop = unitPosition.getY() + unitSprite->getHeight() - worldSpaceRect.y;
		float distFromBottom = worldSpaceRect.y + worldSpaceRect.h - unitPosition.getY();
		float distFromRight = unitPosition.getX() + unitSprite->getWidth() - worldSpaceRect.x;
		float distFromLeft = worldSpaceRect.x + worldSpaceRect.w - unitPosition.getX();

		double minVal = min(min(distFromTop, distFromBottom), min(distFromRight, distFromLeft));
		if (minVal == distFromRight)
		{
			if (!mCanMoveOnX)
			{
				unitToHandleCollisionFor->setPosition(Vector2D(unitPosition.getX() - distFromRight, unitPosition.getY()));
			}
			else
			{
				mLocation = Vector2D(mLocation.getX() + distFromRight, mLocation.getY());
			}
		}
		else if (minVal == distFromLeft)
		{
			if (!mCanMoveOnX)
			{
				unitToHandleCollisionFor->setPosition(Vector2D(unitPosition.getX() + distFromLeft, unitPosition.getY()));
			}
			else
			{
				mLocation = Vector2D(mLocation.getX() - distFromLeft, mLocation.getY());
			}
		}
		else if (minVal == distFromBottom)
		{
			if (!mCanMoveOnY)
			{
				unitToHandleCollisionFor->setPosition(Vector2D(unitPosition.getX(), unitPosition.getY() + distFromBottom));
			}
			else
			{
				mLocation = Vector2D(mLocation.getX(), mLocation.getY() - distFromBottom);
			}
		}
		else if (minVal == distFromTop)
		{
			if (!mCanMoveOnY)
			{
				unitToHandleCollisionFor->setPosition(Vector2D(unitPosition.getX(), unitPosition.getY() - distFromTop));
			}
			else
			{
				mLocation = Vector2D(mLocation.getX(), mLocation.getY() + distFromTop);
			}
		}
	}
}

void World::draw(GraphicsBuffer* destBuffer, GraphicsBuffer* hud)
{
	Unit::draw(destBuffer);

	for (tuple<Unit*, vector<string>> unit : mWorldEnemys)
	{
		Vector2D unitDrawLocation = get<0>(unit)->getPosition() + mLocation;
		get<0>(unit)->drawAt(destBuffer, unitDrawLocation);
	}

	for (tuple<Unit*, vector<string>> unit : mWorldNPCUnits)
	{
		Vector2D unitDrawLocation = get<0>(unit)->getPosition() + mLocation;
		get<0>(unit)->drawAt(destBuffer, unitDrawLocation);
	}

	if (get<0>(mCurrUnitOverlappingPlayer) != nullptr)
	{
		Sprite& talkIcon = Game::getInstance()->getTalkIconSprite();
		destBuffer->draw(Vector2D(0, 0), talkIcon, 1);
		hud->writeText(Vector2D(talkIcon.getWidth() - 150, 20), *Game::getInstance()->getFont(), Color(1.0f, 1.0f, 1.0f), Game::getInstance()->getCurrTalkText());
	}
}

void World::update(float deltaTime, Unit* player)
{
	Unit::update(deltaTime);
	tuple<Unit*, vector<string>> prevUnitOverlappingPlayer = mCurrUnitOverlappingPlayer;
	get<0>(mCurrUnitOverlappingPlayer) = nullptr;

	for (tuple<Unit*, vector<string>> unit : mWorldEnemys)
	{
		get<0>(unit)->update(deltaTime);

		SDL_Rect playerRect = player->getRect();
		SDL_Rect unitRect = get<0>(unit)->getRect();
		unitRect = { unitRect.x + (int)mLocation.getX(), unitRect.y + (int)mLocation.getY(), unitRect.w, unitRect.h };
		if (SDL_HasIntersection(&unitRect, &playerRect))
		{
			mCurrUnitOverlappingPlayer = unit;
		}
	}

	for (tuple<Unit*, vector<string>> unit : mWorldNPCUnits)
	{
		get<0>(unit)->update(deltaTime);

		SDL_Rect playerRect = player->getRect();
		SDL_Rect unitRect = get<0>(unit)->getRect();
		unitRect = { unitRect.x + (int)mLocation.getX(), unitRect.y + (int)mLocation.getY(), unitRect.w, unitRect.h };
		if (SDL_HasIntersection(&unitRect, &playerRect))
		{
			mCurrUnitOverlappingPlayer = unit;
		}
	}

	handleCollision(player);
}

string World::getSaveData()
{
	string data = "";
	data += "World_Pos: " + to_string((int)mLocation.getX());
	data += " " + to_string((int)mLocation.getY());
	data += "\nWorld_Can_Move_X: " + to_string(mCanMoveOnX);
	data += "\nWorld_Can_Move_Y: " + to_string(mCanMoveOnY);
	data += "\nTotal_Enemies: " + to_string(mTotalEnemies);

	data += "\n\nWorld_Enemies:\n";
	for (tuple<Unit*, vector<string>> unit : mWorldEnemys)
	{
		data += "\nEnemyNPC:" + get<0>(unit)->getSaveData() + "\n";
		data += "EnemyNPC_Text:\n";
		for (string message : get<1>(unit))
		{
			data += "Text: " + message + "\n";
		}
	}

	data += "\nWorld_NPCs:\n";
	for (tuple<Unit*, vector<string>> unit : mWorldNPCUnits)
	{
		data += "\nNPC:" + get<0>(unit)->getSaveData() + "\n";
		data += "NPC_Text:\n";
		for (string message : get<1>(unit))
		{
			data += "Text: " + message + "\n";
		}
	}

	data += "\nWorld_Baracades:\n";
	for (SDL_Rect& rect : mBaracadeRects)
	{
		data += "Rect: " + to_string(rect.x) + " " + to_string(rect.y) + " " + to_string(rect.w) + " " + to_string(rect.h) + "\n";
	}

	auto iterator = find(mWorldNPCUnits.begin(), mWorldNPCUnits.end(), mCurrUnitOverlappingPlayer);
	int index = distance(mWorldNPCUnits.begin(), iterator);

	if (index != mWorldNPCUnits.size())
	{
		data += "\nCurr_Overlapping_Unit_Is_An_Enemy: " + to_string(false);
		data += "\nCurr_Overlapping_Unit_Index: " + to_string(index);
	}
	else
	{
		auto iterator = find(mWorldEnemys.begin(), mWorldEnemys.end(), mCurrUnitOverlappingPlayer);
		index = distance(mWorldEnemys.begin(), iterator);

		if (index != mWorldEnemys.size())
		{
			data += "\nCurr_Overlapping_Unit_Is_An_Enemy: " + to_string(true);
			data += "\nCurr_Overlapping_Unit_Index: " + to_string(index);
		}
	}

	data += "\n\n\n";

	return data;
}

void World::loadData(string fileData)
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

		if (subStrData.find("World_Pos: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("World_Pos: ", subStrData);

			splitter = " ";
			stringPos = subStrData.find(splitter);
			string playerXPos = subStrData.substr(0, stringPos);
			subStrData.erase(0, stringPos + splitter.length());

			mLocation.setX((float)stoi(playerXPos));
			mLocation.setY((float)stoi(subStrData));
		}
		else if (subStrData.find("World_Can_Move_X: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("World_Can_Move_X: ", subStrData);
			mCanMoveOnX = static_cast<bool>(stoi(data));
		}
		else if (subStrData.find("World_Can_Move_Y: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("World_Can_Move_Y: ", subStrData);
			mCanMoveOnY = static_cast<bool>(stoi(data));
		}
		else if (subStrData.find("Total_Enemies: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Total_Enemies: ", subStrData);
			mTotalEnemies = stoi(data);
		}
		else if (subStrData.find("Curr_Overlapping_Unit_Is_An_Enemy: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Curr_Overlapping_Unit_Is_An_Enemy: ", subStrData);
			bool isAnEnemy = static_cast<bool>(stoi(data));
			splitter = "\n";
			stringPos = fileData.find(splitter);
			string indexData = fileData.substr(0, stringPos);
			data = Game::getInstance()->extractDataFromDataString("Curr_Overlapping_Unit_Index: ", indexData);
			int index = stoi(data);

			if (isAnEnemy)
			{
				mCurrUnitOverlappingPlayer = mWorldEnemys[index];
			}
			else
			{
				mCurrUnitOverlappingPlayer = mWorldNPCUnits[index];
			}
		}
		else if (subStrData.find("World_Enemies:") != string::npos)
		{
			splitter = "EnemyNPC:\n";
			stringPos = fileData.find(splitter);
			while (stringPos != string::npos)
			{
				splitter = "EnemyNPC_Text:\n";
				stringPos = fileData.find(splitter);
				string enemyData = fileData.substr(0, stringPos);
				fileData.erase(0, stringPos + splitter.length());

				Unit* unit = new Unit();
				unit->loadData(enemyData);

				splitter = "\n\n";
				stringPos = fileData.find(splitter);
				string enemyTextData = fileData.substr(0, stringPos);
				vector<string> enemyText;

				string textData = "_";
				while (textData != "")
				{
					textData = Game::getInstance()->extractDataFromDataString("Text: ", enemyTextData);
					if (textData != "")
					{
						enemyText.push_back(textData);
					}
				}

				mWorldEnemys.push_back(make_tuple(unit, enemyText));

				fileData.erase(0, stringPos + splitter.length());
				splitter = "EnemyNPC:\n";
				stringPos = fileData.find(splitter);
			}

			splitter = "NPC:\n";
			stringPos = fileData.find(splitter);
			while (stringPos != string::npos)
			{
				splitter = "NPC_Text:\n";
				stringPos = fileData.find(splitter);
				string enemyData = fileData.substr(0, stringPos);
				fileData.erase(0, stringPos + splitter.length());

				Unit* unit = new Unit();
				unit->loadData(enemyData);

				splitter = "\n\n";
				stringPos = fileData.find(splitter);
				string enemyTextData = fileData.substr(0, stringPos);
				vector<string> enemyText;

				string textData = "_";
				while (textData != "")
				{
					textData = Game::getInstance()->extractDataFromDataString("Text: ", enemyTextData);
					if (textData != "")
					{
						enemyText.push_back(textData);
					}
				}

				mWorldNPCUnits.push_back(make_tuple(unit, enemyText));

				fileData.erase(0, stringPos + splitter.length());
				splitter = "NPC:\n";
				stringPos = fileData.find(splitter);
			}
		}
		else if (subStrData.find("World_Baracades:") != string::npos)
		{
			int stringPos = 0;
			while (stringPos != string::npos)
			{
				splitter = "Rect: ";
				stringPos = fileData.find(splitter);

				SDL_Rect rect;
				string rectData = Game::getInstance()->extractDataFromDataString("Rect: ", fileData);
				if (rectData != "")
				{
					splitter = " ";
					stringPos = rectData.find(splitter);
					string rectX = rectData.substr(0, stringPos);

					rectData.erase(0, stringPos + splitter.length());

					stringPos = rectData.find(splitter);
					string rectY = rectData.substr(0, stringPos);

					rectData.erase(0, stringPos + splitter.length());

					stringPos = rectData.find(splitter);
					string rectW = rectData.substr(0, stringPos);

					rectData.erase(0, stringPos + splitter.length());

					rect = {stoi(rectX), stoi(rectY), stoi(rectW), stoi(rectData)};
				}

				mBaracadeRects.push_back(rect);
				splitter = "Rect: ";
				stringPos = fileData.find(splitter);
			}
		}
	}
}
