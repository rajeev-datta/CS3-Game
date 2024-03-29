#include "image.h"
#include "screen_set.h"
#include "sdl_wrapper.h"
#include "levels.h"
#include <assert.h>
#include "animate.h"

static const double IMG_X_SCALE = 0.9;
static const double IMG_Y_SCALE = 0.8;
static const int LOCK_WIDTH = 70;
static const int LOCK_HEIGHT = 100;
static const int HALVE = 2;
static const double THIRD = 3.0;
static const int WHOLE = 1;

static SDL_Surface *LEVEL1_SURFACE;
static SDL_Surface *LEVEL2_SURFACE;
static SDL_Surface *LEVEL3_SURFACE;
static SDL_Surface *LOCK_SURFACE;

void image_load() {
    LEVEL1_SURFACE = IMG_Load("images/level1.png");
    if (!LEVEL1_SURFACE) {
        printf("IMG_LoadRW: %s\n", IMG_GetError());
    }
    LEVEL2_SURFACE = IMG_Load("images/level2.png");
    if (!LEVEL2_SURFACE) {
        printf("IMG_LoadRW: %s\n", IMG_GetError());
    }
    LEVEL3_SURFACE = IMG_Load("images/level3.png");
    if (!LEVEL3_SURFACE) {
        printf("IMG_LoadRW: %s\n", IMG_GetError());
    }
    LOCK_SURFACE = IMG_Load("images/lock.png");
    if (!LOCK_SURFACE) {
        printf("IMG_LoadRW: %s\n", IMG_GetError());
    }
}

void image_pause_screen(scene_t *scene, int *unlocked_level) {
    double level_width = get_top_right().x / THIRD - get_level_buffer();
    double level_height = level_width/HALVE;
    double image_width = level_width * IMG_X_SCALE;
    double image_height = level_height * IMG_Y_SCALE;

    list_t *easy_shape = body_get_shape(scene_get_body(scene, EASY_BUT));
    assert(list_size(easy_shape) == get_num_rect_pts());
    int easy_x = ((vector_t *)list_get(easy_shape, BOTTOM_LEFT_CORNER))->x
                                       + level_width * (WHOLE - IMG_X_SCALE)/HALVE;
    int easy_y = ((vector_t *)list_get(easy_shape, TOP_LEFT_CORNER))->y
                                       - level_height * (WHOLE - IMG_Y_SCALE)/HALVE;
    sdl_image(LEVEL1_SURFACE, easy_x, easy_y, image_width, image_height);

    list_t *medium_shape = body_get_shape(scene_get_body(scene, MEDIUM_BUT));
    assert(list_size(medium_shape) == get_num_rect_pts());
    int medium_x = ((vector_t *)list_get(medium_shape, BOTTOM_LEFT_CORNER))->x
                                         + level_width * (WHOLE - IMG_X_SCALE)/HALVE;
    int medium_y = ((vector_t *)list_get(medium_shape, TOP_LEFT_CORNER))->y
                                         - level_height * (WHOLE - IMG_Y_SCALE)/HALVE;
    sdl_image(LEVEL2_SURFACE, medium_x, medium_y, image_width, image_height);

    list_t *hard_shape = body_get_shape(scene_get_body(scene, HARD_BUT));
    assert(list_size(hard_shape) == get_num_rect_pts());
    int hard_x = ((vector_t *)list_get(hard_shape, BOTTOM_LEFT_CORNER))->x
                                       + level_width * (WHOLE - IMG_X_SCALE)/HALVE;
    int hard_y = ((vector_t *)list_get(hard_shape, TOP_LEFT_CORNER))->y
                                       - level_height * (WHOLE - IMG_Y_SCALE)/HALVE;
    sdl_image(LEVEL3_SURFACE, hard_x, hard_y, image_width, image_height);

    if (*unlocked_level < get_second_level()) {
        sdl_image(LOCK_SURFACE, medium_x + image_width/HALVE - LOCK_WIDTH/HALVE,
                 medium_y + image_height/HALVE - LOCK_HEIGHT/HALVE - get_buffer(),
                 LOCK_WIDTH, LOCK_HEIGHT);
    }
    if (*unlocked_level < get_third_level()) {
        sdl_image(LOCK_SURFACE, hard_x + image_width/HALVE - LOCK_WIDTH/HALVE,
                 hard_y + image_height/HALVE - LOCK_HEIGHT/HALVE - get_buffer(),
                 LOCK_WIDTH, LOCK_HEIGHT);
    }

    list_free(easy_shape);
    list_free(medium_shape);
    list_free(hard_shape);
}

void image_free() {
    SDL_FreeSurface(LEVEL1_SURFACE);
    SDL_FreeSurface(LEVEL2_SURFACE);
    SDL_FreeSurface(LEVEL3_SURFACE);
    SDL_FreeSurface(LOCK_SURFACE);
}
