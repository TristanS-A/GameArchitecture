#pragma once
#include <Trackable.h>
#include <unordered_map>

class AnimationData;

typedef std::string AniDataKey;

class AnimationManager : public Trackable
{
public:
	AnimationManager();
	~AnimationManager();
	void init() {};
	void cleanup();

	AnimationData* addAnimationData(const AniDataKey& key, AnimationData* aniData);
	void deleteAnimationData(const AniDataKey& key);
	void deleteAnimationData(AnimationData* aniDataToDelete);
	AnimationData* getAnimationData(const AniDataKey& key) const;
	AniDataKey getAniDataKey(AnimationData* aniDataToFind);
private:
	std::unordered_map<AniDataKey, AnimationData*> mAniDataMap;
};

