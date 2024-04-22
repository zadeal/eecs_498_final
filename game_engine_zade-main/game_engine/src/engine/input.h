#pragma once

#include "../vendor/SDL2/SDL.h"

#include <vector>
#include <cstdint>
#include <unordered_map>

// This class handles both keyboard and mouse input by abstracting the SDL
// input functionality.
class Input
{
public:
	static void initializeInputStates();

	// Called at the beginning of the frame.
	static void processEvent(const SDL_Event& event);

	// Called at the end of the frame.
	static void lateUpdate();

	// Keyboard functionality.
	static bool getKey(SDL_Scancode keycode);
	static bool getKeyDown(SDL_Scancode keycode);
	static bool getKeyUp(SDL_Scancode keycode);

	// Mouse functionality.
	static bool getMouseButton(Uint8 button);
	static bool getMouseButtonDown(Uint8 button);
	static bool getMouseButtonUp(Uint8 button);

private:
	enum class InputState
	{
		INPUT_STATE_UP,
		INPUT_STATE_JUST_BECAME_DOWN,
		INPUT_STATE_DOWN,
		INPUT_STATE_JUST_BECAME_UP
	};

	static inline std::unordered_map<SDL_Scancode, InputState>
		m_keyboardStates;

	static inline std::vector<SDL_Scancode> m_justBecameDownScancodes;
	static inline std::vector<SDL_Scancode> m_justBecameUpScancodes;

	static inline std::unordered_map<Uint8, InputState>
		m_mouseButtonStates;

	static inline std::vector<Uint8> m_justBecameDownMouseButtons;
	static inline std::vector<Uint8> m_justBecameUpMouseButtons;
};
