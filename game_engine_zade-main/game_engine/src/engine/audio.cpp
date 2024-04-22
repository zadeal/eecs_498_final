#include "audio.h"
#include "engine.h"
#include "../utilities/audioHelper.h"

#include "../vendor/SDL2_mixer/SDL_mixer.h"

#include <string>
#include <cstdlib>
#include <filesystem>

Audio::Audio()
{
	AudioHelper::Mix_OpenAudio498(48000, AUDIO_F32SYS, 2, 2048);

	m_gameConfigDocument = Engine::getGameConfigDocument();
	m_isPlaying = false;
}

Mix_Chunk* Audio::loadAudio(const std::string& audioFilePath)
{
	return AudioHelper::Mix_LoadWAV498(audioFilePath.c_str());
}

const void Audio::playIntroAudio()
{
	if (!m_isPlaying)
	{
		std::string audioPath = "resources/audio/";
		if (std::filesystem::exists(audioPath))
		{
			if (m_gameConfigDocument->HasMember("intro_bgm"))
			{
				std::string audioName = m_gameConfigDocument->FindMember(
					"intro_bgm")->value.GetString();

				Mix_Chunk* audio = nullptr;

				// Try OGG file first.
				std::string oggSuffix = ".ogg";
				std::string finalPath = audioPath + audioName + oggSuffix;
				audio = loadAudio(finalPath);

				// Try WAV file.
				if (!audio)
				{
					std::string wavSuffix = ".wav";
					std::string finalPath = audioPath + audioName + wavSuffix;
					audio = loadAudio(finalPath);
				}

				// If audio is still null, it was not loaded correctly or does
				// not exist.
				if (!audio)
				{
					std::cout << "error: failed to play audio clip "
						<< audioName;
					std::exit(EXIT_SUCCESS);
				}


				AudioHelper::Mix_PlayChannel498(0, audio, -1);
				m_isPlaying = true;
			}
		}
	}
}

const void Audio::playGameAudio()
{
	if (!m_isPlaying)
	{
		std::string audioPath = "resources/audio/";
		if (std::filesystem::exists(audioPath))
		{
		}
	}
}

const void Audio::playBadAudio()
{
	if (!m_isPlaying)
	{
		std::string audioPath = "resources/audio/";
		if (std::filesystem::exists(audioPath))
		{
		}
	}
}

const void Audio::playGoodAudio()
{
	if (!m_isPlaying)
	{
		std::string audioPath = "resources/audio/";
		if (std::filesystem::exists(audioPath))
		{
		}
	}
}

const void Audio::stopAudio()
{
	// Only halt if music is currently playing on channel zero.
	if (m_isPlaying)
	{
		AudioHelper::Mix_HaltChannel498(0);
		m_isPlaying= false;
	}
}
