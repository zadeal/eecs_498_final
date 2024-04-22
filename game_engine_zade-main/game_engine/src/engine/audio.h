#pragma once

#include "../vendor/rapidjson/document.h"

#if !__APPLE__
    #include "../vendor/SDL2_mixer/SDL_mixer.h"
#else
    #include <SDL2_mixer/SDL_mixer.h>
#endif

#include <string>

class Audio
{
public:
	Audio();
	static inline Mix_Chunk* loadAudio(const std::string& audioFilePath);
	const void playIntroAudio();
	const void playGameAudio();
	static const void playBadAudio();
	static const void playGoodAudio();
	static const void stopAudio();

private:
	// Mix_Playing was off by one frame for me.
	static inline bool m_isPlaying;
	static inline rapidjson::Document* m_gameConfigDocument;
};
