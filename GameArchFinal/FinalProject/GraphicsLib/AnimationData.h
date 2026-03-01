#pragma once
#include <Trackable.h>
#include <vector>

class GraphicsBuffer;
class Sprite;

using namespace std;

class AnimationData : public Trackable
{
public:
	AnimationData(GraphicsBuffer* spriteSheetBuffer, int spriteWidth, int spriteHeight, int rows, int cols, int startingIndex, int endIndex);
	~AnimationData();

	const Sprite* getSprite(unsigned int index) const;
	int getNumOfSprites();

private:
	vector<Sprite> mSpriteList = {};
};

