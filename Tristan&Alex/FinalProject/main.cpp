#include <iostream>
#include <cassert>
#include <string>
#include <random>
#include <ctime>

#include <PerformanceTracker.h>
#include <MemoryTracker.h>

#include <GraphicsSystem.h>
#include <GraphicsBuffer.h>
#include <Sprite.h>
#include <Color.h>
#include <Font.h>
#include "Game.h"
#include <EventSystem.h>

using namespace std;

int main()
{
	bool ret = false;
	const unsigned int SCREEN_WIDTH = 800;
	const unsigned int SCREEN_HEIGHT = 600;

	{
		EventSystem::createInstance();
		EventSystem* pEventSystem = EventSystem::getInstance();

		Game::createInstance();
		Game* pGame = Game::getInstance();

		pEventSystem->init();
		ret = pGame->init(SCREEN_WIDTH, SCREEN_HEIGHT, 20);

		pGame->doLoop();

		pGame->deleteInstance();
		pEventSystem->destroyInstance();
	}

	MemoryTracker::getInstance()->reportAllocations(cout);
	system("pause");

	return ret;

}