#include "ParticlePool.h"

ParticlePool::~ParticlePool()
{
	cleanup();
}

void ParticlePool::init(int numberOfParticlesInPool, vector<Animation> particleAnimations, float animationSpeed, int startingAniIndex)
{
	mParticles = new Particle[numberOfParticlesInPool];
	mNumbrOfParticles = numberOfParticlesInPool;

	for (int i = 0; i < mNumbrOfParticles; i++)
	{
		mParticles[i].setAnimations(particleAnimations, animationSpeed, startingAniIndex);
	}

	resetFreeList();
}

void ParticlePool::cleanup()
{
	deleteFreeList();
	delete[] mParticles;
	mParticles = nullptr;
}

void ParticlePool::deleteFreeList()
{
	if (mpFreeList != nullptr)
	{
		delete mpFreeList;
		mpFreeList = nullptr;
	}
}

void ParticlePool::createParticle(Vector2D location, Vector2D velocity, float speed, int lifeTime)
{
	velocity.normalize();
	if (!mpFreeList->empty())
	{
		mpFreeList->front()->init(location, velocity, speed, lifeTime);
		mpFreeList->pop();
		return;
	}
	else
	{
		Particle* firstMadeParticle = &mParticles[0];
		for (int i = 1; i < mNumbrOfParticles; i++)
		{
			if (firstMadeParticle->getLifeLeft() > mParticles[i].getLifeLeft())
			{
				firstMadeParticle = &mParticles[i];
			}
		}

		firstMadeParticle->init(location, velocity, speed, lifeTime);
	}
}

void ParticlePool::resetFreeList()
{
	deleteFreeList();
	mpFreeList = new queue<Particle*>;
	for (int i = 0; i < mNumbrOfParticles; i++)
	{
		mpFreeList->push(&mParticles[i]);
	}
}

void ParticlePool::resetParticles()
{
	for (int i = 0; i < mNumbrOfParticles; i++)
	{
		if (mParticles[i].isAlive())
		{
			mParticles[i].kill();
		}
	}

	resetFreeList();
}

Vector2D ParticlePool::getParticleSpriteDimensions()
{
	SDL_assert(mNumbrOfParticles > 0);
	const Sprite* particleSprite = mParticles[0].getCurrAnimation()->getCurrentSprite();

	return Vector2D((float)particleSprite->getWidth(), (float)particleSprite->getHeight());
}

void ParticlePool::update(float deltaTime)
{
	for (int i = 0; i < mNumbrOfParticles; i++)
	{
		if (mParticles[i].doTheThing(deltaTime))
		{
			mpFreeList->push(&mParticles[i]);
		}
	}
}

void ParticlePool::render(GraphicsBuffer* destBuffer)
{
	for (int i = 0; i < mNumbrOfParticles; i++)
	{
		if (mParticles[i].isAlive())
		{
			mParticles[i].draw(destBuffer);
		}
	}
}
