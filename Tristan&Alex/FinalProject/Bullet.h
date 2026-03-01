#pragma once
#include "Unit.h"
#include <vector>

using namespace std;

class Animation;

class Bullet : public Unit
{
public:
	Bullet(vector<Animation> animations, Vector2D location, float speed, float animationSpeed, bool shotByPlayer, int startingAnimationIndex = 0);
	Bullet();
	~Bullet();

	void init();
	void cleanup();

	void update(float deltaTime);
	bool getShotByPlayer() { return mShotByPlayer; };

	string getSaveData() override;
	void loadData(string& fileData) override;
private:
	bool mShotByPlayer;
};