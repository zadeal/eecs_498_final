#include "renderer.h"
#include "input.h"
#include "engine.h"
#include "audio.h"
#include "scene.h"
#include "camera.h"
#include "../utilities/helper.h"
#include "../utilities/engineUtilities.h"

#include "../vendor/SDL2/SDL.h"
#include "../vendor/SDL2_ttf/SDL_ttf.h"
#include "../vendor/rapidjson/document.h"
#include "../vendor/SDL2_image/SDL_image.h"

#include <cmath>
#include <string>
#include <cstdlib>
#include <filesystem>

Renderer::Renderer()
{
	m_xResolution = 640;
	m_yResolution = 360;
	m_cameraZoom = 1.0f;
	m_easeFactor = 1.0f;
	m_flipDirection = false;

	SDL_Init(SDL_INIT_VIDEO);
	initializeWindowProperties();
	createRenderer();
	loadIntroImages();
	loadIntroText();
}

const void Renderer::renderImage(const std::string& imageName, int x, int y, int w, int h)
{
	SDL_Rect imageViewport{};

	imageViewport.x = x;
	imageViewport.y = y;
	imageViewport.w = w;
	imageViewport.h = h;

	SDL_RenderCopy(m_renderer, m_introImages.find(imageName)->second, nullptr,
		&imageViewport);
}

const void Renderer::renderEnding(const std::string& imageName, int x, int y, int w, int h)
{
	SDL_Rect imageViewport{};

	imageViewport.x = x;
	imageViewport.y = y;
	imageViewport.w = w;
	imageViewport.h = h;

	SDL_Texture* endingImage = IMG_LoadTexture(m_renderer, imageName.c_str());

	SDL_RenderCopy(m_renderer, endingImage, nullptr,
		&imageViewport);
}

const void Renderer::renderText(const std::string& text) const
{
	SDL_Surface* surface = TTF_RenderText_Solid(m_font,
		text.c_str(), {255, 255, 255, 255});

	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

	SDL_Rect TextViewport{};

	TextViewport.x = 25;
	TextViewport.y = m_yResolution - 50;
	TextViewport.w = surface->w;
	TextViewport.h = surface->h;

	SDL_RenderCopy(m_renderer, texture, nullptr, &TextViewport);
}

const void Renderer::renderText(const std::string& text, int x, int y)
{
	SDL_Surface* surface = TTF_RenderText_Solid(m_font,
		text.c_str(), { 255, 255, 255, 255 });

	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

	SDL_Rect TextViewport{};

	TextViewport.x = x;
	TextViewport.y = y;
	TextViewport.w = surface->w;
	TextViewport.h = surface->h;

	SDL_RenderCopy(m_renderer, texture, nullptr, &TextViewport);
}

const void Renderer::renderActor(Actor& actor, Camera& camera)
{
	camera.setCameraZoom(m_cameraZoom);
	glm::fvec2 cameraOffset = static_cast<glm::fvec2>(camera.getRenderOffset());

	cameraOffset *= (1.0f / m_cameraZoom);

	// Calculate the position of the actor with respect to the camera's
	// viewport.
	SDL_Rect actorRect = *actor.getDestViewRectangle();
	actorRect.x -= static_cast<int>(cameraOffset.x + 320.0f);
	actorRect.y -= static_cast<int>(cameraOffset.y + 180.0f);

	SDL_RenderSetScale(m_renderer, m_cameraZoom, m_cameraZoom);

	if (Input::getKey(SDL_SCANCODE_DOWN) || Input::getKey(SDL_SCANCODE_S))
	{
		actor.setIsFacingUp(false);
	}

	if ((actor.getBackTexture() != nullptr && (Input::getKey(SDL_SCANCODE_UP) || Input::getKey(SDL_SCANCODE_W))) ||
		actor.getIsFacingUp())
	{
		actor.setIsFacingUp(true);

		actorRect.w = actor.getBackTextureWidth() * std::abs(actor.getScale().x);
		actorRect.h = actor.getBackTextureHeight() * std::abs(actor.getScale().y);

		Helper::SDL_RenderCopyEx498(actor.getID(), actor.getName(),
			m_renderer, actor.getBackTexture(), nullptr,
			&actorRect, actor.getRotationInDegrees(),
			actor.getPivotPoint(), actor.getFlip());
	}
	else
	{
		Helper::SDL_RenderCopyEx498(actor.getID(), actor.getName(),
			m_renderer, actor.getTexture(), nullptr,
			&actorRect, actor.getRotationInDegrees(),
			actor.getPivotPoint(), actor.getFlip());
	}

	/*SDL_SetRenderDrawColor(Renderer::getRenderer(), 255, 0, 0, 255);
	SDL_RenderDrawRect(Renderer::getRenderer(), &actorRect);
	SDL_SetRenderDrawColor(Renderer::getRenderer(), 0, 0, 0, 255);*/

	SDL_RenderSetScale(m_renderer, 1.0f, 1.0f);
}

const void Renderer::renderIntro()
{
	if (Input::getKeyDown(SDL_SCANCODE_SPACE) ||
		Input::getKeyDown(SDL_SCANCODE_RETURN) ||
		Input::getMouseButtonDown(SDL_BUTTON_LEFT))
	{
		++m_currentIntroImage;
		++m_currentIntroText;
	}

	if (m_gameConfigDocument->HasMember("intro_image"))
	{
		if (m_gameConfigDocument->FindMember("intro_text") != m_gameConfigDocument->MemberEnd())
		{
			if (m_currentIntroImage >=
				m_gameConfigDocument->FindMember("intro_image")->value.GetArray().end()
				&& m_currentIntroText <
				m_gameConfigDocument->FindMember("intro_text")->value.GetArray().end())
			{
				renderImage((m_gameConfigDocument->FindMember(
					"intro_image")->value.GetArray().end() - 1)->GetString(), 0, 0, m_xResolution, m_yResolution);
				renderText(m_currentIntroText->GetString());
				return;
			}

			if (m_currentIntroText >=
				m_gameConfigDocument->FindMember("intro_text")->value.GetArray().end()
				&& m_currentIntroImage <
				m_gameConfigDocument->FindMember(
					"intro_image")->value.GetArray().end())
			{
				renderImage(m_currentIntroImage->GetString(), 0, 0, m_xResolution, m_yResolution);
				if (!m_gameConfigDocument->FindMember(
					"intro_text")->value.GetArray().Empty())
				{
					renderText((m_gameConfigDocument->FindMember(
						"intro_text")->value.GetArray().end() - 1)->GetString());
				}
				return;
			}
		}

		// Render intro image.
		if (m_currentIntroImage <
			m_gameConfigDocument->FindMember("intro_image")->value.GetArray().end())
		{
			renderImage(m_currentIntroImage->GetString(), 0, 0, m_xResolution, m_yResolution);
		}

		// Render intro text.
		if (m_gameConfigDocument->FindMember("intro_text") != m_gameConfigDocument->MemberEnd())
		{
			if (m_currentIntroText <
				m_gameConfigDocument->FindMember("intro_text")->value.GetArray().end())
			{
				renderText(m_currentIntroText->GetString());
			}
		}

		if (m_gameConfigDocument->FindMember("intro_text") != m_gameConfigDocument->MemberEnd())
		{
			if (m_currentIntroImage >=
				m_gameConfigDocument->FindMember("intro_image")->value.GetArray().end()
				&& m_currentIntroText >=
				m_gameConfigDocument->FindMember("intro_text")->value.GetArray().end())
			{
				Audio::stopAudio();
				Engine::setCurrentGameState(GameState::GAME_STATE);
				Scene::updateActors();
				Scene::renderScene();
				// Load initial scene here.
				//Scene::loadScene(m_gameConfigDocument->FindMember("initial_scene")->value.GetString());
			}
		}
	}
	else
	{
		Audio::stopAudio();
		Engine::setCurrentGameState(GameState::GAME_STATE);
		Scene::updateActors();
		Scene::renderScene();
		// Load initial scene here.
		//Scene::loadScene(m_gameConfigDocument->FindMember("initial_scene")->value.GetString());
	}
}

const void Renderer::initializeWindowProperties()
{
	Engine::loadGameConfigDocument();
	Engine::loadRenderingConfigDocument();

	// Load game.config file contents into a document.
	if (std::filesystem::exists("resources/game.config"))
	{
		m_gameConfigDocument = Engine::getGameConfigDocument();
	}

	// Set window title.
	std::string windowTitle;

	if (m_gameConfigDocument->HasMember("game_title"))
	{
		windowTitle = m_gameConfigDocument->FindMember(
			"game_title")->value.GetString();
	}

	// Load rendering.config file contents into a document.
	if (std::filesystem::exists("resources/rendering.config"))
	{
		EngineUtilities::readJsonFile("resources/rendering.config",
			m_renderingConfigDocument);

		// Set window x resolution.
		if (m_renderingConfigDocument.HasMember("x_resolution"))
		{
			m_xResolution = m_renderingConfigDocument["x_resolution"].GetInt();
		}

		// Set window y resolution.
		if (m_renderingConfigDocument.HasMember("y_resolution"))
		{
			m_yResolution = m_renderingConfigDocument["y_resolution"].GetInt();
		}

		// Set camera zoom resolution.
		if (m_renderingConfigDocument.HasMember("zoom_factor"))
		{
			m_cameraZoom = m_renderingConfigDocument["zoom_factor"].GetFloat();
		}
		
		// Set camera ease factor.
		if (m_renderingConfigDocument.HasMember("cam_ease_factor"))
		{
			m_easeFactor = m_renderingConfigDocument["cam_ease_factor"].GetFloat();
		}
	}

	createWindow(windowTitle);
}

const void Renderer::createWindow(const std::string& windowTitle)
{
	m_window = Helper::SDL_CreateWindow498(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, m_xResolution, m_yResolution,
		SDL_WINDOW_SHOWN);
}

const void Renderer::createRenderer()
{
	m_renderer = Helper::SDL_CreateRenderer498(m_window, -1,
		SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	// Set red clear color value from game config document.
	int red = 255;
	int green = 255;
	int blue = 255;

	if (std::filesystem::exists("resources/rendering.config"))
	{

		if (m_renderingConfigDocument.HasMember("clear_color_r"))
		{
			red = m_renderingConfigDocument["clear_color_r"].GetInt();
		}

		// Set green clear color value from game config document.
		if (m_renderingConfigDocument.HasMember("clear_color_g"))
		{
			green = m_renderingConfigDocument["clear_color_g"].GetInt();
		}

		// Set blue clear color value from game config document.
		if (m_renderingConfigDocument.HasMember("clear_color_b"))
		{
			blue = m_renderingConfigDocument["clear_color_b"].GetInt();
		}
	}

	SDL_SetRenderDrawColor(m_renderer, red, green, blue, 255);
	SDL_RenderClear(m_renderer);
}

const void Renderer::loadIntroImages()
{
	const std::string introImageFilePath = "resources/images";

	if (std::filesystem::exists(introImageFilePath))
	{
		const auto& introImages =
			std::filesystem::directory_iterator(introImageFilePath);

		for (const auto& introImage : introImages)
		{
			m_introImages[introImage.path().stem().string()] =
				IMG_LoadTexture(m_renderer,
					introImage.path().string().c_str());
		}
	}

	// Ensure files specified in the game config file actually exist.
	if (m_gameConfigDocument->HasMember("intro_image"))
	{
		for (const auto& image :
			m_gameConfigDocument->FindMember("intro_image")->value.GetArray())
		{
			if (m_introImages.find(image.GetString()) == m_introImages.end())
			{
				std::cout << "error: missing image " << image.GetString();
				std::exit(EXIT_SUCCESS);
			}
		}

		m_currentIntroImage =
			m_gameConfigDocument->FindMember(
				"intro_image")->value.GetArray().begin();
	}
	else
	{
		Engine::setCurrentGameState(GameState::GAME_STATE);
	}
}

const void Renderer::loadIntroText()
{
	TTF_Init();

	if (m_gameConfigDocument->HasMember("intro_text"))
	{
		m_currentIntroText =
			m_gameConfigDocument->FindMember(
				"intro_text")->value.GetArray().begin();

		if (!m_gameConfigDocument->HasMember("font"))
		{
			std::cout << "error: text render failed. No font configured";
			std::exit(EXIT_SUCCESS);
		}
	}

	if (m_gameConfigDocument->HasMember("font"))
	{
		std::string fontPrefix = "resources/fonts/";
		std::string fontName = m_gameConfigDocument->FindMember(
			"font")->value.GetString();
		std::string fontSuffix = ".ttf";
		std::string fontPath = fontPrefix + fontName + fontSuffix;

		if (!std::filesystem::exists(fontPath))
		{
			std::cout << "error: font " << fontName << " missing";
			std::exit(EXIT_SUCCESS);
		}

		m_font = TTF_OpenFont(fontPath.c_str(), 16);
	}
}
