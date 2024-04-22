#include "scene.h"
#include "actor.h"
#include "input.h"
#include "camera.h"
#include "engine.h"
#include "renderer.h"
#include "component.h"
#include "../utilities/helper.h"
#include "../utilities/engineUtilities.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/SDL2_image/SDL_image.h"
#include "../vendor/rapidjson/document.h"
#include "../vendor/lua/lua.hpp"
#include "../vendor/LuaBridge/LuaBridge.h"

#include <cmath>
#include <string>
#include <utility>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <algorithm>
#include <filesystem>

const void Scene::loadScene(const std::string& name)
{
	std::string filePath = "resources/scenes/";
	std::string suffix = ".scene";

	if (std::filesystem::exists(filePath + name + suffix))
	{
		rapidjson::Document sceneInfo;

		std::string sceneFilePath = "resources/scenes/";
		std::string sceneSuffix = ".scene";
		std::string fullPath = sceneFilePath + name + sceneSuffix;

		EngineUtilities::readJsonFile(fullPath, sceneInfo);

		// LOAD here, do *not* render every actor every time (only the visible ones)
		lua_State* luaState = luaL_newstate();
		luaL_openlibs(luaState);

		int index = 0;
		for (const auto& actor : sceneInfo.FindMember("actors")->value.GetArray())
		{
			bool doesBounce = false;
			bool isBlocking = false;
			float rotation = 0.0;
			std::optional<int> renderOrder;
			glm::fvec2 position = { 0.0f, 0.0f };
			glm::fvec2 scale = { 1.0f, 1.0f };
			glm::fvec2 center = { 0.0f, 0.0f };
			glm::fvec2 velocity = { 0.0f, 0.0f };
			std::string nearbyDialogue;
			std::string contactDialogue;
			std::string name;
			std::string sprite;
			std::string backSprite;

			if (actor.HasMember("movement_bounce_enabled"))
			{
				doesBounce = actor.FindMember("movement_bounce_enabled")->value.GetBool();
			}

			if (actor.HasMember("blocking"))
			{
				isBlocking = actor.FindMember("blocking")->value.GetBool();
			}

			if (actor.HasMember("transform_rotation_degrees"))
			{
				rotation = actor.FindMember("transform_rotation_degrees")->value.GetFloat();
			}

			// X position.
			if (actor.HasMember("transform_position_x"))
			{
				position.x = actor.FindMember("transform_position_x")->value.GetFloat();
			}

			// Y position.
			if (actor.HasMember("transform_position_y"))
			{
				position.y = actor.FindMember("transform_position_y")->value.GetFloat();
			}

			// X scale.
			if (actor.HasMember("transform_scale_x"))
			{
				scale.x = actor.FindMember("transform_scale_x")->value.GetFloat();
			}

			// Y scale.
			if (actor.HasMember("transform_scale_y"))
			{
				scale.y = actor.FindMember("transform_scale_y")->value.GetFloat();
			}

			// X velocity.
			if (actor.HasMember("vel_x"))
			{
				velocity.x = actor.FindMember("vel_x")->value.GetFloat();
			}

			// Y velocity.
			if (actor.HasMember("vel_y"))
			{
				velocity.y = actor.FindMember("vel_y")->value.GetFloat();
			}

			if (actor.HasMember("nearby_dialogue"))
			{
				nearbyDialogue = actor.FindMember("nearby_dialogue")->value.GetString();
			}

			if (actor.HasMember("contact_dialogue"))
			{
				contactDialogue = actor.FindMember("contact_dialogue")->value.GetString();
			}

			if (actor.HasMember("name"))
			{
				name = actor.FindMember("name")->value.GetString();
			}

			// Ensure there is a health sprite if there is a player.
			if (name == "player")
			{
				hasPlayer = true;
				playerIndex = index;

				if (Engine::getRenderingConfigDocument()->HasMember("zoom_factor"))
				{
					m_camera.setCameraZoom(Engine::getRenderingConfigDocument()->FindMember("zoom_factor")->value.GetFloat());
				}
				else
				{
					m_camera.setCameraZoom(1.0f);
				}

				if (Engine::getGameConfigDocument()->HasMember("x_resolution"))
				{
					m_camera.setCameraWidth(Engine::getGameConfigDocument()->FindMember("x_resolution")->value.GetInt());
				}

				if (Engine::getGameConfigDocument()->HasMember("y_resolution"))
				{
					m_camera.setCameraWidth(Engine::getGameConfigDocument()->FindMember("y_resolution")->value.GetInt());
				}

				if (Engine::getRenderingConfigDocument()->HasMember("cam_offset_x"))
				{
					m_camera.setCameraOffset({ Engine::getRenderingConfigDocument()->FindMember("cam_offset_x")->value.GetFloat(),
						m_camera.getCameraOffset().y });
				}

				m_camera.setCameraPosition(position + m_camera.getCameraOffset());


				std::string filePrefix = "resources/images/";
				std::string fileSuffix = ".png";
				std::string fullPath = filePrefix + heartSpriteName + fileSuffix;

				heartSpritePath = fullPath;

				m_healthTexture = IMG_LoadTexture(Renderer::getRenderer(),
					heartSpritePath.c_str());

				SDL_QueryTexture(m_healthTexture, nullptr, nullptr, &m_healthTextureWidth, &m_healthTextureHeight);
			}

			if (actor.HasMember("view_image"))
			{
				sprite = actor.FindMember("view_image")->value.GetString();
			}

			if (actor.HasMember("view_image_back"))
			{
				backSprite = actor.FindMember("view_image_back")->value.GetString();
			}

			std::string filePrefix = "resources/images/";
			std::string fileSuffix = ".png";
			std::string fullPath = filePrefix + sprite + fileSuffix;

			// Need to load?
			std::string fullBackPath = filePrefix + backSprite + fileSuffix;

			// Loading twice?
			SDL_Texture* texture = IMG_LoadTexture(Renderer::getRenderer(), fullPath.c_str());
			SDL_Texture* backTexture = IMG_LoadTexture(Renderer::getRenderer(), fullBackPath.c_str());
			int width, height;

			// Delete?
			SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

			if (actor.HasMember("view_pivot_offset_x"))
			{
				center.x = actor.FindMember("view_pivot_offset_x")->value.GetFloat();
			}
			else
			{
				center.x = width * 0.5f;
			}

			if (actor.HasMember("view_pivot_offset_y"))
			{
				center.y = actor.FindMember("view_pivot_offset_y")->value.GetFloat();
			}
			else
			{
				center.y = height * 0.5f;
			}

			if (actor.HasMember("render_order"))
			{
				renderOrder = actor.FindMember("render_order")->value.GetInt();
			}
			Actor temp = Actor(doesBounce, isBlocking, rotation, renderOrder, position, scale, center,
				velocity, nearbyDialogue, contactDialogue, name, sprite, backSprite);
			m_actors.emplace_back(temp);

			// TODO: Make into function!
			//static std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> componentTables;
			std::map<std::string, Component> actorComponents;

			// Need to check if actor has components, may have template instead.
			if (actor.HasMember("template")) {
				const std::string templateName = actor["template"].GetString();
				const std::string templatePath = "resources/actor_templates/" + templateName + ".template";
				rapidjson::Document templateDocument;

				EngineUtilities::readJsonFile(templatePath, templateDocument);

				const rapidjson::Value& templateComponents = templateDocument["components"];
				for (const auto& itr : templateComponents.GetObject()) {
					// Load the component from the template
					// Verify component exists.
					std::string luaFile = std::string("resources/component_types/") + itr.value["type"].GetString() + std::string(".lua");
					if (!std::filesystem::exists(luaFile))
					{
						std::cout << "error: failed to locate component " << itr.value["type"].GetString();
						std::exit(EXIT_SUCCESS);
					}

					if (luaL_dofile(luaState, luaFile.c_str()) != LUA_OK)
					{
						std::cout << "problem with lua file " << itr.value["type"].GetString();
						std::exit(EXIT_SUCCESS);
					}

					// 1 : 
					std::string componentName = itr.name.GetString();
					Component newComponent(luaState, itr.value["type"].GetString(), componentName);
					//actorComponents.emplace(itr->name.GetString(), Component(luaState, itr->value["type"].GetString(), componentName));
					//componentTables.insert({ componentName,
						//std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(luaState, componentName.c_str())) });

					// Inject the component's properties (overrides)
					for (const auto& m : itr.value.GetObject())
					{
						if (std::string(m.name.GetString()) != "type")
						{ // Exclude the "type" property
							// Directly set the property on the component's Lua table
							if (m.value.IsString())
							{ // Check if the value is a string.
								(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetString();
							}
							else if (m.value.IsInt())
							{ // Check if the value is an int.
								(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetInt();
							}
							else if (m.value.IsFloat())
							{ // Check if the value is a float.
								(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetFloat();
							}
							else if (m.value.IsBool())
							{ // Check if the value is a bool.
								(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetBool();
							}
						}
					}

					// move vs emplace_back?
					actorComponents.emplace(componentName, std::move(newComponent)); // Emplace the component into the components map
				}
			}

			if (actor.HasMember("components"))
			{
				const rapidjson::Value& components = actor["components"];
				for (const auto& itr : components.GetObject())
				{
					if (itr.value.HasMember("type"))
					{
						// Verify component exists.
						std::string luaFile = std::string("resources/component_types/") + itr.value["type"].GetString() + std::string(".lua");
						if (!std::filesystem::exists(luaFile))
						{
							std::cout << "error: failed to locate component " << itr.value["type"].GetString();
							std::exit(EXIT_SUCCESS);
						}

						if (luaL_dofile(luaState, luaFile.c_str()) != LUA_OK)
						{
							std::cout << "problem with lua file " << itr.value["type"].GetString();
							std::exit(EXIT_SUCCESS);
						}

						// 1 : 
						std::string componentName = itr.name.GetString();
						auto foundComponentIt = actorComponents.find(componentName);
						//actorComponents.emplace(itr->name.GetString(), Component(luaState, itr->value["type"].GetString(), componentName));
						//componentTables.insert({ componentName,
							//std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(luaState, componentName.c_str())) });

						if (foundComponentIt != actorComponents.end()) {
							// Override component properties
							for (const auto& m : itr.value.GetObject()) {
								if (std::string(m.name.GetString()) != "type") {
									if (m.value.IsString()) {
										(*(foundComponentIt->second.getComponentReference()))[m.name.GetString()] = m.value.GetString();
									}
									else if (m.value.IsInt()) {
										(*(foundComponentIt->second.getComponentReference()))[m.name.GetString()] = m.value.GetInt();
									}
									else if (m.value.IsFloat()) {
										(*(foundComponentIt->second.getComponentReference()))[m.name.GetString()] = m.value.GetFloat();
									}
									else if (m.value.IsBool()) {
										(*(foundComponentIt->second.getComponentReference()))[m.name.GetString()] = m.value.GetBool();
									}
									// Add other types if necessary
								}
							}
						}
						else {
							// It is a new component
							Component newComponent(luaState, itr.value["type"].GetString(), componentName);
							for (const auto& m : itr.value.GetObject()) {
								if (std::string(m.name.GetString()) != "type") {
									if (m.value.IsString()) {
										(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetString();
									}
									else if (m.value.IsInt()) {
										(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetInt();
									}
									else if (m.value.IsFloat()) {
										(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetFloat();
									}
									else if (m.value.IsBool()) {
										(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetBool();
									}
								}
							}
							actorComponents.emplace(componentName, std::move(newComponent));
						}
					}
					else
					{
						// itr.name.GetString() = inherited_intro_message
						// Verify component exists.
						/*std::string luaFile = std::string("resources/component_types/") + itr.value["type"].GetString() + std::string(".lua");
						if (!std::filesystem::exists(luaFile))
						{
							std::cout << "error: failed to locate component " << itr.value["type"].GetString();
							std::exit(EXIT_SUCCESS);
						}

						if (luaL_dofile(luaState, luaFile.c_str()) != LUA_OK)
						{
							std::cout << "problem with lua file " << itr.value["type"].GetString();
							std::exit(EXIT_SUCCESS);
						}*/

						// 1 : 
						//std::string componentName = itr.name.GetString();
						//auto foundComponentIt = actorComponents.find(componentName);
						auto foundComponentIt = actorComponents.find(itr.name.GetString());
						//actorComponents.emplace(itr->name.GetString(), Component(luaState, itr->value["type"].GetString(), componentName));
						//componentTables.insert({ componentName,
							//std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(luaState, componentName.c_str())) });

						if (foundComponentIt != actorComponents.end()) {
							// Override component properties
							for (const auto& m : itr.value.GetObject()) {
								if (std::string(m.name.GetString()) != "type") {
									if (m.value.IsString()) {
										(*(foundComponentIt->second.getComponentReference()))[m.name.GetString()] = m.value.GetString();
									}
									else if (m.value.IsInt()) {
										(*(foundComponentIt->second.getComponentReference()))[m.name.GetString()] = m.value.GetInt();
									}
									else if (m.value.IsFloat()) {
										(*(foundComponentIt->second.getComponentReference()))[m.name.GetString()] = m.value.GetFloat();
									}
									else if (m.value.IsBool()) {
										(*(foundComponentIt->second.getComponentReference()))[m.name.GetString()] = m.value.GetBool();
									}
									// Add other types if necessary
								}
							}
						}
						//else {
						//	// I is a new component
						//	Component newComponent(luaState, itr.value["type"].GetString(), componentName);
						//	for (const auto& m : itr.value.GetObject()) {
						//		if (std::string(m.name.GetString()) != "type") {
						//			if (m.value.IsString()) {
						//				(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetString();
						//			}
						//			else if (m.value.IsInt()) {
						//				(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetInt();
						//			}
						//			else if (m.value.IsFloat()) {
						//				(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetFloat();
						//			}
						//			else if (m.value.IsBool()) {
						//				(*(newComponent.getComponentReference()))[m.name.GetString()] = m.value.GetBool();
						//			}
						//		}
						//	}
						//	actorComponents.emplace(componentName, std::move(newComponent));
						//}
					}
				}
			}

			// Actor class needs to be exposed to Lua before loading actor components
			luabridge::getGlobalNamespace(luaState)
				.beginClass<Actor>("Actor")
				.addFunction("GetName", &Actor::getName)  // Assuming the function is Actor::getName()
				.addFunction("GetID", &Actor::getID)      // Assuming the function is Actor::getID()
				.addFunction("GetComponentByKey", &Actor::getComponentByKey)
				.addFunction("GetComponent", &Actor::GetComponent)
				.addFunction("GetComponents", &Actor::GetComponents)
				.endClass();

			// Inject Debug.Log for all components.
			luabridge::getGlobalNamespace(luaState)
				.beginNamespace("Debug")
				.addFunction("Log", Component::Log)
				.addFunction("LogError", Component::LogError)
				.endNamespace();


			// Now iterate through the ordered map keys to initialize components
			//for (auto& actor : m_actors) {
				//Actor::allActors.push_back(&actor);
				temp.luaState = luaState;  // Store luaState in the actor for creating nil LuaRef if needed
				for (auto& [key, component] : actorComponents) {
					// The LuaRefs are already stored inside the Component object and are moved to Actor
					temp.components[key] = component.getComponentReference();
				}

				// Now iterate through each component and initialize them
				for (auto& [key, luaRef] : temp.components) {
					// Set the 'actor' to be the owner
					(*luaRef)["actor"] = &temp;

					// Retrieve onStart from the table and call it
					if ((*luaRef)["OnStart"].isFunction()) {


						//try
						//{
						(*luaRef)["OnStart"]((*luaRef));
						//}
						//catch (luabridge::LuaException e)
						//{
							//std::cout << "\033[33m" << actor.getName() << " : " << e.what() << "\033[0m" << std::endl;
						//}
					}
				}
			//}


			//for (auto& [key, luaRef] : actor.components) {
			//	// Set the 'actor' to be the owner
			//	(*luaRef)["actor"] = &actor;

			//	// Retrieve onStart from the table and call it
			//	if ((*luaRef)["OnStart"].isFunction()) {

			//		//Actor::Find(luaState, "Nikhil");

			//		//try
			//		//{
			//		(*luaRef)["OnStart"]((*luaRef));
			//		//}
			//		//catch (luabridge::LuaException e)
			//		//{
			//			//std::cout << "\033[33m" << actor.getName() << " : " << e.what() << "\033[0m" << std::endl;
			//		//}
			//	}
			//}


			++index;
		}

		// lua_close(luaState);


		m_sortedActors.reserve(m_actors.size());

		// Change this (move it up)?
		for (auto& actor : m_actors)
		{
			m_sortedActors.push_back(&actor);

			m_actorMap[EngineUtilities::createCompositeKey(actor.getPosition().x,
				actor.getPosition().y)].push_back(&actor);
		}

		//m_scenes.emplace(name, m_actors);
	}
}

const void Scene::renderScene()
{
	//// Render actors.


	//std::sort(m_sortedActors.begin(), m_sortedActors.end(),
	//	[](const Actor* a, const Actor* b) -> bool {
	//		// First, get render orders or y positions as sort keys
	//		auto getKey = [](const Actor* actor) {
	//			return actor->getRenderOrder().has_value() ? actor->getRenderOrder().value() : actor->getPosition().y;
	//			};

	//		float aKey = getKey(a);
	//		float bKey = getKey(b);

	//		// If keys are equal, fall back to comparing IDs
	//		if (aKey == bKey) {
	//			return a->getID() < b->getID();
	//		}

	//		// Otherwise, compare the keys
	//		return aKey < bKey;
	//	});

	//if (Actor::getPlayerHealth() <= 0)
	//{
	//}
	//else
	//{
	//	for (auto& actor : m_sortedActors)
	//	{
	//		Renderer::renderActor(*actor, m_camera);
	//	}
	//}

	//std::vector<std::pair<int, std::string>> dialogue;
	//// Populate dialogue vector.
	//if (!m_actors.empty())
	//{
	//	for (int i = m_actors[playerIndex].getPosition().x - 1; i <= m_actors[playerIndex].getPosition().x + 1; ++i)
	//	{
	//		for (int j = m_actors[playerIndex].getPosition().y - 1; j <= m_actors[playerIndex].getPosition().y + 1; ++j)
	//		{
	//			const auto& actorsAtPositionVector =
	//				m_actorMap.find(EngineUtilities::createCompositeKey(i, j));

	//			// Then an actor is at the specified location.
	//			if (actorsAtPositionVector != m_actorMap.end())
	//			{
	//				for (const auto& actor : actorsAtPositionVector->second)
	//				{
	//					if (i == m_actors[playerIndex].getPosition().x && j == m_actors[playerIndex].getPosition().y &&
	//						!actor->getContactDialogue().empty())
	//					{
	//						dialogue.push_back({ actor->getID(), actor->getContactDialogue() });

	//						// Handle new scene load.
	//						std::string s = EngineUtilities::obtainWordAfterPhrase(actor->getContactDialogue(), "proceed to");
	//						if (!s.empty())
	//						{
	//							m_actors.clear();
	//							m_actorMap.clear();
	//							m_sortedActors.clear();
	//							m_actorPointSet.clear();

	//							loadScene(s);
	//							SDL_RenderClear(Renderer::getRenderer());
	//							m_camera.setCameraPosition(static_cast<glm::fvec2>(m_actors[playerIndex].getPosition()) + m_camera.getCameraOffset());
	//							renderScene();
	//							return;
	//						}

	//						if (actor->getContactDialogue().find("you win") != std::string::npos && !hasWon)
	//						{
	//							Engine::setCurrentGameState(GameState::END_STATE);

	//							Audio::stopAudio();
	//							Audio::playGoodAudio();

	//							hasWon = true;

	//							return;
	//						}

	//						if (actor->getContactDialogue().find("health down") != std::string::npos &&
	//							Helper::GetFrameNumber() >= damageFrame + 180)
	//						{
	//							Actor::setPlayerHealth(Actor::getPlayerHealth() - 1);
	//							damageFrame = Helper::GetFrameNumber();
	//						}

	//						if (actor->getContactDialogue().find("score up") != std::string::npos &&
	//							m_actorPointSet.find(actor->getID()) == m_actorPointSet.end())
	//						{
	//							Actor::setPlayerScore(Actor::getPlayerScore() + 1);
	//							m_actorPointSet.insert(actor->getID());
	//						}
	//					}
	//					else if (!actor->getNearbyDialogue().empty())
	//					{
	//						dialogue.push_back({ actor->getID(), actor->getNearbyDialogue() });

	//						// Handle new scene load.
	//						std::string s = EngineUtilities::obtainWordAfterPhrase(actor->getNearbyDialogue(), "proceed to");
	//						if (!s.empty())
	//						{
	//							m_actors.clear();
	//							m_actorMap.clear();
	//							m_sortedActors.clear();
	//							m_actorPointSet.clear();

	//							loadScene(s);
	//							SDL_RenderClear(Renderer::getRenderer());
	//							m_camera.setCameraPosition(static_cast<glm::fvec2>(m_actors[playerIndex].getPosition()) + m_camera.getCameraOffset());
	//							renderScene();
	//							return;
	//						}

	//						if (actor->getNearbyDialogue().find("you win") != std::string::npos && !hasWon)
	//						{
	//							Engine::setCurrentGameState(GameState::END_STATE);

	//							Audio::stopAudio();
	//							Audio::playGoodAudio();

	//							hasWon = true;

	//							return;
	//						}

	//						if (actor->getNearbyDialogue().find("health down") != std::string::npos &&
	//							Helper::GetFrameNumber() >= damageFrame + 180)
	//						{
	//							Actor::setPlayerHealth(Actor::getPlayerHealth() - 1);
	//							damageFrame = Helper::GetFrameNumber();

	//							if (Actor::getPlayerHealth() <= 0 && !isGameOver)
	//							{
	//								
	//							}
	//						}

	//						if (actor->getNearbyDialogue().find("score up") != std::string::npos &&
	//							m_actorPointSet.find(actor->getID()) == m_actorPointSet.end())
	//						{
	//							Actor::setPlayerScore(Actor::getPlayerScore() + 1);
	//							m_actorPointSet.insert(actor->getID());
	//						}
	//					}
	//				}
	//			}

	//		}
	//	}
	//}

	//if (hasPlayer)
	//{
	//	// Render health.
	//	int numHearts = Actor::getPlayerHealth();

	//	for (int i = 0; i < numHearts; ++i)
	//	{
	//		Renderer::renderImage(heartSpriteName, 5 + i * (m_healthTextureWidth + 5), 25,
	//			m_healthTextureWidth, m_healthTextureHeight);
	//	}

	//	// Render score.
	//	Renderer::renderText("score : " + std::to_string(
	//		Actor::getPlayerScore()), 5, 5);
	//}

	//// Sort messages.
	//std::sort(dialogue.begin(), dialogue.end());

	//// Print dialogue.
	//int height = Renderer::getWindowYResolution();
	//for (size_t i = 0; i < dialogue.size(); ++i)
	//{
	//	Renderer::renderText(dialogue[i].second, 25, height - 50 - (dialogue.size() - 1 - i) * 50);
	//}

	//dialogue.clear();
}

const void Scene::updateActors()
{
	if (playerIndex != -1 && Engine::getCurrentGameState() !=
		GameState::INTRO_STATE)
	{
		// Set player velocity.
		glm::fvec2 direction = glm::fvec2(0.0f, 0.0f);

		if (Input::getKey(SDL_SCANCODE_UP) || Input::getKey(SDL_SCANCODE_W))
		{
			direction.y -= 1.0f;
		}

		if (Input::getKey(SDL_SCANCODE_DOWN) || Input::getKey(SDL_SCANCODE_S))
		{
			direction.y += 1.0f;
		}

		if (Input::getKey(SDL_SCANCODE_LEFT) || Input::getKey(SDL_SCANCODE_A))
		{
			direction.x -= 1.0f;
			if (Renderer::getFlip())
			{
				m_actors[playerIndex].setFlip(SDL_FLIP_HORIZONTAL);
				m_actors[playerIndex].setScale({ -std::fabs(m_actors[playerIndex].getScale().x), m_actors[playerIndex].getScale().y });
			}
		}

		if (Input::getKey(SDL_SCANCODE_RIGHT) || Input::getKey(SDL_SCANCODE_D))
		{
			direction.x += 1.0f;
			if (Renderer::getFlip())
			{
				m_actors[playerIndex].setFlip(SDL_FLIP_NONE);
				m_actors[playerIndex].setScale({ std::fabs(m_actors[playerIndex].getScale().x), m_actors[playerIndex].getScale().y });
			}
		}

		// Normalize the direction vector, and then scale it by the player speed
		if (glm::length(direction) != 0.0f)
		{
			direction = glm::normalize(direction) * m_playerSpeed;
		}

		// Now apply the normalized and scaled direction as the velocity
		m_actors[playerIndex].setVelocity(direction);

		for (auto& actor : m_actors)
		{
			glm::fvec2 desiredPosition = actor.getPosition() + actor.getVelocity();
			// Get actor vector iterator.
			const auto& actorAtPosition =
				m_actorMap.find(EngineUtilities::createCompositeKey(desiredPosition.x, desiredPosition.y));

			if (actor.getVelocity() != glm::fvec2(0.0f, 0.0f))
			{
				// Set flips.
				if (actor.getName() != "player")
				{
					if (Renderer::getFlip() && actor.getVelocity().x < 0.0f)
					{
						actor.setFlip(SDL_FLIP_HORIZONTAL);
						actor.setScale({ -std::fabs(m_actors[playerIndex].getScale().x), m_actors[playerIndex].getScale().y });
					}
					else if (Renderer::getFlip())
					{
						m_actors[playerIndex].setFlip(SDL_FLIP_NONE);
						m_actors[playerIndex].setScale({ std::fabs(m_actors[playerIndex].getScale().x), m_actors[playerIndex].getScale().y });
					}
				}

				if (actorAtPosition == m_actorMap.end() ||
					(actorAtPosition != m_actorMap.end() && !actorAtPosition->second.front()->getIsBlocking()))
				{
					// Get actor vector iterator.
					const auto& actorAtPosition =
						m_actorMap.find(EngineUtilities::createCompositeKey(actor.getPosition().x, actor.getPosition().y));
					auto& actorsAtPositionVector = actorAtPosition->second;

					// Remove the current actor from the vector.
					actorsAtPositionVector.erase(std::remove_if(
						actorsAtPositionVector.begin(),
						actorsAtPositionVector.end(), [&](Actor* ptr)
						{
							return ptr == &actor;
						}),
						actorsAtPositionVector.end());

					// If the vector is empty after erasing it,
					// then remove it from the map.
					if (actorsAtPositionVector.empty())
					{
						m_actorMap.erase(actorAtPosition);
					}

					// Move the actor.
					actor.setPosition(actor.getPosition() + actor.getVelocity());

					// Update the actor's position.
					m_actorMap[EngineUtilities::createCompositeKey(actor.getPosition().x, actor.getPosition().y)].push_back(&actor);
				}
				else if (actor.getName() != "player")
				{
					actor.setVelocity(actor.getVelocity() * -1.0f);
				}
			}
		}

		m_camera.setCameraPosition(glm::mix(m_camera.getPosition(), m_actors[playerIndex].getPosition() + m_camera.getCameraOffset(), Renderer::getCameraEaseFactor()));
		m_actors[playerIndex].setVelocity({ 0.0f, 0.0f });
	}
}
