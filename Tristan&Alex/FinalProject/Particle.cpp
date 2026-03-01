#include "Particle.h"
#include "Animation.h"
#include "GraphicsBuffer.h"

Particle::Particle(vector<Animation> animations, float animationSpeed, int startingAnimationIndex) : Unit(mAnimations, animationSpeed, startingAnimationIndex)
{
	mLifeLeft = 0;
}

Particle::Particle() : Unit()
{
	mLifeLeft = 0;
}

void Particle::init(Vector2D location, Vector2D velocity, float speed, int lifeTime)
{
	mLocation = location;
	mVelocity = velocity;
	mSpeed = speed;
	mLifeLeft = lifeTime;
	mTotalLifeTime = lifeTime;
	mScale = 1;

	getCurrAnimation()->resetAnimation(); //Resets current animation
}

bool Particle::doTheThing(float deltaTime)
{
	if (!isAlive())
	{
		return false;
	}

	update(deltaTime);
	mLocation += mVelocity * mSpeed;
	mScale = mLifeLeft / (float)mTotalLifeTime;
	mLifeLeft--;

	return mLifeLeft == 0;
}

void Particle::kill()
{
	mLifeLeft = 0;
}

void Particle::draw(GraphicsBuffer* destBuffer)
{
	const Sprite* currSprite = mAnimations[mCurrAnimationIndex].getCurrentSprite();
	destBuffer->draw(mLocation, *currSprite, mScale);
}
