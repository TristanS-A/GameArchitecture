#pragma once
#include <Trackable.h>
#include <vector>
#include <unordered_map>
#include <Vector2D.h>
#include "Button.h"

using namespace std;

class GraphicsBuffer;
class Sprite;

class UIMenu : public Trackable
{
public:
	enum MenuType
	{
		NO_MENU,
		BATTLE_MENU,
		TITLE_MENU,
		OPTIONS_MENU,
		PAUSE_MENU,
		GAME_OVER_MENU,
		WIN_GAME_MENU
	};

	UIMenu(MenuType type);
	~UIMenu();

	void init();
	void cleanup();

	void addButton(Button* button);
	void addUISprite(Sprite sprite, string spriteKey);
	Button* getButtonAtPoint(Vector2D point);

	MenuType getType();
	Button* getButtonbyType(Button::Buttons buttonType);
	Sprite* getSprite(string key);

	void updateMenu(Vector2D mousePos);
	void renderMenu(GraphicsBuffer* renderBuffer);

	string getSaveData();
	void loadData(string& fileData);

private:
	vector<Button*> mButtonList = {};
	unordered_map<string, Sprite> mSpriteList = {};
	MenuType mType;
};

