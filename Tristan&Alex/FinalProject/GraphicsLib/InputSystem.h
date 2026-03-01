#pragma once
#include <SDL.h>
#include <Vector2D.h>
#include <Trackable.h>
#include <unordered_map>
#include <vector>

class InputSystem : public Trackable
{
public:
	enum Mouse_Buttons
	{
		LEFT = SDL_BUTTON_LEFT,
		RIGHT = SDL_BUTTON_RIGHT,
		MIDDLE = SDL_BUTTON_MIDDLE,
		NUM_MOUSE_BUTTON_EVENTS = 3
	};

	enum Keys
	{
		W_KEY = SDLK_w,
		A_KEY = SDLK_a,
		S_KEY = SDLK_s,
		D_KEY = SDLK_d,
		SPACE_KEY = SDLK_SPACE,
		NUM_1_KEY = SDLK_1,
		NUM_2_KEY = SDLK_2,
		ENTER_KEY = SDLK_KP_ENTER,
		ESCAPE_KEY = SDLK_ESCAPE,
		LSHIFT_KEY = SDLK_LSHIFT,
	};

	enum KeyScanCodes
	{
		F_KEY_SC = SDL_SCANCODE_F,
		M_KEY_SC = SDL_SCANCODE_M,
		S_KEY_SC = SDL_SCANCODE_S,
		SPACE_KEY_SC = SDL_SCANCODE_SPACE,
		NUM_1_KEY_SC = SDL_SCANCODE_1,
		NUM_2_KEY_SC = SDL_SCANCODE_2,
		ENTER_KEY_SC = SDL_SCANCODE_KP_ENTER,
		ESCAPE_KEY_SC = SDL_SCANCODE_ESCAPE,
		LSHIFT_KEY_SC= SDL_SCANCODE_LSHIFT,
	};

	InputSystem();
	~InputSystem();

	bool init();
	void cleanup();

	void updateEventQueue();
	void getKeyState();
	bool getKeyPressed(int keyCode);
	bool getKeyDown(KeyScanCodes keyCode);
	Uint32 getMouseState();
	bool getMousePressed(Mouse_Buttons button);
	Vector2D getMousePos();
private:
	const Uint8* mKeystates = nullptr;
	Vector2D mMousePos = Vector2D(0, 0);

	std::unordered_map<KeyScanCodes, bool> mKeyMap;
	std::vector<unsigned int> mKeyList;

	bool mKeyDown = false;
};
