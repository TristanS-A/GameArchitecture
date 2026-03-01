#include "Bullet.h"
#include "Animation.h"
#include <string>
#include "Game.h"

Bullet::Bullet(vector<Animation> animations, Vector2D location, float speed, float animationSpeed, bool shotByPlayer, int startingAnimationIndex) :
	Unit(animations, location, speed, animationSpeed, startingAnimationIndex), mShotByPlayer(shotByPlayer)
{
	init();
}

Bullet::Bullet() : Unit()
{
	init();
}

Bullet::~Bullet()
{
	cleanup();
}

void Bullet::init()
{
}

void Bullet::cleanup()
{
}

void Bullet::update(float deltaTime)
{
	Unit::update(deltaTime);
	mLocation += mVelocity * mSpeed;
}

string Bullet::getSaveData()
{
	string data = "";

	data += Unit::getSaveData();
	data += "\nWas_Shot_By_Player: " + to_string(mShotByPlayer);

	return data;
}

void Bullet::loadData(string& fileData)
{
	string splitter = "Was_Shot_By_Player: ";
	int stringPos = fileData.find(splitter);
	string unitData = fileData.substr(0, stringPos);
	fileData.erase(0, stringPos);

	Unit::loadData(unitData);

	string shotByPlayer = Game::getInstance()->extractDataFromDataString("Was_Shot_By_Player: ", fileData);
	mShotByPlayer = stoi(shotByPlayer);
}
