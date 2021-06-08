#include "sound.h"

static const char *MUSIC = "sounds/music.wav";

void load_sound() {
    //set up music
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        return;
    }
    Mix_Music *sound = Mix_LoadMUS(MUSIC);
    if (!sound) {
        printf("Mix_LoadMUS: %s\n", Mix_GetError());
        return;
    }
    if (Mix_PlayMusic(sound, -1) == -1) {
        printf("Mix_PlayMusic: %s\n", Mix_GetError());
        return;
    }
}

void play_remove_sound() {
    Mix_Chunk* remove_sound;
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    remove_sound = Mix_LoadWAV("remove_sound.wav");
    Mix_PlayChannel(-1, remove_sound, 0);
    Mix_FreeChunk(remove_sound);
}

void play_shoot_sound() {
    Mix_Chunk* shoot_sound;
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    shoot_sound = Mix_LoadWAV("shoot_sound.wav");
    Mix_PlayChannel(-1, shoot_sound, 0);
    Mix_FreeChunk(shoot_sound);
}

void free_sound() {

}
