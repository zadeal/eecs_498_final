#include "input.h"

#include "../vendor/SDL2/SDL.h"

void Input::initializeInputStates()
{
	// This should be modified as more user inputs are added.
	for (int code = SDL_SCANCODE_A; code <= SDL_SCANCODE_UP; ++code)
	{
		m_keyboardStates[static_cast<SDL_Scancode>(code)] =
			InputState::INPUT_STATE_UP;
	}

	for (int code = SDL_BUTTON_LEFT; code <= SDL_BUTTON_LEFT; ++code)
	{
		m_mouseButtonStates[static_cast<Uint8>(code)] =
			InputState::INPUT_STATE_UP;
	}
}

void Input::processEvent(const SDL_Event& event)
{
	// Handle key down.
	if (event.type == SDL_KEYDOWN)
	{
		m_keyboardStates[event.key.keysym.scancode] =
			InputState::INPUT_STATE_JUST_BECAME_DOWN;

		m_justBecameDownScancodes.push_back(event.key.keysym.scancode);
	}
	// Handle key up.
	else if (event.type == SDL_KEYUP)
	{
		m_keyboardStates[event.key.keysym.scancode] =
			InputState::INPUT_STATE_JUST_BECAME_UP;

		m_justBecameUpScancodes.push_back(event.key.keysym.scancode);
	}

	// Handle mouse button down.
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		m_mouseButtonStates[event.button.button] =
			InputState::INPUT_STATE_JUST_BECAME_DOWN;

		m_justBecameDownMouseButtons.push_back(event.button.button);
	}
	// Handle mouse button up.
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		m_mouseButtonStates[event.button.button] =
			InputState::INPUT_STATE_JUST_BECAME_UP;

		m_justBecameUpMouseButtons.push_back(event.button.button);
	}
}

void Input::lateUpdate()
{
	// Reset key just became down scancodes.
	for (const SDL_Scancode& code : m_justBecameDownScancodes)
	{
		m_keyboardStates[code] = InputState::INPUT_STATE_DOWN;
	}

	m_justBecameDownScancodes.clear();

	// Reset key just became up scancodes.
	for (const SDL_Scancode& code : m_justBecameUpScancodes)
	{
		m_keyboardStates[code] = InputState::INPUT_STATE_UP;
	}

	m_justBecameUpScancodes.clear();

	// Reset mouse just became down buttons.
	for (const Uint8& button : m_justBecameDownMouseButtons)
	{
		m_mouseButtonStates[button] = InputState::INPUT_STATE_DOWN;
	}
	m_justBecameDownMouseButtons.clear();

	// Reset mouse just became up buttons.
	for (const Uint8& button : m_justBecameUpMouseButtons)
	{
		m_mouseButtonStates[button] = InputState::INPUT_STATE_UP;
	}
	m_justBecameUpMouseButtons.clear();
}

bool Input::getKey(SDL_Scancode keycode)
{
	InputState currentState = m_keyboardStates.find(keycode)->second;

	return currentState == InputState::INPUT_STATE_JUST_BECAME_DOWN ||
		currentState == InputState::INPUT_STATE_DOWN;
}

bool Input::getKeyDown(SDL_Scancode keycode)
{
	InputState currentState = m_keyboardStates.find(keycode)->second;
	
	return currentState == InputState::INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::getKeyUp(SDL_Scancode keycode)
{
	InputState currentState = m_keyboardStates.find(keycode)->second;

	return currentState == InputState::INPUT_STATE_JUST_BECAME_UP;
}

bool Input::getMouseButton(Uint8 button)
{
	InputState currentState = m_mouseButtonStates.find(button)->second;
	return currentState == InputState::INPUT_STATE_DOWN ||

		currentState == InputState::INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::getMouseButtonDown(Uint8 button)
{
	InputState currentState = m_mouseButtonStates.find(button)->second;

	return currentState == InputState::INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::getMouseButtonUp(Uint8 button)
{
	InputState currentState = m_mouseButtonStates.find(button)->second;

	return currentState == InputState::INPUT_STATE_JUST_BECAME_UP;
}
