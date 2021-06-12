#ifndef __TEXT_H__
#define __TEXT_H__

#include "scene.h"
#include <stdbool.h>
#include "sdl_wrapper.h"
#include "screen_set.h"

void text_open_font();

void text_for_pause(scene_t *scene, bool *multi, bool *choosing_level,
                    game_outcomes_t win, bool *game_started);

void text_for_play(scene_t *scene, bool *multi, tank_t *tank1, tank_t *tank2);

void text_free();

#endif // #ifndef __TEXT_H__