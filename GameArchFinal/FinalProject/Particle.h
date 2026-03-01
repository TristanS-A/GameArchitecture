#pragma once
#include "Unit.h"

class Animation;

class Particle : public Unit
{
public:
	Particle(vector<Animation> animations, float animationSpeed, int startingAnimationIndex = 0);
	Particle();
	~Particle() {};

	void init(Vector2D location, Vector2D velocity, float speed, int lifeTime);
	bool isAlive() { return mLifeLeft > 0; };
	bool doTheThing(float deltaTime);
	int getLifeLeft() { return mLifeLeft; };
	void kill();

	void draw(GraphicsBuffer* destBuffer) override;
private:
	int mLifeLeft;
	int mTotalLifeTime;
	float mScale;
};

