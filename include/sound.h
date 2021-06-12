#ifndef __SOUND_H__
#define __SOUND_H__

#include "sdl_wrapper.h"

/**
 * Loads all necessary libraries/functionality before playing sounds
 */
void load_sound();

/**
 * Plays remove sound when tank dies
 */
void play_remove_sound();

/**
 * Plays shooting sound when player tank shoots
 */
void play_shoot_sound();

/**
 * Frees all tank noises
 */
void free_sound();

#endif // #ifndef __SOUND_H__
