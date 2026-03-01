#include "Unit.h"
#include "GraphicsBuffer.h"
#include "UnitManager.h"
#include "Game.h"
#include "AnimationManager.h"

Unit::Unit(vector<Animation> animations, Vector2D location = Vector2D(0, 0), float speed = 5.0f, float animationSpeed = Animation::MEDIUM, int startingAnimationIndex = 0)
{
	mAnimations = animations;
	mLocation = location;
	mSpeed = speed;

	setUnitAnimation(startingAnimationIndex, animationSpeed);
}

Unit::Unit(vector<Animation> animations, float animationSpeed = Animation::MEDIUM, int startingAnimationIndex = 0, Vector2D location)
{
	mLocation = location;
	mAnimations = animations;
	setUnitAnimation(startingAnimationIndex, animationSpeed);
}

Unit::Unit()
{
	mAnimations = {};
}

Unit::~Unit()
{
	mAnimations.clear();
}

void Unit::update(float deltaTime)
{
	//Updates animation and unit position
	if ((unsigned int)mCurrAnimationIndex < mAnimations.size() && mCurrAnimationIndex >= 0)
	{
		mAnimations[mCurrAnimationIndex].update(deltaTime);
	}
	//mLocation = mLocation + mVelocity;
}

void Unit::setPosition(Vector2D newPos)
{
	mLocation = newPos;
}

Vector2D Unit::getPosition()
{
	return mLocation;
}

void Unit::setVelocity(Vector2D velocity)
{
	mVelocity = velocity;
}

SDL_Rect Unit::getRect()
{
	const Sprite* currSprite = getCurrAnimation()->getCurrentSprite();
	SDL_Rect unitRect = {(int)mLocation.getX(), (int)mLocation.getY(), (int)currSprite->getWidth(), (int)currSprite->getHeight()};
	return unitRect;
}

bool Unit::moveTo(Vector2D locationToMoveTo, float speed)
{
	bool hasMovedToLocation = false;
	Vector2D directionVector(locationToMoveTo - mLocation);
	float distenceFromPoint = directionVector.getLength();
	directionVector.normalize();

	if (distenceFromPoint <= speed)
	{
		mLocation = locationToMoveTo;
		hasMovedToLocation = true;
	}
	else
	{
		mLocation += directionVector * speed;
	}

	return hasMovedToLocation;
}

void Unit::draw(GraphicsBuffer* destBuffer)
{
	const Sprite* currSprite = mAnimations[mCurrAnimationIndex].getCurrentSprite();
	destBuffer->draw(mLocation, *currSprite, 1.0);
}

void Unit::drawAt(GraphicsBuffer* destBuffer, Vector2D location)
{
	const Sprite* currSprite = mAnimations[mCurrAnimationIndex].getCurrentSprite();
	destBuffer->draw(location, *currSprite, 1.0);
}

void Unit::setUnitAnimation(unsigned int index, float animationSpeed)
{
	SDL_assert(index < mAnimations.size());

	if (mCurrAnimationIndex != index) 
	{
		mCurrAnimationIndex = index;

		//Resets animation and sets speed so that the new animation starts at index 0 and has the same speed as the previous animation
		mAnimations[mCurrAnimationIndex].resetAnimation();
	}

	setUnitAnimationSpeed(animationSpeed);
}

void Unit::setUnitAnimationSpeed(float animationSpeed)
{
	mAnimationSpeed = animationSpeed;
	mAnimations[mCurrAnimationIndex].setSpeed(mAnimationSpeed);
}

void Unit::setAllAnimationsPaused(bool paused)
{
	for (Animation& animation : mAnimations)
	{
		animation.setPaused(paused);
	}
}

void Unit::toggleAnimation()
{
	switch (mCurrAnimationIndex)
	{
		case 0:
			setUnitAnimation(1, mAnimationSpeed);
			break;
		case 1:
			setUnitAnimation(0, mAnimationSpeed);
			break;
		default:
			setUnitAnimation(0, mAnimationSpeed);
			break;
	}
}

Animation* Unit::getCurrAnimation()
{
	return &mAnimations[mCurrAnimationIndex];
}

void Unit::setAnimations(vector<Animation> animations, float animationSpeed, int startingAnimationIndex)
{
	mAnimations = animations;
	mAnimationSpeed = animationSpeed;

	setUnitAnimation(startingAnimationIndex, mAnimationSpeed);
}

unsigned int Unit::getCurrAnimationIndex()
{
	return mCurrAnimationIndex;
}

string Unit::getSaveData()
{
	string data = "";

	data += "\nPosition: " + to_string((int)mLocation.getX());
	data += " " + to_string(mLocation.getY());
	data += "\nVelocity: " + to_string(mVelocity.getX());
	data += " " + to_string(mVelocity.getY());
	data += "\nSpeed: " + to_string(mSpeed);
	data += "\nAnimations:";

	for (Animation ani : mAnimations)
	{
		data += "\nAnimation: " + Game::getInstance()->getAnimationManager()->getAniDataKey(ani.getAnimationData());
		data += "\nIs_Looping: " + to_string(ani.getIsLooping());
	}

	data += "\nCurr_Animation_Index: " + to_string(mCurrAnimationIndex);
	data += "\nAnimation_Speed: " + to_string(mAnimationSpeed);

	return data;
}

void Unit::loadData(string& fileData)
{
	string splitter;
	int stringPos;
	while (fileData.length() > 0)
	{
		splitter = '\n';
		stringPos = fileData.find(splitter);
		string subStrData = fileData.substr(0, stringPos);

		if (stringPos != string::npos)
		{
			fileData.erase(0, stringPos + splitter.length());
		}
		else
		{
			fileData.erase(0);
		}


		if (subStrData.find("Position: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Position: ", subStrData);

			splitter = " ";
			stringPos = subStrData.find(splitter);
			string playerXPos = subStrData.substr(0, stringPos);
			subStrData.erase(0, stringPos + splitter.length());

			mLocation.setX((float)stoi(playerXPos));
			mLocation.setY((float)stoi(subStrData));

		}
		else if (subStrData.find("Velocity: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Velocity: ", subStrData);

			splitter = " ";
			stringPos = subStrData.find(splitter);
			string xVel = subStrData.substr(0, stringPos);
			subStrData.erase(0, stringPos + splitter.length());

			mVelocity.setX((float)stof(xVel));
			mVelocity.setY((float)stof(subStrData));

		}
		else if (subStrData.find("Speed: ") != string::npos)
		{
			string data = Game::getInstance()->extractDataFromDataString("Speed: ", subStrData);
			mSpeed = (float)stoi(data);
		}
		else if (subStrData.find("Animations:") != string::npos)
		{
			splitter = "\n";
			stringPos = fileData.find(splitter);
			subStrData = fileData.substr(0, stringPos);
			while (subStrData.find("Animation: ") != string::npos)
			{
				string animationKey = Game::getInstance()->extractDataFromDataString("Animation: ", fileData);
				bool isLooping = static_cast<bool>(stoi(Game::getInstance()->extractDataFromDataString("Is_Looping: ", fileData)));
				mAnimations.push_back(Animation(Game::getInstance()->getAnimationManager()->getAnimationData(animationKey), isLooping));

				stringPos = fileData.find(splitter);
				subStrData = fileData.substr(0, stringPos);
			}

			string currAnimation = Game::getInstance()->extractDataFromDataString("Curr_Animation_Index: ", fileData);

			stringPos = fileData.find(splitter);
			subStrData = fileData.substr(0, stringPos);

			string animationSpeed = Game::getInstance()->extractDataFromDataString("Animation_Speed: ", fileData);
			setUnitAnimation(stoi(currAnimation), static_cast<float>(stoi(animationSpeed)));
		}
	}
}
