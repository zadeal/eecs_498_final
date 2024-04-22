#pragma once

#include "actor.h"
#include "camera.h"

#include "../vendor/rapidjson/document.h"

#if !__APPLE__
    #include "../vendor/SDL2/SDL.h"
    #include "../vendor/SDL2_ttf/SDL_ttf.h"
#else
    #include <SDL2/SDL.h>
    #include <SDL2_ttf/SDL_ttf.h>
#endif

#include <string>
#include <unordered_map>

// This class handles various rendering functionality such as window creation,
// renderer creation, as well as image, text, and actor rendering.
class Renderer
{
public:
	// Initializes SDL, window properties, and config documents.
	Renderer();

	static inline SDL_Renderer* getRenderer() { return m_renderer; }
	static const void renderImage(const std::string& imageName,
		int x, int y, int w, int h);

	static const void renderEnding(const std::string& imageName,
		int x, int y, int w, int h);

	const void renderText(const std::string& text) const;
	static const void renderText(const std::string& text, int x, int y);

	static const void renderActor(Actor& actor, Camera& camera);

	const void renderIntro();

	static inline const int getWindowXResolution() { return m_xResolution; }
	static inline const int getWindowYResolution() { return m_yResolution; }

	static inline const float getCameraZoom() { return m_cameraZoom; }
	static inline const float getCameraEaseFactor() { return m_easeFactor; }
	static inline const bool getFlip() { return m_flipDirection; }

private:
	static inline bool m_flipDirection;
	static inline float m_easeFactor;
	static inline float m_cameraZoom;
	static inline int m_xResolution;
	static inline int m_yResolution;
	SDL_Window* m_window;
	static inline SDL_Renderer* m_renderer;
	static inline TTF_Font* m_font;
	rapidjson::Value::ConstValueIterator m_currentIntroImage;
	rapidjson::Value::ConstValueIterator m_currentIntroText;
	rapidjson::Document* m_gameConfigDocument;
	rapidjson::Document m_renderingConfigDocument;

	static inline std::unordered_map<std::string, SDL_Texture*> m_introImages;

	const void initializeWindowProperties();
	const void createWindow(const std::string& windowTitle);
	const void createRenderer();
	const void loadIntroImages();
	const void loadIntroText();
};
