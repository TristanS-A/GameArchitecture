#pragma once
#include <vector>
#include <Vector2D.h>
#include <Trackable.h>
#include <EventSystem.h>
#include "Animation.h" //This is needed here to use the enum in the header file (idk if it should be done like this)

class GraphicsBuffer;
class Unit;
class Player;
class Enemy;
class Bullet;
class HUD;
class Font;
class World;

using namespace std;

class UnitManager : public Trackable
{
public:
	UnitManager();
	~UnitManager();
	void init();
	void cleanup();
	void resetUnits();

	void createPlayer(Player* thePlayer);
	void addEnemy(Enemy* enemyToAdd);
	void addBullet(Bullet* bulletToAdd);
	void createWorld(vector<Animation> animations, Vector2D location, float speed, float animationSpeed = Animation::SLOW, int startingAnimationIndex = 0);
	void deleteEnemy(Enemy* enemyToDelete);
	void removeEnemyFromActiveList(Enemy* enemyToDelete);
	void addEnemyToActiveList(Enemy* enemyToDelete);
	bool deleteBullet(Bullet* bulletToDelete);

	void setAllUnitsAnimationSpeed(float speed);
	void toggleAllUnitsAnimationsPaused();
	void setAllUnitsAnimationsPaused(bool paused);

	Player* getPlayer() { return mpPlayer; };
	World* getWorld() { return mpWorld; };
	void setStartingPlayerHealth();
	bool getNoMoreBulletsOnScreen() { return mBulletList.size() == 0; };
	void clearBullets();
	void clearEnemies();
	bool getNoMoreEnemies() { return mActiveEnemyList.size() == 0; };
	bool getAllUnitsAnimationsPaused() { return mAllUnitsAnimationsPaused; };
	Animation::Animation_Speed getCurrUnitAnimationSpeed() { return mCurrUnitAnimationSpeed; };
	void setAnimationSpeed(Animation::Animation_Speed newSpeed) { mCurrUnitAnimationSpeed = newSpeed; };
	bool getAllEnemiesEmpty();

	void decideCurrEnemyActions();
	vector<Vector2D> getEnemyBattleLocations();
	void haveEnemiesAttack();
	bool checkForBulletCollisionsAndDestroyBullet(Bullet* bullet);
	bool relocateBattleUnits(float speedToRelocate);
	void setBattleUnitLocations();
	void resetBattle();
	void handleWorldPlayerMovement();

	void loadDataFromFile();
	void loadUnitDataFromFile(string& data);
	string getUnitSaveData();
	void loadWorldNPCs(float animationSpeed, vector<vector<string>> npcDiologue, vector<vector<string>> npcEnemyDiologue);

	void update(float deltaTime);
	void drawAll(GraphicsBuffer* destBuffer, GraphicsBuffer* hud);
	void drawWorld(GraphicsBuffer* destBuffer, GraphicsBuffer* hud);
	void drawTempUnits(GraphicsBuffer* destBuffer);

	void displayUnitInfo(HUD* hud, Font* font);
private:
	Player* mpPlayer = nullptr;
	vector<Enemy*> mActiveEnemyList = {};
	vector<Enemy*> mDeactiveEnemyList = {};
	vector<Bullet*> mBulletList = {};
	World* mpWorld = nullptr;

	int mStartingBattlePlayerHealth;
	int mPlayerBulletDamage = 0;
	int mEnemyBulletDamage = 0;

	bool mAllUnitsAnimationsPaused = false;
	Animation::Animation_Speed mCurrUnitAnimationSpeed = Animation::MEDIUM;
};