#pragma once
#include "Unit.h"
#include <EventListener.h>
#include <vector>

using namespace std;

class Animation;
class GameEvent;
class ParticlePool;

class Player : public Unit, public EventListener
{
public:
	Player(vector<Animation> animations, float animationSpeed, int startingAniIndex = 0, Vector2D location = ZERO_VECTOR2D);
	~Player();

	void init();
	void cleanup();

	void handleEvent(const Event& theEvent);

	void playerMoveUp();
	void playerMoveLeft();
	void playerMoveDown();
	void playerMoveRight();
	void playerDash();

	void handleStayOnScreen(Vector2D gameWindowDimension);

	bool getIsPlayerEmpty();
	void fire(const Event& theEvent);
	void addToPlayerHealth(int amountToAdd);
	void setHealth(int health) { mHealth = health; };
	int getHealth() { return mHealth; };
	void resetPlayer(int health);
	void spawnTeleportParticles(ParticlePool* particlePool);
	void setCanMoveOnXAxis(bool canMoveOnX) { mCanMoveOnX = canMoveOnX; };
	void setCanMoveOnYAxis(bool canMoveOnY) { mCanMoveOnY = canMoveOnY; };
	void setCanDash(bool canDash) { mCanDash = canDash; };
	void setCanShoot(bool canShoot) { mCanShoot = canShoot; };

	int getNumBullets() const { return mNumberOfBullets; };
	float getSpeed() { return mVelocity.getLength(); };
	bool getPlayerCanMove() { return !mPlayerMotionLocked; };

	string getSaveData() override;
	void loadData(string& fileData) override;

	void loadPlayerData();

private:
	bool mCanMoveOnX;
	bool mCanMoveOnY;
	bool mPlayerMotionLocked;
	bool mCanShoot;
	bool mCanDash;
	bool mSkipNextTurn;

	int mNumberOfBullets = 0;
	int mNumberOfBulletsPerTurn = 0;
	int mHealth = 0;
	int mMaxHealth = 0;

	int mDashDistence = 200;

	int mNumParticlesPerTeleportation = 0;
	int mParticleSpeed = 1;
	int mParticleLifeTime = 30;
	int mParticleDirXHigh = 400;
	int mParticleDirYHigh = 400;
	int mParticleDirXLow = -400;
	int mParticleDirYLow = -400;
	int mParticleDisHigh = 100;
	int mParticleDisLow = 0;
	int mMaxParticleDisFromPlayer = 50;
	int mMaxParticleSpeedOffset = 3;
};