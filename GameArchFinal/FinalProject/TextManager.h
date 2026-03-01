#pragma once
#include <Trackable.h>
#include <unordered_map>
#include <vector>

using namespace std;

typedef string TextKey;

class TextManager : public Trackable
{
public:
	TextManager() {};
	~TextManager() {};

	void addText(TextKey key, vector<string> textToAdd);
	vector<string> getText(TextKey key);
private:
	unordered_map<TextKey, vector<string>> mTextMap;
};

