#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "scene.h"
#include "sdl_wrapper.h"

/**
 * Loads all necessary libraries/functionality for displaying images
 */
void image_load();

/**
 * Shows pause screen
 */
void image_pause_screen(scene_t *scene, int *unlocked_level);

/**
 * Frees all images
 */
void image_free();

#endif // #ifndef __IMAGE_H__