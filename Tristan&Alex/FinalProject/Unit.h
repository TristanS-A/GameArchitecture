#pragma once
#include <Vector2D.h>
#include <Trackable.h>
#include <vector>
#include <SDL.h>
#include "Animation.h"

using namespace std;

class GraphicsBuffer;

class Unit : public Trackable
{
public:
	Unit(vector<Animation> animations, Vector2D location, float speed, float animationSpeed, int startingAnimationIndex);
	Unit(vector<Animation> animations, float animationSpeed, int startingAnimationIndex, Vector2D location = ZERO_VECTOR2D);
	Unit();
	~Unit();

	virtual void update(float deltaTime);
	void setPosition(Vector2D newPos);
	Vector2D getPosition();
	Vector2D getVelocity() { return mVelocity; };
	void setVelocity(Vector2D velocity);
	SDL_Rect getRect();
	bool moveTo(Vector2D locationToMoveTo, float speed);
	virtual void draw(GraphicsBuffer* destBuffer);
	virtual void drawAt(GraphicsBuffer* destBuffer, Vector2D location);
	void setUnitAnimation(unsigned int index, float animationSpeed);
	void setUnitAnimationSpeed(float animationSpeed);
	void setAllAnimationsPaused(bool paused);
	void toggleAnimation();
	Animation* getCurrAnimation();
	void setAnimations(vector<Animation> animations, float animationSpeed, int startingAnimationIndex = 0);
	unsigned int getCurrAnimationIndex();

	virtual string getSaveData();
	virtual void loadData(string& fileData);

protected:
	Vector2D mLocation = ZERO_VECTOR2D;
	vector<Animation> mAnimations = {};
	int mCurrAnimationIndex = 0;
	float mAnimationSpeed = 0;
	Vector2D mVelocity = Vector2D(1, 0);
	float mSpeed = 0;
};