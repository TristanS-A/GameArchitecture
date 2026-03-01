#pragma once
#include <Trackable.h>
#include <unordered_map>
#include <EventListener.h>
#include "UIMenu.h"
#include "ButtonManager.h"

using namespace std;

class UIMenuManager : public EventListener
{
public:
	UIMenuManager();
	~UIMenuManager();

	void init();
	void cleanup();

	void createAndAddNewMenu(UIMenu::MenuType type);
	void addButtonToCurrMenu(const Button::Buttons& key, Vector2D location, Sprite* buttonSprite, Sprite* hoveredSprite, Font* font, SoundClipKey clipKey, vector<string> textList, vector<string> alTextList = {}, Vector2D scale = Vector2D(1, 1));
	void addUISpriteToCurrMenu(Sprite sprite, string spriteKey);
	void setCurrMenu(UIMenu::MenuType type);
	UIMenu::MenuType getCurrMenu();
	
	void addToMessageList(string message);
	void removeFirstFromMessageList();
	string getCurrMessage();
	bool getIsMessages();

	void renderCurrMenu(GraphicsBuffer* renderBuffer);

	string getSaveData();
	void loadData(string& fileData);

	string getMessagesSaveData();
	void clearMessageList() { mMessageList.clear(); };

	void handleEvent(const Event& theEvent);

private:
	void deleteMenu(UIMenu::MenuType type);
	UIMenu* getMenu(UIMenu::MenuType type);
	void setPlayerAttackMenuEnabled(bool enabled);

	unordered_map<UIMenu::MenuType, UIMenu*> mMenuMap;
	UIMenu* mpCurrMenu = nullptr;
	ButtonManager* mpButtonManager = nullptr;

	vector<string> mMessageList = {};
};

