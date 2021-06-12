#ifndef __TEXT_H__
#define __TEXT_H__

#include "scene.h"
#include <stdbool.h>
#include "sdl_wrapper.h"
#include "screen_set.h"

/**
 * Opens necessary things for text to show on screen
 */
void text_open_font();

/**
 * Shows text for pause screen
 */
void text_for_pause(scene_t *scene, bool *multi, bool *choosing_level,
                    game_outcomes_t win, bool *game_started);

/**
 * Shows text for game play screen
 */
void text_for_play(scene_t *scene, bool *multi, tank_t *tank1, tank_t *tank2);

/**
 * Frees all text
 */
void text_free();

#endif // #ifndef __TEXT_H__