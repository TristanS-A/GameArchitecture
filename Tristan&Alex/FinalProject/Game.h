#pragma once
#include <EventListener.h>
#include <EventSystem.h>
#include <PerformanceTracker.h>
#include <Timer.h>
#include <vector>
#include <unordered_map>
#include "Vector2D.h"
#include <SDL.h>
#include "Button.h"
#include "UIMenu.h"
#include "UIMenuManager.h"

using namespace std;

class GraphicsSystem;
class InputSystem;
class InputTranslator;
class UnitManager;
class GraphicsBufferManager;
class GraphicsBuffer;
class SoundManager;
class HUD;
class Font;
class Player;
class AnimationManager;
class TextManager;
class ParticlePool;
class World;

class Game : public EventListener
{
public:
	enum Screens {
		TITLE_SCREEN,
		OPTIONS_SCREEN,
		OVERWORLD_SCREEN,
		GAME_SCREEN,
		GAME_OVER_SCEEN,
		WIN_GAME_SCREEN
	};

	enum BattleState
	{
		PLAYER_TURN,
		ENEMY_TURN,
		SHOOTING_MODE,
		RELOCATING_UNITS,
		WIN_BATTLE
	};

	enum Difficulty
	{
		EASY,
		MEDIUM,
		HARD
	};

	static void createInstance();
	static Game* getInstance();
	static void deleteInstance();
	static Vector2D getDisplayDimensions();

	bool init(unsigned int displayWidth, unsigned int displayHeight, float FPS = 16.7);
	void cleanup();

	void doLoop();
	static UnitManager* getUnitManager();
	Sprite& getTextBGSprite() { return mTextBGSprite; };
	Sprite& getTalkIconSprite() { return mTalkIcon; };
	Screens getCurrScreen();
	static bool getGameIsPaused();

	AnimationManager* getAnimationManager();
	bool getIsMessages() { return mpMenuManager->getIsMessages(); };

	string extractDataFromDataString(string dataToFindSplitter, string& fileData);

	void handleEvent(const Event& theEvent);

	string getCurrBulletsRemainingText() { return mCurrBulletsRemainingText; };
	string getCurrHealthDisplayText() { return mCurrHealthDisplayText; };
	string getCurrTalkText() { return mCurrTalkText; };
	int getEnemyData(string dataToGet);
	int getPlayerData(string dataToGet);
	Difficulty getCurrentDifficulty() { return mCurrDifficulty; };
	void switchDifficulty(Difficulty difficulty);
	ParticlePool* getPlayerParticlePool() { return mpPlayerParticlePool; };
	Player* getPlayer();
	World* getWorld();
	Font* getFont() { return mpFont; };

	void createPlayer();
	void createWorld();
	void createEnemy();
	void saveGame();

private:
	static Game* mpsInstance;
	Game();
	~Game();

	void loadBuffers();
	void loadAnimations();
	void loadSounds();
	void loadFont();
	void loadDataFromFile();
	void loadUIMenus();
	void loadNPCDiologue();
	void addEventListeners();

	void createBullet(Vector2D spawnPos, Vector2D shootAtPos, bool shotByPlayer);
	vector<string> getSplitText(string text, float textWidth);
	void winBattle();

	void loadGame();
	void resetAndStartGame();

	void update(float deltaTime);
	void render();

	void switchTextLanguage(Button::LanguageOptions language);

	GraphicsSystem* mpGraphicsSystem = nullptr;
	InputSystem* mpInputSystem = nullptr;
	InputTranslator* mpInputTranslator = nullptr;
	UnitManager* mpUnitManager = nullptr;
	GraphicsBufferManager* mpBufferManager = nullptr;
	SoundManager* mpSoundManager = nullptr;
	UIMenuManager* mpMenuManager = nullptr;
	AnimationManager* mpAnimationManager = nullptr;
	TextManager* mpTextManager = nullptr;
	ParticlePool* mpPlayerParticlePool = nullptr;
	HUD* mpHUD = nullptr;
	Font* mpFont = nullptr;
	bool mRunning = false;


	Screens mCurrScreen = TITLE_SCREEN;
	bool mGamePaused = false;
	GraphicsBuffer* mpCurrBackgroundBuffer = nullptr;
	float mFPS = 0;
	Button::LanguageOptions mCurrLanguage = Button::ENGLISH;
	UIMenu::MenuType mPrevMenu;

	bool mInBattle = false;
	BattleState mBattleState;

	Difficulty mCurrDifficulty = MEDIUM;

	int mNumberOfPlayerParticles = 0;
	int mCurrBulletSpeed = 0;
	int mBulletSpeedEasy = 0;
	int mBulletSpeedMedium = 0;
	int mBulletSpeedHard = 0;
	int mRelocateBattleUnitsSpeed = 0;
	Vector2D mOverworldPlayerPos;

	Sprite mTextBGSprite;
	Sprite mPauseBGSprite;
	Sprite mTalkIcon;

	unordered_map<string, int> mEnemyDataMap;
	unordered_map<string, int> mPlayerDataMap;

	string mCurrBulletsRemainingText;
	string mCurrPlayerTurnText;
	string mCurrPlayerWinText;
	string mCurrHealthDisplayText;
	string mCurrTalkText;

	vector<vector<string>> mNPCDiologueEnglish;
	vector<vector<string>> mNPCEnemyDiologueEnglish;
	vector<vector<string>> mNPCDiologueSpanish;
	vector<vector<string>> mNPCEnemyDiologueSpanish;
	vector<vector<string>> mNPCDiologueFrench;
	vector<vector<string>> mNPCEnemyDiologueFrench;
};