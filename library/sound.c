#include "sound.h"

static Mix_Music *MUSIC;
static Mix_Chunk* REMOVE_SOUND; 
static Mix_Chunk* SHOOT_SOUND;

void load_sound() {
    //set up music
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        return;
    }
    MUSIC = Mix_LoadMUS("sounds/music.wav");
    if (!MUSIC) {
        return;
    }
    if (Mix_PlayMusic(MUSIC, -1) == -1) {
        return;
    }
    REMOVE_SOUND = Mix_LoadWAV("sounds/remove_sound.wav");
    SHOOT_SOUND = Mix_LoadWAV("sounds/shoot_sound.wav");
}

void play_remove_sound() {
    Mix_PlayChannel(-1, REMOVE_SOUND, 0);
}

void play_shoot_sound() {
    Mix_PlayChannel(-1, SHOOT_SOUND, 0);
}

void free_sound() {
    Mix_FreeChunk(REMOVE_SOUND);
    Mix_FreeChunk(SHOOT_SOUND);
    Mix_FreeMusic(MUSIC);
}
