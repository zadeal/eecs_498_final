#pragma once

#include "actor.h"
#include "camera.h"

#include "../vendor/rapidjson/document.h"

#if !__APPLE__
    #include "../vendor/SDL2_image/SDL_image.h"
#else
    #include <SDL2_image/SDL_image.h>
#endif

#include <string>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_set>
#include <unordered_map>

class Scene
{
public:
	static const void loadScene(const std::string& name);
	static const void renderScene();
	static const void updateActors();

private:
	static inline float m_playerSpeed = 0.02f;

	static inline bool hasPlayer = false;
	static inline bool isGameOver = false;
	static inline bool hasWon = false;
	static inline int playerIndex = -1;
	static inline int damageFrame = -180;
	static inline std::vector<Actor> m_actors;

	// Actors sorted by render order.
	static inline std::vector<Actor*> m_sortedActors;

	static inline std::unordered_map<uint64_t, std::vector<Actor*>> m_actorMap;

	static inline std::unordered_set<int> m_actorPointSet;

	static inline std::string heartSpritePath;
	static inline std::string heartSpriteName;
	static inline Camera m_camera;
	static inline SDL_Texture* m_healthTexture;
	static inline int m_healthTextureWidth;
	static inline int m_healthTextureHeight;
};
