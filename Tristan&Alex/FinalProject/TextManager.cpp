#include "TextManager.h"

void TextManager::addText(TextKey key, vector<string> textToAdd)
{
	auto iterator = mTextMap.find(key);

	if (iterator == mTextMap.end())
	{
		mTextMap[key] = textToAdd;
	}
}

vector<string> TextManager::getText(TextKey key)
{
	auto iterator = mTextMap.find(key);

	if (iterator != mTextMap.end())
	{
		return iterator->second;
	}

	return {};
}
