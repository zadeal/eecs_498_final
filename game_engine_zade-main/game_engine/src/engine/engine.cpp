#include "engine.h"
#include "renderer.h"
#include "input.h"
#include "scene.h"
#include "../utilities/helper.h"
#include "../utilities/engineUtilities.h"

#include "../vendor/SDL2/SDL.h"
#include "../vendor/lua/lua.hpp"
#include "../vendor/LuaBridge/LuaBridge.h"

#include <cstdlib>
#include <filesystem>

Engine::Engine()
{
	initializeLua();
}

const void Engine::beginGameLoop()
{
	// Initialize.
	Input::initializeInputStates();
	Scene::loadScene(m_gameConfigDocument.FindMember("initial_scene")->value.GetString());

	while (true)
	{
		processInput();
		SDL_RenderClear(m_renderer.getRenderer());

		switch (m_currentGameState)
		{
		case GameState::INTRO_STATE:
			m_audio.playIntroAudio();
			m_renderer.renderIntro();
			break;

		case GameState::GAME_STATE:
			m_audio.playGameAudio();
			Scene::updateActors();
			Scene::renderScene();
			break;

		case GameState::END_STATE:
			Scene::renderScene();
			break;
		}

		Helper::SDL_RenderPresent498(m_renderer.getRenderer());
		Input::lateUpdate();
	}
}

const void Engine::processInput()
{
	SDL_Event event;
	while (Helper::SDL_PollEvent498(&event))
	{
		Input::processEvent(event);

		if (event.type == SDL_QUIT)
		{
			SDL_RenderClear(m_renderer.getRenderer());
			Scene::updateActors();
			Scene::renderScene();
			Helper::SDL_RenderPresent498(m_renderer.getRenderer());
			std::exit(EXIT_SUCCESS);
		}
	}
}

const void Engine::loadGameConfigDocument()
{
	if (std::filesystem::exists("resources/game.config"))
	{
		EngineUtilities::readJsonFile("resources/game.config",
			m_gameConfigDocument);
	}
}

const void Engine::loadRenderingConfigDocument()
{
	if (std::filesystem::exists("resources/rendering.config"))
	{
		EngineUtilities::readJsonFile("resources/rendering.config",
			m_renderingConfigDocument);
	}
}

const void Engine::initializeLua() const
{
	lua_State* luaState = luaL_newstate();
	luaL_openlibs(luaState);
	/*const char* luaCode = "print('Hello, Lua!')";
	luaL_dostring(luaState, luaCode);*/
}
