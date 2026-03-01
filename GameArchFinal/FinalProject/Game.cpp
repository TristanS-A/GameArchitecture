#include "Game.h"
#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "InputTranslator.h"
#include "GraphicsBuffer.h"
#include "UnitManager.h"
#include "GraphicsBufferManager.h"
#include "AnimationManager.h"

#include "GameEvent.h"
#include "ChangeUnitAnimationEvent.h"
#include "ChangeUnitAnimationSpeedEvent.h"
#include "ClickButtonEvent.h"
#include "FireGunEvent.h"
#include "MoveMouseEvent.h"
#include "AddToMessageListEvent.h"
#include "PlaySoundEvent.h"
#include "ChangeLanguageEvent.h"

#include "SoundManager.h"
#include "HUD.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "AnimationData.h"
#include "TextManager.h"
#include "ParticlePool.h"
#include "World.h"
#include "Game.h"

#include <fstream>
#include <sstream>

Game* Game::mpsInstance = nullptr;

Game::Game()
{
}

Game::~Game()
{
	cleanup();
}

void Game::createInstance()
{
	if (mpsInstance == nullptr)
	{
		mpsInstance = new Game();
	}
}

Game* Game::getInstance()
{
	SDL_assert(mpsInstance);

	return mpsInstance;
}

void Game::deleteInstance()
{
	delete mpsInstance;
	mpsInstance = nullptr;
}

Vector2D Game::getDisplayDimensions()
{
	//Returns the game window dimensions if the instance exists
	if (mpsInstance != nullptr && mpsInstance->mpGraphicsSystem != nullptr)
	{
		GraphicsBuffer* backBuffer = mpsInstance->mpGraphicsSystem->getBackBuffer();
		SDL_Rect bufferRect = backBuffer->getDimentions();
		return Vector2D(bufferRect.w, bufferRect.h);
	}

	return Vector2D(0, 0);
}

bool Game::init(unsigned int displayWidth, unsigned int displayHeight, float FPS)
{
	//Creates dynamic pointers
	mpGraphicsSystem = new GraphicsSystem();
	mpInputSystem = new InputSystem();
	mpInputTranslator = new InputTranslator();
	mpUnitManager = new UnitManager();
	mpBufferManager = new GraphicsBufferManager();
	mpSoundManager = new SoundManager();
	mpMenuManager = new UIMenuManager();
	mpHUD = new HUD();
	mpAnimationManager = new AnimationManager();
	mpTextManager = new TextManager();
	mpPlayerParticlePool = new ParticlePool();

	if (!mpGraphicsSystem->init(displayWidth, displayHeight))
	{
		return false;
	}
	if (!mpInputSystem->init()) 
	{
		return false;
	}

	mpHUD->initGraphicsBuffer(displayWidth, displayHeight);
	mpUnitManager->init();
	mpSoundManager->init();

	//Sets fps for game
	mFPS = FPS;

	//Loads the things
	loadFont();
	loadBuffers();
	loadAnimations();
	loadDataFromFile(); //Loads data from file before loading sounds to add any sounds that are loaded in from the file
	loadSounds();
	loadUIMenus();
	loadNPCDiologue();

	//Creates particle pool
	vector<Animation> animations = { Animation(mpAnimationManager->getAnimationData("sparkleAnimation"), true) };
	mpPlayerParticlePool->init(mNumberOfPlayerParticles, animations, Animation::MEDIUM);

	//Creates sprites for ui
	GraphicsBuffer* textBGBuffer = mpBufferManager->getGraphicsBuffer("textBackgroundBuffer");
	mTextBGSprite = Sprite(textBGBuffer, Vector2D(textBGBuffer->getDimentions().x, textBGBuffer->getDimentions().y), textBGBuffer->getDimentions().w, textBGBuffer->getDimentions().h);	
	GraphicsBuffer* pauseBGBuffer = mpBufferManager->getGraphicsBuffer("pauseBackgroundBuffer");
	mPauseBGSprite = Sprite(pauseBGBuffer, Vector2D(0, 0), pauseBGBuffer->getDimentions().w, pauseBGBuffer->getDimentions().h);
	GraphicsBuffer* talkIconBuffer = mpBufferManager->getGraphicsBuffer("talkIconBuffer");
	mTalkIcon = Sprite(talkIconBuffer, Vector2D(0, 0), talkIconBuffer->getDimentions().w, talkIconBuffer->getDimentions().h);

	//Adds event listeners
	addEventListeners();

	//Seeds randomness
	srand((unsigned)time(NULL));;

	//Sets language to english and sets dificulty to medium
	EventSystem::getInstance()->fireEvent(ChangeLanguageEvent(Button::ENGLISH));
	switchDifficulty(MEDIUM);

	return true;
}

void Game::loadBuffers()
{
	const string LOCAL_ASSET_PATH = "..\\..\\GameArchFinal\\assets\\";
	const string ENEMY_FILENAME = "enemySpriteSheet.png";
	const string PLAYER_FILENAME = "playerSpriteSheet.png";
	const string BATTLE_BG_FILENAME = "battleBG.png";
	const string TITLE_FILENAME = "titleBG.png";
	const string WIN_BG_FILENAME = "winBG.png";
	const string BUTTONS_FILENAME = "buttons.png";
	const string ORB_SHEET_FILENAME = "orbSpriteSheet.png";
	const string BULLETS_FILENAME = "bulletSpriteSheet.png";
	const string TEXTBOX_FILENAME = "textBox.png";
	const string PLAYER_PARTICLES_FILENAME = "sparkleSheet.png";
	const string WORLD_FILENAME = "worldImage.png";
	const string LOGO_FILENAME = "titleLogo.png";
	const string WESTERNER_SPRITE_SHEET1_FILENAME = "westernerSpriteSheet1.png";
	const string WESTERNER_SPRITE_SHEET2_FILENAME = "westernerSpriteSheet2.png";
	const string PAUSE_ICON_FILENAME = "pauseIcon.png";
	const string PAUSE_BG_FILENAME = "pauseBG.png";
	const string TEXT_BG_FILENAME = "textBG.png";
	const string TALK_ICON_BUFFER = "talkIcon.png";

	//Creates and adds Graphics Buffers to buffer manager
	mpBufferManager->createAndAddGraphicsBuffer("battleBackgroundBuffer", LOCAL_ASSET_PATH + BATTLE_BG_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("titleBackgroundBuffer", LOCAL_ASSET_PATH + TITLE_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("winBackgroundBuffer", LOCAL_ASSET_PATH + WIN_BG_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("buttonsBuffer", LOCAL_ASSET_PATH + BUTTONS_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("playerBuffer", LOCAL_ASSET_PATH + PLAYER_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("enemyBuffer", LOCAL_ASSET_PATH + ENEMY_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("bulletsBuffer", LOCAL_ASSET_PATH + BULLETS_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("orbSheetBuffer", LOCAL_ASSET_PATH + ORB_SHEET_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("textBoxBuffer", LOCAL_ASSET_PATH + TEXTBOX_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("playerParticleBuffer", LOCAL_ASSET_PATH + PLAYER_PARTICLES_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("worldBuffer", LOCAL_ASSET_PATH + WORLD_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("logoBuffer", LOCAL_ASSET_PATH + LOGO_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("westerner1Buffer", LOCAL_ASSET_PATH + WESTERNER_SPRITE_SHEET1_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("westerner2Buffer", LOCAL_ASSET_PATH + WESTERNER_SPRITE_SHEET2_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("pauseIconBuffer", LOCAL_ASSET_PATH + PAUSE_ICON_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("pauseBackgroundBuffer", LOCAL_ASSET_PATH + PAUSE_BG_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("textBackgroundBuffer", LOCAL_ASSET_PATH + TEXT_BG_FILENAME);
	mpBufferManager->createAndAddGraphicsBuffer("talkIconBuffer", LOCAL_ASSET_PATH + TALK_ICON_BUFFER);
}

void Game::loadAnimations()
{
	//Creates animation data for flyweight pattern
	AnimationData* personIdleAniData = new AnimationData(mpBufferManager->getGraphicsBuffer("playerBuffer"), 80, 125, 1, 2, 0, 1);
	AnimationData* westerner1AniData = new AnimationData(mpBufferManager->getGraphicsBuffer("westerner1Buffer"), 100, 125, 1, 2, 0, 1);
	AnimationData* westerner2AniData = new AnimationData(mpBufferManager->getGraphicsBuffer("westerner2Buffer"), 80, 125, 1, 2, 0, 1);
	AnimationData* enemyIdleAniData = new AnimationData(mpBufferManager->getGraphicsBuffer("enemyBuffer"), 106, 125, 1, 2, 0, 1);
	AnimationData* bulletAniData = new AnimationData(mpBufferManager->getGraphicsBuffer("bulletsBuffer"), 29, 5, 1, 4, 0, 3);
	AnimationData* orbAniData = new AnimationData(mpBufferManager->getGraphicsBuffer("orbSheetBuffer"), 44, 40, 1, 5, 0, 4);
	AnimationData* sparkleAniData = new AnimationData(mpBufferManager->getGraphicsBuffer("playerParticleBuffer"), 40, 40, 4, 4, 0, 15);
	AnimationData* worldAniData = new AnimationData(mpBufferManager->getGraphicsBuffer("worldBuffer"), 2400, 1440, 1, 1, 0, 0);
	
	//Adds animation data to animation manager
	mpAnimationManager->addAnimationData("personAnimation", personIdleAniData);
	mpAnimationManager->addAnimationData("enemyAnimation", enemyIdleAniData);
	mpAnimationManager->addAnimationData("westerner1Animation", westerner1AniData);
	mpAnimationManager->addAnimationData("westerner2Animation", westerner2AniData);
	mpAnimationManager->addAnimationData("bulletAnimation", bulletAniData);
	mpAnimationManager->addAnimationData("orbAnimation", orbAniData);
	mpAnimationManager->addAnimationData("sparkleAnimation", sparkleAniData);
	mpAnimationManager->addAnimationData("worldAnimation", worldAniData); //I know this is not very optimized since there is just one sprite in the animation, but I thought it would be fine since we don't have too much time left
}

void Game::loadSounds()
{
	const string ASSET_PATH = "..\\..\\shared\\assets\\";
	const string LOCAL_ASSET_PATH = "..\\..\\GameArchFinal\\assets\\";
	const string EXPLOSION_SOUND = "minetrap\\EXPLO1.wav";
	const string CLICK_SOUND = "minetrap\\SHOVEL.wav";
	const string BATTLE_MUSIC = "battleMusic.wav";
	const string WORLD_MUSIC = "worldMusic.wav";
	const string TITLE_MUSIC = "titleMusic.wav";
	const string START_SOUND = "minetrap\\NEWPLAY.wav";
	const string PAUSE_SOUND = "minetrap\\BUMP1.wav";
	const string UNPAUSE_SOUND = "minetrap\\BUMP3.wav";
	const string GUNSHOT_SOUND = "gunshotSound.wav";
	const string ZAP_SOUND = "zap.wav";
	const string TELEPORT_SOUND = "teleport.wav";
	const string HEAL_SOUND = "healSound.wav";
	const string CHARGE_SOUND = "charge.wav";

	//Loads and adds sound to sound manager
	mpSoundManager->loadAndAddSound("titleMusic", LOCAL_ASSET_PATH + TITLE_MUSIC);
	mpSoundManager->loadAndAddSound("worldMusic", LOCAL_ASSET_PATH + WORLD_MUSIC);
	mpSoundManager->loadAndAddSound("battleMusic", LOCAL_ASSET_PATH + BATTLE_MUSIC);
	mpSoundManager->loadAndAddSound("startSound", ASSET_PATH + START_SOUND);
	mpSoundManager->loadAndAddSound("pauseSound", ASSET_PATH + PAUSE_SOUND);
	mpSoundManager->loadAndAddSound("unpauseSound", ASSET_PATH + UNPAUSE_SOUND);
	mpSoundManager->loadAndAddSound("gunshotSound", LOCAL_ASSET_PATH + GUNSHOT_SOUND);
	mpSoundManager->loadAndAddSound("zapSound", LOCAL_ASSET_PATH + ZAP_SOUND);
	mpSoundManager->loadAndAddSound("teleportSound", LOCAL_ASSET_PATH + TELEPORT_SOUND);
	mpSoundManager->loadAndAddSound("healSound", LOCAL_ASSET_PATH + HEAL_SOUND);
	mpSoundManager->loadAndAddSound("chargeSound", LOCAL_ASSET_PATH + CHARGE_SOUND);
}

void Game::loadFont()
{
	//Loads and creates game font
	const string FONT_FILE = "..\\..\\shared\\assets\\cour.ttf";
	const int FONT_SIZE = 30;

	mpFont = new Font(FONT_FILE, FONT_SIZE);
}

void Game::loadDataFromFile()
{
	ifstream file;
	file.open("..\\..\\GameArchFinal\\assets\\gameValues.txt");
	string data;

	if (file.is_open())
	{
		//Gets data from game values file from any location in the file (It's probobly not the most efficient code)
		while (!file.eof())
		{
			getline(file, data, ' ');

			auto iterator = data.find("Good_Points_Sound:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mpSoundManager->loadAndAddSound("goodSound", data);
			}

			iterator = data.find("Bad_Points_Sound:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mpSoundManager->loadAndAddSound("badSound", data);
			}

			iterator = data.find("Player_Health:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mPlayerDataMap["Player Health"] = stoi(data);
			}

			iterator = data.find("Player_Speed:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mPlayerDataMap["Player Speed"] = stoi(data);
			}

			iterator = data.find("Bullet_Speed_Easy:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mBulletSpeedEasy = stoi(data);
			}

			iterator = data.find("Bullet_Speed_Medium:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mBulletSpeedMedium = stoi(data);
			}

			iterator = data.find("Bullet_Speed_Hard:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mBulletSpeedHard = stoi(data);
			}

			iterator = data.find("Relocate_Battle_Units_Speed:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mRelocateBattleUnitsSpeed = stoi(data);
			}

			iterator = data.find("Player_Bullets_Per_Turn:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mPlayerDataMap["Player Bullets Per Turn"] = stoi(data);
			}

			iterator = data.find("Enemy_Bullets_Per_Turn:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyDataMap["Enemy Bullets Per Turn"] = stoi(data);
			}

			iterator = data.find("Enemy_Health_Easy:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyDataMap["Enemy Health Easy"] = stoi(data);
			}

			iterator = data.find("Enemy_Health_Medium:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyDataMap["Enemy Health Medium"] = stoi(data);
			}

			iterator = data.find("Enemy_Health_Hard:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyDataMap["Enemy Health Hard"] = stoi(data);
			}

			iterator = data.find("Enemy_Shoot_Chance_Per_Frame:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyDataMap["Enemy Shoot Chance Per Frame"] = stoi(data);
			}

			iterator = data.find("Enemy_Distence_From_Player:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyDataMap["Enemy Distence From Player"] = stoi(data);
			}

			iterator = data.find("Enemy_Distence_From_Other_Enemies:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyDataMap["Enemy Distence From Other Enemies"] = stoi(data);
			}

			iterator = data.find("Enemy_Speed:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mEnemyDataMap["Enemy Speed"] = stoi(data);
			}

			iterator = data.find("Player_Dash_Distence");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mPlayerDataMap["Player Dash Distence"] = stoi(data);
			}

			iterator = data.find("Number_Of_Player_Particles:");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mNumberOfPlayerParticles = stoi(data);
			}

			iterator = data.find("Number_Of_Teleport_Particles");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mPlayerDataMap["Number Of Teleport Particles"] = stoi(data);
			}

			iterator = data.find("Teleport_Particles_Speed");
			if (iterator != string::npos)
			{
				getline(file, data, '\n');
				mPlayerDataMap["Teleport Particles Speed"] = stoi(data);
			}
		}

		file.close();
	}
	else
	{
		cout << "Error opening gameValues file" << endl;
	}

	file;
	file.open("..\\..\\GameArchFinal\\assets\\textHolder.txt");
	data = "";

	//Gets text from text holder file for buttons and ui text and adds it to the text manager
	if (file.is_open())
	{
		while (!file.eof())
		{
			getline(file, data, '\n');

			auto iterator = data.find("Button:");
			if (iterator != string::npos)
			{
				string textKey = data.substr(0, data.length() - 1);
				vector<string> text;

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Bullets Remaining Text:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Sound Settings:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Difficulty Settings:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Language Settings:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Game Over Text:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Win Game Text:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Intro Text English:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Intro Text Spanish:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}
			iterator = data.find("Intro Text French:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Pause Text:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Player Turn Text:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Player Win Text:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Health Display Text:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}

			iterator = data.find("Talk Text:");
			if (iterator != string::npos)
			{
				vector<string> text;
				string textKey = data.substr(0, data.length() - 1);

				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);
				getline(file, data, '\n');
				text.push_back(data);

				mpTextManager->addText(textKey, text);
			}
		}
	}
	else
	{
		cout << "Error opening buttonTextHolder file" << endl;
	}
}

void Game::loadUIMenus()
{
	//Loads all UI menus and adds ui buttons and ui sprites

	mpMenuManager->createAndAddNewMenu(UIMenu::BATTLE_MENU);
	mpMenuManager->setCurrMenu(UIMenu::BATTLE_MENU);
	Sprite* buttonSprite = new Sprite(mpBufferManager->getGraphicsBuffer("buttonsBuffer"), Vector2D(0, 0), 200, 85);
	Sprite* hoverSprite = new Sprite(mpBufferManager->getGraphicsBuffer("buttonsBuffer"), Vector2D(200, 0), 200, 85);
	mpMenuManager->addUISpriteToCurrMenu(Sprite(mpBufferManager->getGraphicsBuffer("textBoxBuffer"), Vector2D(0, 0), 650, 150, Vector2D((mpGraphicsSystem->getBackBuffer()->getDimentions().w - 650) / 2.0f, 400.0f)), "textBox");

	vector<string> buttonTextList = mpTextManager->getText("Attack Button");
	vector<string> buttonAltTextList;
	mpMenuManager->addButtonToCurrMenu(Button::ATTACK_BUTTON, Vector2D(100, 420), buttonSprite, hoverSprite, mpFont, "goodSound", buttonTextList, {}, Vector2D(0.8f, 0.8f));
	buttonTextList = mpTextManager->getText("Reload Button");
	mpMenuManager->addButtonToCurrMenu(Button::RELOAD_BUTTON, Vector2D(300, 420), buttonSprite, hoverSprite, mpFont, "goodSound", buttonTextList, {}, Vector2D(0.8f, 0.8f));
	buttonTextList = mpTextManager->getText("Heal Button");
	mpMenuManager->addButtonToCurrMenu(Button::HEAL_BUTTON, Vector2D(500, 420), buttonSprite, hoverSprite, mpFont, "goodSound", buttonTextList, {}, Vector2D(0.8f, 0.8f));
	
	mpMenuManager->createAndAddNewMenu(UIMenu::TITLE_MENU);
	mpMenuManager->setCurrMenu(UIMenu::TITLE_MENU);
	buttonTextList = mpTextManager->getText("Start Button");
	Vector2D buttonPos(Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 3.0f));
	mpMenuManager->addButtonToCurrMenu(Button::START_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("Load Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 2.0f));
	mpMenuManager->addButtonToCurrMenu(Button::LOAD_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("Options Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.5f));
	mpMenuManager->addButtonToCurrMenu(Button::OPTIONS_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("Quit Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.2f));
	mpMenuManager->addButtonToCurrMenu(Button::QUIT_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList, {}, Vector2D(0.8f, 0.8f));
	GraphicsBuffer* logoBuffer = mpBufferManager->getGraphicsBuffer("logoBuffer");
	mpMenuManager->addUISpriteToCurrMenu(Sprite(logoBuffer, Vector2D(0, 0), logoBuffer->getDimentions().w, logoBuffer->getDimentions().h, Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - logoBuffer->getDimentions().w / 2.0f, 20.0f)), "logo");


	mpMenuManager->createAndAddNewMenu(UIMenu::OPTIONS_MENU);
	mpMenuManager->setCurrMenu(UIMenu::OPTIONS_MENU);
	buttonTextList = mpTextManager->getText("Sound Toggle Button");
	buttonAltTextList = mpTextManager->getText("Alt Sound Toggle Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 8.0f));
	mpMenuManager->addButtonToCurrMenu(Button::TOGGLE_SOUNDS_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList, buttonAltTextList);
	buttonTextList = mpTextManager->getText("English Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 5.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.5f));
	mpMenuManager->addButtonToCurrMenu(Button::ENGLISH_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("Spanish Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.5f));
	mpMenuManager->addButtonToCurrMenu(Button::SPANISH_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("French Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 1.25f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.5f));
	mpMenuManager->addButtonToCurrMenu(Button::FRENCH_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("Easy Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 5.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 2.6f));
	mpMenuManager->addButtonToCurrMenu(Button::EASY_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("Medium Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 2.6f));
	mpMenuManager->addButtonToCurrMenu(Button::MEDIUM_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("Hard Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 1.25f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 2.6f));
	mpMenuManager->addButtonToCurrMenu(Button::HARD_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
	buttonTextList = mpTextManager->getText("Back Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.2f));
	mpMenuManager->addButtonToCurrMenu(Button::TO_TITLE_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);

	mpMenuManager->createAndAddNewMenu(UIMenu::GAME_OVER_MENU);
	mpMenuManager->setCurrMenu(UIMenu::GAME_OVER_MENU);
	buttonTextList = mpTextManager->getText("Retry Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 2.0f));
	mpMenuManager->addButtonToCurrMenu(Button::RETRY_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "goodSound", buttonTextList);

	mpMenuManager->createAndAddNewMenu(UIMenu::PAUSE_MENU);
	mpMenuManager->setCurrMenu(UIMenu::PAUSE_MENU);	
	mpMenuManager->addUISpriteToCurrMenu(Sprite(mpBufferManager->getGraphicsBuffer("pauseIconBuffer"), Vector2D(0, 0), 270, 310, Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - 270 / 2.0f, 70.0f)), "pauseIcon");
	buttonTextList = mpTextManager->getText("Resume Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.6f));
	mpMenuManager->addButtonToCurrMenu(Button::PAUSE_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "", buttonTextList);
	buttonTextList = mpTextManager->getText("Main Menu Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.25f));
	mpMenuManager->addButtonToCurrMenu(Button::TO_TITLE_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);

	mpMenuManager->createAndAddNewMenu(UIMenu::WIN_GAME_MENU);
	mpMenuManager->setCurrMenu(UIMenu::WIN_GAME_MENU);
	buttonTextList = mpTextManager->getText("Main Menu Button");
	buttonPos = (Vector2D(mpGraphicsSystem->getBackBuffer()->getDimentions().w / 2.0f - hoverSprite->getWidth() / 2.0f, mpGraphicsSystem->getBackBuffer()->getDimentions().h / 1.5f));
	mpMenuManager->addButtonToCurrMenu(Button::TO_TITLE_BUTTON, buttonPos, buttonSprite, hoverSprite, mpFont, "startSound", buttonTextList);
}

void Game::loadNPCDiologue()
{
	int currNPC;
	ifstream file;
	file.open("..\\..\\GameArchFinal\\assets\\NPCDiologue.txt");
	string data;

	if (file.is_open())
	{
		//Gets diologue data from diologue file
		while (!file.eof())
		{
			getline(file, data, '\n');

			auto iterator = data.find("NPC Diologue English:");
			if (iterator != string::npos)
			{
				currNPC = 0;
				getline(file, data, '\n');
				getline(file, data, '\n');
				while (data == "Diologue:")
				{
					mNPCDiologueEnglish.push_back({});
					getline(file, data, '\n');
					while (data != "" && !file.eof())
					{
						mNPCDiologueEnglish[currNPC].push_back(data);
						getline(file, data, '\n');
					}

					currNPC++;
					getline(file, data, '\n');
				}
			}

			iterator = data.find("NPC Enemy Diologue English:");
			if (iterator != string::npos)
			{
				currNPC = 0;
				getline(file, data, '\n');
				getline(file, data, '\n');
				while (data == "Diologue:")
				{
					mNPCEnemyDiologueEnglish.push_back({});
					getline(file, data, '\n');
					while (data != "" && !file.eof())
					{
						mNPCEnemyDiologueEnglish[currNPC].push_back(data);
						getline(file, data, '\n');
					}

					currNPC++;
					getline(file, data, '\n');
				}
			}

			iterator = data.find("NPC Diologue Spanish:");
			if (iterator != string::npos)
			{
				currNPC = 0;
				getline(file, data, '\n');
				getline(file, data, '\n');
				while (data == "Diologue:")
				{
					mNPCDiologueSpanish.push_back({});
					getline(file, data, '\n');
					while (data != "" && !file.eof())
					{
						mNPCDiologueSpanish[currNPC].push_back(data);
						getline(file, data, '\n');
					}

					currNPC++;
					getline(file, data, '\n');
				}
			}

			iterator = data.find("NPC Enemy Diologue Spanish:");
			if (iterator != string::npos)
			{
				currNPC = 0;
				getline(file, data, '\n');
				getline(file, data, '\n');
				while (data == "Diologue:")
				{
					mNPCEnemyDiologueSpanish.push_back({});
					getline(file, data, '\n');
					while (data != "" && !file.eof())
					{
						mNPCEnemyDiologueSpanish[currNPC].push_back(data);
						getline(file, data, '\n');
					}

					currNPC++;
					getline(file, data, '\n');
				}
			}

			iterator = data.find("NPC Diologue French:");
			if (iterator != string::npos)
			{
				currNPC = 0;
				getline(file, data, '\n');
				getline(file, data, '\n');
				while (data == "Diologue:")
				{
					mNPCDiologueFrench.push_back({});
					getline(file, data, '\n');
					while (data != "" && !file.eof())
					{
						mNPCDiologueFrench[currNPC].push_back(data);
						getline(file, data, '\n');
					}

					currNPC++;
					getline(file, data, '\n');
				}
			}

			iterator = data.find("NPC Enemy Diologue French:");
			if (iterator != string::npos)
			{
				currNPC = 0;
				getline(file, data, '\n');
				getline(file, data, '\n');
				while (data == "Diologue:")
				{
					mNPCEnemyDiologueFrench.push_back({});
					getline(file, data, '\n');
					while (data != "" && !file.eof())
					{
						mNPCEnemyDiologueFrench[currNPC].push_back(data);
						getline(file, data, '\n');
					}

					currNPC++;
					getline(file, data, '\n');
				}
			}
		}

		file.close();
	}
	else
	{
		cout << "Error opening gameValues file" << endl;
	}
}

void Game::addEventListeners()
{
	EventSystem* theEventSystem = EventSystem::getInstance();
	theEventSystem->addListener((EventType)GameEvent::QUIT_GAME, this);
	theEventSystem->addListener((EventType)GameEvent::START_GAME, this);
	theEventSystem->addListener((EventType)GameEvent::LOAD_GAME, this);
	theEventSystem->addListener((EventType)GameEvent::GO_TO_TITLE_SCREEN, this);
	theEventSystem->addListener((EventType)GameEvent::GO_TO_OPTIONS, this);
	theEventSystem->addListener((EventType)GameEvent::CLICK_BUTTON, this);
	theEventSystem->addListener((EventType)GameEvent::PAUSE, this);
	theEventSystem->addListener((EventType)GameEvent::UNPAUSE, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_BATTLE, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_PLAYER_TURN, this);
	theEventSystem->addListener((EventType)GameEvent::BEGIN_ENEMY_TURN, this);
	theEventSystem->addListener((EventType)GameEvent::ENTER_SHOOTING_MODE, this);
	theEventSystem->addListener((EventType)GameEvent::FIRE_GUN, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_DIES, this);
	theEventSystem->addListener((EventType)GameEvent::RESTART_BATTLE, this);
	theEventSystem->addListener((EventType)GameEvent::CHANGE_LANGUAGE, this);
	theEventSystem->addListener((EventType)GameEvent::PLAYER_DASH, this);
}

void Game::createPlayer()
{
	//Creates the player puts it in the unit manager
	vector<Animation> playerAnimations;
	playerAnimations.push_back(Animation(mpAnimationManager->getAnimationData("personAnimation"), true));
	mpUnitManager->createPlayer(new Player(playerAnimations, Animation::SLOW, 0, Vector2D(100, 1000)));
}

void Game::createWorld()
{
	Vector2D worldPos(0, 0);
	Vector2D playerWorldRelationPos(0, 0);

	//This calculates the world's starting position in relation to the player's if the player exists
	if (getPlayer() != nullptr)
	{
		Vector2D windowDimensions = getDisplayDimensions();
		playerWorldRelationPos = worldPos - getPlayer()->getPosition() + windowDimensions / 2;
	}

	//Creates world in the unit manager
	vector<Animation> animations = { Animation(mpAnimationManager->getAnimationData("worldAnimation"), true) };
	mpUnitManager->createWorld(animations, playerWorldRelationPos, 0);
}

void Game::createEnemy()
{
	//Creates enemy and adds it to the unit manager
	vector<Animation> enemyAnimations;
	enemyAnimations.push_back(Animation(mpAnimationManager->getAnimationData("enemyAnimation"), true));
	mpUnitManager->addEnemy(new Enemy(enemyAnimations, Animation::SLOW));
}

void Game::createBullet(Vector2D spawnPos, Vector2D shootAtPos, bool shotByPlayer)
{
	//Gives bullet a bullet animation if shot by the enemy and gives it an orb animation if shot by the player
	vector<Animation> bulletAnimations;
	if (!shotByPlayer)
	{
		bulletAnimations.push_back(Animation(mpAnimationManager->getAnimationData("bulletAnimation"), true));
	}
	else
	{
		bulletAnimations.push_back(Animation(mpAnimationManager->getAnimationData("orbAnimation"), true));
	}

	//Creates bullet
	Bullet* currBullet = new Bullet(bulletAnimations, spawnPos - Vector2D(bulletAnimations[0].getCurrentSprite()->getWidth() / 2.0f, bulletAnimations[0].getCurrentSprite()->getHeight() / 2.0f), (float)mCurrBulletSpeed, Animation::FAST, shotByPlayer);

	//Sets the direction the bullet moves in
	Vector2D directionVelocityVector(shootAtPos - spawnPos);
	directionVelocityVector.normalize();
	currBullet->setVelocity(directionVelocityVector);

	//Adds bullet to the unit manager
	mpUnitManager->addBullet(currBullet);
}

vector<string> Game::getSplitText(string text, float textWidth)
{
	//Splits string into a vector of strings that are under the length specified
	vector<string> splitText = {};
	float scaleOffset = 1.4f; //Scales font size
	unsigned int numberOfCharsASlice = (unsigned int)(textWidth / mpFont->getSize() * scaleOffset);
	string wordToAddToString = "";
	while (text != "" || wordToAddToString != " ")
	{
		string splitString = "";

		while (splitString.length() + wordToAddToString.length() <= numberOfCharsASlice)
		{
			splitString += wordToAddToString;

			string splitter = " ";
			int stringPos = text.find(splitter);
			wordToAddToString = text.substr(0, stringPos) + " ";

			if (stringPos == string::npos)
			{
				splitter = "";
			}

			text.erase(0, stringPos + splitter.length());
		}

		splitText.push_back(splitString);
	}

	return splitText;
}

void Game::winBattle()
{
	//Deletes enemies
	mpUnitManager->clearEnemies();

	//Returns to overworld if game not finished
	if (!getWorld()->getAllEnemeisDefeated())
	{
		mpMenuManager->setCurrMenu(UIMenu::NO_MENU);
		mpUnitManager->getWorld()->removeNextBaracade();
		mCurrScreen = OVERWORLD_SCREEN;
		EventSystem::getInstance()->fireEvent((EventType)GameEvent::GO_TO_OVERWORLD);
		mInBattle = false;
		getPlayer()->setCanShoot(false);
		getPlayer()->setCanDash(false);
		getPlayer()->setPosition(mOverworldPlayerPos);
	}
	else //Ends game if player defeated all enemies
	{
		mpCurrBackgroundBuffer = mpBufferManager->getGraphicsBuffer("winBackgroundBuffer");
		mpMenuManager->setCurrMenu(UIMenu::WIN_GAME_MENU);
		mCurrScreen = WIN_GAME_SCREEN;
	}
}

void Game::saveGame()
{
	const string SAVE_FILE_PATH = "..\\..\\GameArchFinal\\assets\\saveGame.txt";

	ofstream saveFile;
	saveFile.open(SAVE_FILE_PATH);

	//Saves necessary game values to savefile
	if (saveFile.is_open())
	{
		saveFile << "Game_Data:\n";
		saveFile << "Game_Screen: " << mCurrScreen << "\n";
		saveFile << "Game_Paused: " << mGamePaused << "\n";
		saveFile << "Game_Background: " << mpBufferManager->getGraphicsBufferKey(mpCurrBackgroundBuffer) << "\n";
		saveFile << "In_Battle: " << mInBattle << "\n";
		saveFile << "Battle_State: " << mBattleState << "\n";
		saveFile << "Language: " << mCurrLanguage << "\n";
		saveFile << "Difficulty: " << mCurrDifficulty << "\n";
		//saveFile << "Sound On: " << mpSoundManager->getSoundOn() << "\n"; This is commented out because I think it is better that you can close the game, turn off or on the sound, and then load the game with that new setting (So that it doesn't always load the setting it was before in case you change your mind)
		saveFile << "Bullet Speed: " << mCurrBulletSpeed << "\n";
		saveFile << "Prev Menu: " << mPrevMenu << "\n";
		saveFile << "Player Overworld Pos: " << mOverworldPlayerPos.getX() << " " << mOverworldPlayerPos.getY() << "\n";

		saveFile << mpMenuManager->getSaveData();
		saveFile << mpUnitManager->getUnitSaveData();

		saveFile.close();
	}
	else
	{
		cout << "Error opening saveFile file" << endl;
	}
}

void Game::loadGame()
{
	const string SAVE_FILE_PATH = "..\\..\\GameArchFinal\\assets\\saveGame.txt";

	ifstream saveFile;
	saveFile.open(SAVE_FILE_PATH);
	string data;

	//Loads save data by converting file into a string to pass parts of the data as string perameters to functions and have more slicing ability with the data
	if (saveFile.is_open())
	{
		ostringstream fileData;
		fileData << saveFile.rdbuf();
		data = fileData.str();

		saveFile.close();

		string splitter;
		string extractedData;
		int stringPos;
		while (data.length() > 0)
		{
			splitter = '\n';
			stringPos = data.find(splitter);
			string subStrData = data.substr(0, stringPos);
			data.erase(0, stringPos + splitter.length());

			if (subStrData.find("Game_Data:") != string::npos)
			{
				extractedData = extractDataFromDataString("Game_Screen: ", data);
				if (extractedData != "")
				{
					mCurrScreen = static_cast<Game::Screens>(stoi(extractedData));

					//Starts previous game music
					if (mCurrScreen == OVERWORLD_SCREEN)
					{
						EventSystem::getInstance()->fireEvent((EventType)GameEvent::GO_TO_OVERWORLD);
					}
					else if (mCurrScreen == TITLE_SCREEN || mCurrScreen == OPTIONS_SCREEN)
					{
						EventSystem::getInstance()->fireEvent(PlaySoundEvent("titleMusic"));
					}
				}

				extractedData = extractDataFromDataString("Game_Paused: ", data);
				if (extractedData != "")
				{
					mGamePaused = static_cast<bool>(stoi(extractedData));
				}

				extractedData = extractDataFromDataString("Game_Background: ", data);
				if (extractedData != "")
				{
					mpCurrBackgroundBuffer = mpBufferManager->getGraphicsBuffer(extractedData);
				}

				extractedData = extractDataFromDataString("In_Battle: ", data);
				if (extractedData != "")
				{
					mInBattle = static_cast<bool>(stoi(extractedData));

					//Starts previous music
					if (mInBattle)
					{
						EventSystem::getInstance()->fireEvent(PlaySoundEvent("battleMusic"));
					}
				}

				extractedData = extractDataFromDataString("Battle_State: ", data);
				if (extractedData != "")
				{
					mBattleState = static_cast<BattleState>(stoi(extractedData));
				}

				extractedData = extractDataFromDataString("Language: ", data);
				if (extractedData != "")
				{
					EventSystem::getInstance()->fireEvent(ChangeLanguageEvent(static_cast<Button::LanguageOptions>(stoi(extractedData))));
				}

				extractedData = extractDataFromDataString("Difficulty: ", data);
				if (extractedData != "")
				{
					mCurrDifficulty = static_cast<Difficulty>(stoi(extractedData));
				}

				//This is commented out for the same reason on line #1034
				/*extractedData = extractDataFromDataString("Sound On: ", data);
				if (extractedData != "")
				{
					bool soundOn = static_cast<bool>(stoi(extractedData));

					if (!soundOn)
					{
						EventSystem::getInstance()->fireEvent((EventType)GameEvent::TOGGLE_SOUND);
					}
				}*/

				extractedData = extractDataFromDataString("Bullet Speed: ", data);
				if (extractedData != "")
				{
					mCurrBulletSpeed = stoi(extractedData);
				}

				extractedData = extractDataFromDataString("Prev Menu: ", data);
				if (extractedData != "")
				{
					mPrevMenu = static_cast<UIMenu::MenuType>(stoi(extractedData));
				}

				extractedData = extractDataFromDataString("Player Overworld Pos: ", data);
				if (extractedData != "")
				{
					splitter = " ";
					int stringPos = extractedData.find(splitter);
					string playerXPos = extractedData.substr(0, stringPos);
					extractedData.erase(0, stringPos + splitter.length());

					mOverworldPlayerPos.setX((float)stoi(playerXPos));
					mOverworldPlayerPos.setY((float)stoi(extractedData));
				}
			}
			else if (subStrData.find("UI_Data:") != string::npos)
			{
				splitter = "\n\n\n";
				stringPos = data.find(splitter);
				subStrData = data.substr(0, stringPos);
				data.erase(0, stringPos + splitter.length());
				mpMenuManager->loadData(subStrData);
			}
			else if (subStrData.find("Player_Data:") != string::npos)
			{
				splitter = "\n\n";
				stringPos = data.find(splitter);
				subStrData = data.substr(0, stringPos);
				data.erase(0, stringPos + splitter.length());
				mpUnitManager->loadUnitDataFromFile("Player_Data:\n" + subStrData + "\n\n");
			}
			else if (subStrData.find("Enemy_Data:") != string::npos)
			{
				splitter = "\n\n\n";
				stringPos = data.find(splitter);
				subStrData = data.substr(0, stringPos);
				data.erase(0, stringPos + splitter.length());
				mpUnitManager->loadUnitDataFromFile("Enemy_Data:\n" + subStrData + "\n\n");
			}
			else if (subStrData.find("World_Data:") != string::npos)
			{
				splitter = "\n\n\n";
				stringPos = data.find(splitter);
				subStrData = data.substr(0, stringPos);
				data.erase(0, stringPos + splitter.length());
				mpUnitManager->loadUnitDataFromFile("World_Data:\n" + subStrData + "\n\n");
			}
		}

		saveFile.close();
	}
	else
	{
		cout << "Error opening saveFile file" << endl;
	}
}

string Game::extractDataFromDataString(string dataToFindSplitter, string& fileData)
{
	//Helper function to extract data from a line
	int stringPos = fileData.find(dataToFindSplitter);
	if (stringPos != string::npos)
	{
		fileData.erase(0, stringPos + dataToFindSplitter.length());

		string splitter;
		splitter = '\n';
		stringPos = fileData.find(splitter);

		string data = fileData.substr(0, stringPos);

		fileData.erase(0, stringPos + splitter.length());
		return data;
	}
	
	return "";
}

void Game::resetAndStartGame()
{
	//Resets units, particles, messages, game states, intro text, and ui menu
	mpUnitManager->resetUnits();
	mpPlayerParticlePool->resetParticles();
	mpMenuManager->clearMessageList();
	mGamePaused = false;
	mInBattle = false;

	mCurrScreen = OVERWORLD_SCREEN;
	EventSystem::getInstance()->fireEvent((EventType)GameEvent::GO_TO_OVERWORLD);
	createPlayer();
	createWorld();

	vector<string> introText;
	switch (mCurrLanguage)
	{
	case Button::ENGLISH:
		mpUnitManager->loadWorldNPCs(Animation::SLOW, mNPCDiologueEnglish, mNPCEnemyDiologueEnglish);
		introText = mpTextManager->getText("Intro Text English");
		break;
	case Button::SPANISH:
		mpUnitManager->loadWorldNPCs(Animation::SLOW, mNPCDiologueSpanish, mNPCEnemyDiologueSpanish);
		introText = mpTextManager->getText("Intro Text Spanish");
		break;
	case Button::FRENCH:
		mpUnitManager->loadWorldNPCs(Animation::SLOW, mNPCDiologueFrench, mNPCEnemyDiologueFrench);
		introText = mpTextManager->getText("Intro Text French");
		break;
	}

	EventSystem::getInstance()->fireEvent((EventType)GameEvent::LOCK_PLAYER_MOVEMENT);
	for (string str : introText)
	{
		EventSystem::getInstance()->fireEvent(AddToMessageListEvent(str));
	}

	getPlayer()->setCanDash(false);
	mpMenuManager->setCurrMenu(UIMenu::NO_MENU);
}

//Handles events
void Game::handleEvent(const Event& theEvent)
{
	EventType theType = theEvent.getType();
	if (theType == GameEvent::QUIT_GAME && (mGamePaused || mCurrScreen != GAME_SCREEN)) //Quits game if not on game screen or is paused on game screen
	{
		saveGame();
		mRunning = false;
	}
	else if (theType == GameEvent::LOAD_GAME)
	{
		//Resets necessary values and loads game
		mpUnitManager->resetUnits();
		mpPlayerParticlePool->resetParticles();
		mpMenuManager->clearMessageList();
		mGamePaused = false;
		loadGame();
	}
	else if (theType == GameEvent::START_GAME) 
	{
		resetAndStartGame();
	}
	else if (theType == GameEvent::GO_TO_TITLE_SCREEN) 
	{
		//Sets initial screen and background to title screen
		mGamePaused = false;
		mCurrScreen = TITLE_SCREEN;
		mpCurrBackgroundBuffer = mpBufferManager->getGraphicsBuffer("titleBackgroundBuffer");
		mpMenuManager->setCurrMenu(UIMenu::TITLE_MENU);
	}
	else if (theType == GameEvent::GO_TO_OPTIONS)
	{
		mCurrScreen = OPTIONS_SCREEN;
		mpMenuManager->setCurrMenu(UIMenu::OPTIONS_MENU);
	}
	else if (theType == GameEvent::PAUSE)
	{
		//Pauses game and all units
		mGamePaused = true;

		if (!mpUnitManager->getAllUnitsAnimationsPaused())
		{
			mpUnitManager->toggleAllUnitsAnimationsPaused();
		}

		mPrevMenu = mpMenuManager->getCurrMenu();
		mpMenuManager->setCurrMenu(UIMenu::PAUSE_MENU);
	}
	else if (theType == GameEvent::UNPAUSE) 
	{

		//Unpauses game and all units
		mGamePaused = false;

		if (mpUnitManager->getAllUnitsAnimationsPaused()) 
		{
			mpUnitManager->toggleAllUnitsAnimationsPaused();
		}

		mpMenuManager->setCurrMenu(mPrevMenu);
	}
	else if (theType == GameEvent::BEGIN_BATTLE)
	{
		//Switches to game screen and background and resets the game
		mCurrScreen = GAME_SCREEN;
		mpCurrBackgroundBuffer = mpBufferManager->getGraphicsBuffer("battleBackgroundBuffer");

		//This is to only save the overworld pos when the user first gets in a battle (not when retrying)
		if (!mInBattle)
		{
			mOverworldPlayerPos = getPlayer()->getPosition();
		}

		getPlayer()->setCanDash(true);
		mpUnitManager->resetBattle();
		mInBattle = true;
		mpUnitManager->relocateBattleUnits(1000); //Instantly relocates battle units to proper positions
		mpUnitManager->setStartingPlayerHealth();
		EventSystem::getInstance()->fireEvent((EventType)GameEvent::RELOCATE_BATTLING_UNITS);
		mBattleState = RELOCATING_UNITS;
	}
	else if (theType == GameEvent::BEGIN_PLAYER_TURN)
	{
		EventSystem::getInstance()->fireEvent(AddToMessageListEvent(mCurrPlayerTurnText));
		mBattleState = PLAYER_TURN;
	}
	else if (theType == GameEvent::BEGIN_ENEMY_TURN)
	{
		mBattleState = ENEMY_TURN;
	}
	else if (theType == GameEvent::ENTER_SHOOTING_MODE)
	{
		mBattleState = SHOOTING_MODE;
	}
	else if (theType == GameEvent::FIRE_GUN)
	{
		const FireGunEvent& fireGunEvent = static_cast<const FireGunEvent&>(theEvent);

		createBullet(fireGunEvent.getShotFromPoint(), fireGunEvent.getShootAtPoint(), fireGunEvent.getShotByPlayer());
	}
	else if (theType == GameEvent::PLAYER_DIES)
	{
		mpMenuManager->setCurrMenu(UIMenu::GAME_OVER_MENU);
		mCurrScreen = GAME_OVER_SCEEN;
		mpMenuManager->clearMessageList(); //Just in case the player dies after all the battle enemies die 
	}
	else if (theType == GameEvent::RESTART_BATTLE)
	{
		mpUnitManager->resetBattle();
		mCurrScreen = GAME_SCREEN;
		EventSystem::getInstance()->fireEvent((EventType)GameEvent::BEGIN_BATTLE);
	}
	else if (theType == GameEvent::CHANGE_LANGUAGE)
	{
		const ChangeLanguageEvent& changeLanguageEvent = static_cast<const ChangeLanguageEvent&>(theEvent);
		mCurrLanguage = changeLanguageEvent.getLanguageToChangeTo();
		switchTextLanguage(mCurrLanguage);
	}
}

int Game::getEnemyData(string dataToGet)
{
	auto iterator = mEnemyDataMap.find(dataToGet);

	if (iterator != mEnemyDataMap.end())
	{
		return iterator->second;
	}

	return 0;
}

int Game::getPlayerData(string dataToGet)
{
	auto iterator = mPlayerDataMap.find(dataToGet);

	if (iterator != mPlayerDataMap.end())
	{
		return iterator->second;
	}

	return 0;
}

void Game::cleanup()
{
	delete mpPlayerParticlePool;
	delete mpTextManager;
	delete mpAnimationManager;
	delete mpInputSystem;
	delete mpInputTranslator;
	delete mpUnitManager;
	delete mpBufferManager;
	delete mpSoundManager;
	delete mpMenuManager;
	delete mpHUD;
	delete mpFont;
	delete mpGraphicsSystem;

	mpPlayerParticlePool = nullptr;
	mpTextManager = nullptr;
	mpAnimationManager = nullptr;
	mpInputSystem = nullptr;
	mpInputTranslator = nullptr;
	mpUnitManager = nullptr;
	mpBufferManager = nullptr;
	mpSoundManager = nullptr;
	mpMenuManager = nullptr;
	mpHUD = nullptr;
	mpFont = nullptr;
	mpGraphicsSystem = nullptr;
}

void Game::update(float deltaTime)
{
	if (!mpMenuManager->getIsMessages()) //While there are no pop up messages the battle will continue
	{
		if (mInBattle)
		{
			if (!mGamePaused)
			{
				//Handles battle
				switch (mBattleState)
				{
				case PLAYER_TURN:
					break;
				case ENEMY_TURN:
					mpUnitManager->decideCurrEnemyActions();
					break;
				case SHOOTING_MODE:
					if (mpUnitManager->getNoMoreEnemies()) //Wins battle if all enemies are defeated
					{
						EventSystem::getInstance()->fireEvent(AddToMessageListEvent(mCurrPlayerWinText));
						mBattleState = WIN_BATTLE;
					}
					else if (getPlayer()->getIsPlayerEmpty() && mpUnitManager->getAllEnemiesEmpty() && mpUnitManager->getNoMoreBulletsOnScreen()) //Ends shooting mode when all battle units are empty or can't shoot
					{
						mBattleState = RELOCATING_UNITS;
						EventSystem::getInstance()->fireEvent((EventType)GameEvent::RELOCATE_BATTLING_UNITS);
					}
					mpUnitManager->haveEnemiesAttack(); //Makes enemies attack player
					break;
				case WIN_BATTLE:
					winBattle();
					break;
				case RELOCATING_UNITS:
					bool allUnitsHaveBeenRelocated = mpUnitManager->relocateBattleUnits((float)mRelocateBattleUnitsSpeed);
					if (allUnitsHaveBeenRelocated)
					{
						EventSystem::getInstance()->fireEvent((EventType)GameEvent::BEGIN_PLAYER_TURN);
					}
					break;
				}
			}

			// Displays and updates UI ammno and health counters
			mpUnitManager->displayUnitInfo(mpHUD, mpFont);
		}
	}
	else
	{
		if (mInBattle) //This is done here so that the text box is above the unit info and so that it updates on the same frame
		{
			// Displays and updates UI ammno and health counters
			mpUnitManager->displayUnitInfo(mpHUD, mpFont);
		}

		//Displays messages and text box sprite
		GraphicsBuffer* textboxBuff = mpBufferManager->getGraphicsBuffer("textBoxBuffer");
		SDL_Rect textboxRect = textboxBuff->getDimentions();
		mpHUD->draw(Vector2D((mpGraphicsSystem->getBackBuffer()->getDimentions().w - 650) / 2.0f, 400.0f), Sprite(textboxBuff, Vector2D(textboxRect.x, textboxRect.y), textboxRect.w, textboxRect.h));

		vector<string> splitText = getSplitText(mpMenuManager->getCurrMessage(), (float)textboxRect.w);
		for (unsigned int i = 0; i < splitText.size(); i++)
		{
			mpHUD->writeHUDText(Vector2D(120, 415 + (i * mpFont->getSize())), *mpFont, Color(1.0f, 1.0f, 1.0f, 1.0f), splitText[i]);
		}
	}

	//Pauses all units and particles while game is paused
	if (!mGamePaused)
	{
		//Updates player particles
		mpPlayerParticlePool->update(mFPS);

		//Updates units
		mpUnitManager->update(deltaTime);
	}
}

void Game::render()
{
	//Renders background
	GraphicsBuffer* backBuffer = mpGraphicsSystem->getBackBuffer();
	backBuffer->draw(Vector2D(0, 0), *mpCurrBackgroundBuffer);


	if (mCurrScreen == OVERWORLD_SCREEN)
	{
		//Renders world
		mpUnitManager->drawWorld(backBuffer, mpHUD->getBuffer());
	}
	else if (mCurrScreen == OPTIONS_SCREEN) //Renders option screen sprite ui and text
	{
		mpHUD->draw(Vector2D(getDisplayDimensions().getX() / 2.0f - mTextBGSprite.getWidth() / 2.0f, 15.0f), mTextBGSprite);
		mpHUD->draw(Vector2D(getDisplayDimensions().getX() / 2.0f - mTextBGSprite.getWidth() / 2.0f, 175.0f), mTextBGSprite);
		mpHUD->draw(Vector2D(getDisplayDimensions().getX() / 2.0f - mTextBGSprite.getWidth() / 2.0f, 335.0f), mTextBGSprite);
		mpHUD->writeHUDText(Vector2D(getDisplayDimensions().getX() / 2.0f, 40.0f), *mpFont, Color(1.0f, 1.0f, 1.0f, 1.0f), mpTextManager->getText("Sound Settings")[mCurrLanguage], Font::CENTER);
		mpHUD->writeHUDText(Vector2D(getDisplayDimensions().getX() / 2.0f, 200.0f), *mpFont, Color(1.0f, 1.0f, 1.0f, 1.0f), mpTextManager->getText("Difficulty Settings")[mCurrLanguage], Font::CENTER);
		mpHUD->writeHUDText(Vector2D(getDisplayDimensions().getX() / 2.0f, 360.0f), *mpFont, Color(1.0f, 1.0f, 1.0f, 1.0f), mpTextManager->getText("Language Settings")[mCurrLanguage], Font::CENTER);
	}
	else if (mCurrScreen == GAME_OVER_SCEEN) //Renders game over sprite ui and text
	{
		mpHUD->draw(Vector2D(getDisplayDimensions().getX() / 2.0f - mTextBGSprite.getWidth() / 2.0f, 100.0f), mTextBGSprite);
		mpHUD->writeHUDText(Vector2D(getDisplayDimensions().getX() / 2.0f, 125.0f), *mpFont, Color(1.0f, 1.0f, 1.0f, 1.0f), mpTextManager->getText("Game Over Text")[mCurrLanguage], Font::CENTER);
	}
	else if (mCurrScreen == WIN_GAME_SCREEN) //Renders win game sprite ui and text
	{
		mpHUD->draw(Vector2D(-40.0f, 0.0f), mTextBGSprite);
		mpHUD->writeHUDText(Vector2D(5.0f, 5.0f), *mpFont, Color(1.0f, 1.0f, 1.0f, 1.0f), mpTextManager->getText("Win Game Text")[mCurrLanguage]);
	}

	if (mCurrScreen == OVERWORLD_SCREEN || mCurrScreen == GAME_SCREEN)
	{
		//Renders units
		mpUnitManager->drawTempUnits(backBuffer);

		//Renders particles
		mpPlayerParticlePool->render(backBuffer);
	}

	//Renders pause menu sprites
	if (mGamePaused)
	{
		mpHUD->draw(Vector2D(0.0f, 0.0f), mPauseBGSprite);
		mpHUD->draw(Vector2D(getDisplayDimensions().getX() / 2.0f - mTextBGSprite.getWidth() / 2.0f, 20.0f), mTextBGSprite);
		mpHUD->writeHUDText(Vector2D(getDisplayDimensions().getX() / 2.0f, 45.0f), *mpFont, Color(1.0f, 1.0f, 1.0f, 1.0f), mpTextManager->getText("Pause Text")[mCurrLanguage], Font::CENTER);
	}

	//Updates UI Menu
	//This is done here so that HUD text renders above UI Sprites
	mpMenuManager->renderCurrMenu(mpHUD->getBuffer());

	//Renders HUD
	mpHUD->renderHUD(backBuffer);

	//Flips backbuffer
	mpGraphicsSystem->render();
}

void Game::switchTextLanguage(Button::LanguageOptions language)
{
	//Switches language of ui text in text manager
	mCurrBulletsRemainingText = mpTextManager->getText("Bullets Remaining Text")[language];
	mCurrPlayerTurnText = mpTextManager->getText("Player Turn Text")[language];
	mCurrPlayerWinText = mpTextManager->getText("Player Win Text")[language];
	mCurrHealthDisplayText = mpTextManager->getText("Health Display Text")[language];
	mCurrTalkText = mpTextManager->getText("Talk Text")[language];
}

void Game::switchDifficulty(Difficulty difficulty)
{
	//Switches the bullet speed on difficulty change
	mCurrDifficulty = difficulty;

	switch (difficulty)
	{
	case EASY:
		mCurrBulletSpeed = mBulletSpeedEasy;
		break;
	case MEDIUM:
		mCurrBulletSpeed = mBulletSpeedMedium;
		break;
	case HARD:
		mCurrBulletSpeed = mBulletSpeedHard;
		break;
	}
}

Player* Game::getPlayer()
{
	//Helper get player function
	return mpUnitManager->getPlayer();
}

World* Game::getWorld()
{
	//Helper get world function
	return mpUnitManager->getWorld();
}

void Game::doLoop()
{
	//Sets loop to run until value is changed
	mRunning = true;

	//Sets up timers
	Timer gameTimer;
	PerformanceTracker* pTimeTracker = new PerformanceTracker();

	EventSystem::getInstance()->fireEvent( (EventType)GameEvent::GO_TO_TITLE_SCREEN );

	//Runs loop
	while (mRunning)
	{
		//Starts timer
		gameTimer.start();

		pTimeTracker->clearTracker("loop");
		pTimeTracker->startTracking("loop");

		////Get input
		//getPollingInput();
		mpInputSystem->updateEventQueue();

		if (mCurrScreen == GAME_SCREEN || mCurrScreen == OVERWORLD_SCREEN)
		{
			update(mFPS); //Updates actual game
		}

		////Render
		render();

		////Take a little nap
		gameTimer.sleepUntilElapsed(mFPS);

		pTimeTracker->stopTracking("loop");
		mpHUD->writeHUDText(Vector2D(550, 0), *mpFont, Color(1.0f, 1.0f, 1.0f), "FPS: " + to_string(1000.0 / pTimeTracker->getElapsedTime("loop")));
	}

	//Deletes Tracker
	delete pTimeTracker;
	pTimeTracker = nullptr;
}

UnitManager* Game::getUnitManager()
{
	return mpsInstance->mpUnitManager;
}

Game::Screens Game::getCurrScreen()
{
	return mpsInstance->mCurrScreen;
}

bool Game::getGameIsPaused()
{
	return mpsInstance->mGamePaused;
}

AnimationManager* Game::getAnimationManager()
{
	return mpAnimationManager;
}
