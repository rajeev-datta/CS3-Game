#ifndef __TEXT_H__
#define __TEXT_H__

#include "scene.h"
#include <stdbool.h>
#include "sdl_wrapper.h"

void add_pause_screen_text(scene_t *scene, bool *multi, TTF_Font *font,
                           bool *choosing_level, int win, bool *game_started);

void add_play_screen_text(scene_t *scene, bool *multi, TTF_Font *font, tank_t *tank1,
                          tank_t *tank2);

#endif // #ifndef __TEXT_H__