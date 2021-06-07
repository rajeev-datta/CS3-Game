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

int main(int argc, char const *argv[]) {
    /*! Initializes audio system. Must be called before calling the items below. Should only be called once. */
    SDL_Init(SDL_INIT_AUDIO);

    /*! Initialize sound files. Should only be called once. */
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);

    /*! See https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_11.html
        for more in-depth explanation on the parameters.
    */
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        return -1;
    }

    /*! See https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_55.html */
    Mix_Music *sound = Mix_LoadMUS("wav/Countdown.wav");
    if (!sound) {
        printf("Mix_LoadMUS: %s\n", Mix_GetError());
        return -1;
    }

    /*! Plays the music, looping as many times as you want (second parameter)
        See https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_57.html */
    if (Mix_PlayMusic(sound, -1) == -1) {
        printf("Mix_PlayMusic: %s\n", Mix_GetError());
        return -1;
    }

    /*! While-loop to run music */
    while(1) {
        continue;
    }

    /*! Frees the music file pointer.
        See https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_56.html */

    Mix_FreeMusic(sound);
}
