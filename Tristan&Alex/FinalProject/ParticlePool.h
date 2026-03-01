#pragma once
#include "Particle.h"
#include <Trackable.h>
#include <queue>

class Animation;

class ParticlePool : public Trackable
{
public:
	ParticlePool() {};
	~ParticlePool();

	void init(int numberOfParticlesInPool, vector<Animation> particleAnimations, float animationSpeed, int startingAniIndex = 0);
	void cleanup();
	void deleteFreeList();

	void createParticle(Vector2D location, Vector2D velocity, float speed, int lifeTime);
	void resetFreeList();
	void resetParticles();

	Vector2D getParticleSpriteDimensions();

	void update(float deltaTime);
	void render(GraphicsBuffer* destBuffer);
private:
	queue<Particle*>* mpFreeList = nullptr;
	Particle* mParticles;
	int mNumbrOfParticles = 0;
};

