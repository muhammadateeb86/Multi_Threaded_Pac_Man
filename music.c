#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "music.h"

void playmusic(const char * filename)
{
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return ;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    return ;
    }

    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) {
        printf("Error loading music: %s\n", Mix_GetError());
        exit(1);
    }
    Mix_PlayMusic(music, -1); // Play the music in 
}
