#include "AnimationManager.h"
#include "AnimationData.h"

AnimationManager::AnimationManager()
{
}

AnimationManager::~AnimationManager()
{
	cleanup();
}

void AnimationManager::cleanup()
{
	for (auto iterator : mAniDataMap)
	{
		AnimationData* aniData = iterator.second;
		delete aniData;
		aniData = nullptr;
	}

	mAniDataMap.clear();
}

AnimationData* AnimationManager::addAnimationData(const AniDataKey& key, AnimationData* aniData)
{
	AnimationData* newAni = nullptr;
	auto iterator = mAniDataMap.find(key);

	if (iterator == mAniDataMap.end())
	{
		newAni = aniData;
		mAniDataMap[key] = newAni;
	}

	return newAni;
}

void AnimationManager::deleteAnimationData(const AniDataKey& key)
{
	auto iterator = mAniDataMap.find(key);

	if (iterator != mAniDataMap.end())
	{
		delete iterator->second;
		mAniDataMap.erase(iterator);
	}
}

void AnimationManager::deleteAnimationData(AnimationData* aniDataToDelete)
{
	for (auto iterator : mAniDataMap)
	{
		if (iterator.second == aniDataToDelete)
		{
			delete iterator.second;
			mAniDataMap.erase(iterator.first);
			return;
		}
	}
}

AnimationData* AnimationManager::getAnimationData(const AniDataKey& key) const
{
	auto iterator = mAniDataMap.find(key);

	if (iterator != mAniDataMap.end())
	{
		return iterator->second;
	}

	return nullptr;
}

AniDataKey AnimationManager::getAniDataKey(AnimationData* aniDataToFind)
{
	for (auto iterator : mAniDataMap)
	{
		if (iterator.second == aniDataToFind)
		{
			return iterator.first;
		}
	}

	return "";
}
