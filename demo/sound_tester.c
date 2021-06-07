#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

int main() {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
    
    Mix_Chunk* remove_sound;
    Mix_Chunk* shoot_sound;
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    remove_sound = Mix_LoadWAV("remove_sound.wav");
    shoot_sound = Mix_LoadWAV("shoot_sound.wav");
    Mix_PlayChannel(-1, remove_sound, 0);
    Mix_PlayChannel(-1, shoot_sound, 0);

    Mix_FreeChunk(remove_sound);
    Mix_FreeChunk(shoot_sound);
}