#pragma once

#include "renderer.h"
#include "audio.h"

#include "../vendor/rapidjson/document.h"

enum class GameState
{
	INTRO_STATE,
	GAME_STATE,
	END_STATE
};

// This class contains various functionality dealing with engine specific
// functions such as executing the main game loop.
class Engine
{
public:
	Engine();

	const void beginGameLoop();
	static inline rapidjson::Document* getGameConfigDocument()
	{ return &m_gameConfigDocument; }

	static inline rapidjson::Document* getRenderingConfigDocument()
	{
		return &m_renderingConfigDocument;
	}

	static const void loadGameConfigDocument();
	static const void loadRenderingConfigDocument();
	static inline const void setCurrentGameState(GameState newGameState)
	{ m_currentGameState = newGameState; }

	static inline const GameState getCurrentGameState()
	{
		return m_currentGameState;
	}

private:
	static inline GameState m_currentGameState = GameState::INTRO_STATE;
	Renderer m_renderer;
	Audio m_audio;
	static inline rapidjson::Document m_gameConfigDocument;
	static inline rapidjson::Document m_renderingConfigDocument;

	const void processInput();

	const void initializeLua() const;
};
