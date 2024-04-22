#pragma once

#include "../vendor/glm/glm.hpp"
#include "../vendor/SDL2/SDL.h"

#include "../vendor/lua/lua.hpp"
#include "../vendor/LuaBridge/LuaBridge.h"

#include <string>
#include <memory>
#include <optional>

// This class houses all actor functionality and properties.
class Actor
{
public:
	Actor(
		bool doesBounce,
		bool isBlocking,
		float rotationInDegrees,
		std::optional<int> renderOrder,
		glm::fvec2 position,
		glm::fvec2 scale,
		glm::fvec2 center,
		glm::fvec2 velocity,
		std::string nearbyDialogue,
		std::string contactDialogue,
		std::string name,
		std::string sprite,
		std::string backSprite);

	inline SDL_Texture* getTexture() const { return m_texture; }
	inline SDL_Texture* getBackTexture() const { return m_backTexture; }
	inline const SDL_Rect* getDestViewRectangle() const { return &m_destRectangleView; }
	inline const float getRotationInDegrees() const { return m_rotationInDegrees; }
	inline const SDL_Point* getPivotPoint() const { return &m_pivotPoint; }
	inline const SDL_RendererFlip getFlip() const { return m_flip; }
	inline const void setFlip(SDL_RendererFlip newFlip) { m_flip = newFlip; }
	inline int getID() { return m_uuid; }
	inline std::string getName() { return m_name; }

	static inline const void setPlayerHealth(int newPlayerHealth) { m_playerHealth = newPlayerHealth; }

	static inline const int getPlayerHealth() { return m_playerHealth; }
	static inline const int getPlayerScore() { return m_playerScore; }
	static inline const void setPlayerScore(int newPlayerScore) { m_playerScore = newPlayerScore; }

	inline const glm::fvec2 getPosition() const { return m_position; }
	inline const void setPosition(const glm::fvec2& newPosition) { m_position = newPosition; calculateViewRectangleDimensions(); }

	inline const glm::fvec2 getVelocity() const { return m_velocity; }
	inline const void setVelocity(const glm::fvec2& newVelocity) { m_velocity = newVelocity; }
	inline const std::optional<int> getRenderOrder() const { return m_renderOrder; }

	inline const std::string& getNearbyDialogue() const { return m_nearbyDialogue; }
	inline const std::string& getContactDialogue() const { return m_contactDialogue; }

	inline const bool getIsBlocking() const { return m_isBlocking; }
	inline const glm::fvec2 getScale() const { return m_scale; }
	inline void setScale(const glm::fvec2& newScale) { m_scale = newScale; }

	inline const bool getIsFacingUp() const { return m_facingUp; }
	inline const void setIsFacingUp(bool newVal) { m_facingUp = newVal; }

	inline const int getBackTextureWidth() const { return m_backTextureWidth; }
	inline const int getBackTextureHeight() const { return m_backTextureHeight; }

	inline const bool getDoesBounce() const { return m_doesBounce; }

	std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components;

	// Function to retrieve a component by key, exposed to Lua.
	luabridge::LuaRef getComponentByKey(const std::string& key) const {
		auto it = components.find(key);
		if (it != components.end()) {
			return *(it->second);  // Dereference to get LuaRef
		}
		return luabridge::LuaRef(luaState);  // Return nil if not found
	}

	luabridge::LuaRef GetComponent(const std::string& type) const {
		for (const auto& compPair : components) {
			auto& compKey = compPair.first;
			auto& compRef = *(compPair.second);
			if (compRef["type"].isString() && compRef["type"].cast<std::string>() == type) {
				return compRef;  // Return LuaRef of the component
			}
		}
		return luabridge::LuaRef(luaState);  // Return nil if not found
	}

	luabridge::LuaRef GetComponents(const std::string& type_name) const {
		lua_State* L = luaState;  // Use the stored lua_State in the Actor instance
		luabridge::LuaRef table = luabridge::newTable(L);  // Create a new Lua table
		int index = 1;  // Lua tables start indexing at 1

		for (const auto& kv : components) {
			// Dereference shared pointer to luabridge::LuaRef and check component type
			if ((*kv.second)["type"].isString() && (*kv.second)["type"].cast<std::string>() == type_name) {
				table[index++] = *kv.second;  // Add component to the Lua table
			}
		}

		return table;  // Return the Lua table
	}

	lua_State* luaState = nullptr;

private:
	bool m_doesBounce;
	bool m_facingUp;

	// Whether or not this actor blocks other actors from colliding with it.
	bool m_isBlocking;

	static inline int m_nextUuid = 0;
	static inline int m_playerHealth = 3;
	static inline int m_playerScore = 0;

	int m_uuid;

	int m_textureWidth;
	int m_textureHeight;

	int m_backTextureWidth;
	int m_backTextureHeight;

	float m_rotationInDegrees;
	std::optional<int> m_renderOrder;

	glm::fvec2 m_position;
	glm::fvec2 m_scale;
	glm::fvec2 m_center;
	glm::fvec2 m_velocity;

	std::string m_nearbyDialogue;
	std::string m_contactDialogue;
	std::string m_name;
	std::string m_sprite;
	std::string m_backSprite;

	SDL_Texture* m_texture;
	SDL_Texture* m_backTexture;
	SDL_Rect m_srcRectangleView;
	glm::ivec2 m_spritePixelDimensions;
	SDL_Rect m_destRectangleView;
	SDL_Point m_pivotPoint;
	SDL_RendererFlip m_flip;

	const void calculateViewRectangleDimensions();
};
