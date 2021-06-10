#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "scene.h"
#include "sdl_wrapper.h"

void image_load();

void image_pause_screen(scene_t *scene, int *unlocked_level);

void image_free();

#endif // #ifndef __IMAGE_H__