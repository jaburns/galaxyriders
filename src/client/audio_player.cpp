#include "audio_player.hpp"
#include "gl.hpp"

#include <iostream>

#ifdef _WIN32
    #include <SDL_mixer.h>
#else
    #include <SDL2/SDL_mixer.h>
#endif

static Mix_Chunk *wavy = nullptr;

AudioPlayer::AudioPlayer()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cout << "Failed to init SDL audio subsystem." << std::endl;
        exit(1);
    }

    int flags = 0, result = 0;
    if (flags != (result = Mix_Init(flags))) {
        std::cout << "Failed to init SDL_mixer." << std::endl;
        std::cout << "Error code: " << Mix_GetError() << std::endl;
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        std::cout << "Mix_OpenAudio() failed." << std::endl;
        exit(1);
    }

    wavy = Mix_LoadWAV("res/kick.wav");
}

void AudioPlayer::play()
{
    Mix_PlayChannel(-1, wavy, 0);
}

AudioPlayer::~AudioPlayer()
{
    Mix_FreeChunk(wavy);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}