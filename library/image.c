#include "image.h"
#include "screen_set.h"
#include "sdl_wrapper.h"
#include "levels.h"
#include <assert.h>

static const double IMG_X_SCALE = 0.9;
static const double IMG_Y_SCALE = 0.8;
static const int LOCK_WIDTH = 70;
static const int LOCK_HEIGHT = 100;

void image_pause_screen(scene_t *scene, SDL_Surface *level1, SDL_Surface *level2,
                             SDL_Surface *level3, SDL_Surface *lock,
                             int *unlocked_level) {
    double level_width = get_top_right().x / 3.0 - get_level_buffer();
    double level_height = level_width/2;
    double image_width = level_width * IMG_X_SCALE;
    double image_height = level_height * IMG_Y_SCALE;

    list_t *easy_shape = body_get_shape(scene_get_body(scene, EASY_BUT));
    assert(list_size(easy_shape) == 4);
    int easy_x = ((vector_t *)list_get(easy_shape, 0))->x + level_width * (1 - IMG_X_SCALE)/2;
    int easy_y = ((vector_t *)list_get(easy_shape, 1))->y - level_height * (1 - IMG_Y_SCALE)/2;
    sdl_image(level1, easy_x, easy_y, image_width, image_height);

    list_t *medium_shape = body_get_shape(scene_get_body(scene, MEDIUM_BUT));
    assert(list_size(medium_shape) == 4);
    int medium_x = ((vector_t *)list_get(medium_shape, 0))->x + level_width * (1 - IMG_X_SCALE)/2;
    int medium_y = ((vector_t *)list_get(medium_shape, 1))->y - level_height * (1 - IMG_Y_SCALE)/2;
    sdl_image(level2, medium_x, medium_y, image_width, image_height);

    list_t *hard_shape = body_get_shape(scene_get_body(scene, HARD_BUT));
    assert(list_size(hard_shape) == 4);
    int hard_x = ((vector_t *)list_get(hard_shape, 0))->x + level_width * (1 - IMG_X_SCALE)/2;
    int hard_y = ((vector_t *)list_get(hard_shape, 1))->y - level_height * (1 - IMG_Y_SCALE)/2;
    sdl_image(level3, hard_x, hard_y, image_width, image_height);

    if (*unlocked_level < get_second_level()) {
        sdl_image(lock, medium_x + image_width/2 - LOCK_WIDTH/2,
                 medium_y + image_height/2 - LOCK_HEIGHT/2 - get_buffer(), LOCK_WIDTH,
                 LOCK_HEIGHT);
    }
    if (*unlocked_level < get_third_level()) {
        sdl_image(lock, hard_x + image_width/2 - LOCK_WIDTH/2,
                 hard_y + image_height/2 - LOCK_HEIGHT/2 - get_buffer(), LOCK_WIDTH,
                 LOCK_HEIGHT);
    }

    list_free(easy_shape);
    list_free(medium_shape);
    list_free(hard_shape);
}