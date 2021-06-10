#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "scene.h"
#include "sdl_wrapper.h"

void add_pause_screen_images(scene_t *scene, SDL_Surface *level1, SDL_Surface *level2,
                             SDL_Surface *level3, SDL_Surface *lock,
                             int *unlocked_level);

#endif // #ifndef __IMAGE_H__