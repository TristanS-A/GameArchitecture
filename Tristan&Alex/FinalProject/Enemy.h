#pragma once
#include "Unit.h"
#include <Vector2D.h>
#include <vector>
#include <EventListener.h>

using namespace std;

class Animation;
class GameEvent;

class Enemy : public Unit, public EventListener
{
public:
	Enemy(vector<Animation> animations, float animationSpeed, int startingAnimationIndex = 0, Vector2D location = ZERO_VECTOR2D);
	Enemy();
	~Enemy();

	void init();
	void cleanup();
	void loadEnemyData();

	void handleEvent(const Event& theEvent);

	void decideAction();
	void attack(Vector2D playerPos, vector<Enemy*> otherEnemies, Vector2D gameWindowDimension);
	void fire(Vector2D playerPos);
	bool getIsEmpty() { return mNumberOfBullets == 0; }
	void addToHealth(int amountToAdd);
	int getHealth() { return mHealth; };
	void setHealth(int health) { mHealth = health; };
	void resetEnemy();
	void handleStayOnScreen(Vector2D gameWindowDimension);

	string getSaveData() override;
	bool loadDataAndGetIsActive(string& fileData);
private:
	int mHealth = 0;
	int mMaxHealth = 0;
	int mNumberOfBullets = 0;
	int mNumberOfBulletsPerTurn = 0;
	int mShootChancePerFrame = 0;
	int mDistenceFromPlayer = 0;
	int mDistenceFromOtherEnemies = 0;
};

