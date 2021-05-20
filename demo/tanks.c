#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include "sdl_wrapper.h"
#include "animate.h"
#include "polygon.h"
#include "star.h"
#include "color.h"
#include "list.h"

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {1000, 500};
const double LEVEL_1_WALL_LENGTH = 300;
const double LEVEL_1_WALL_HEIGHT = 10;
const rgb_color_t RED = {1, 0, 0};
const rgb_color_t BACKGROUND = {1, 1, 1};
const char WALL_INFO = 'w';
const double BUFFER = 10;
const double PAUSE_SCALE = 2.0/3;
const double PAUSE_HEIGHT = 40;
enum info{pause};

void on_key_push(char key, key_event_type_t type, double held_time,
                 void *object, scene_t *scene, bool *play) {
    if (*play) {

    }
}

void make_pause_button(scene_t *scene) {
    double width = PAUSE_SCALE * PAUSE_HEIGHT;
    vector_t pause_center = {BOTTOM_LEFT_COORD.x + width/2 + 3 * BUFFER,
                             TOP_RIGHT_COORD.y - PAUSE_HEIGHT/2 - 3 * BUFFER};
    list_t *big_rect = animate_rectangle(pause_center, width, PAUSE_HEIGHT);
    int *pause_info = malloc(sizeof(int *));
    *pause_info = pause;
    body_t *big_rect_body = body_init_with_info(big_rect, INFINITY, RED,
                                                pause_info, free);
    scene_add_body(scene, big_rect_body);

    list_t *inner_rect = animate_rectangle(pause_center, width/3.0, PAUSE_HEIGHT);
    body_t *inner_rect_body = body_init_with_info(inner_rect, INFINITY,
                                                  BACKGROUND, pause_info, free);
    scene_add_body(scene, inner_rect_body);
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();
    /* list_t *walls = animate_level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT);
    char *wall_info = malloc(sizeof(char *));
    *wall_info = WALL_INFO;
    for (int i = 0; i < list_size(walls); i++) {
        body_t *wall_body = body_init_with_info(list_get(walls, i), INFINITY, RED, wall_info, free);
        scene_add_body(scene, list_get(walls, i));
    }
    */
    
    make_pause_button(scene);

    scene_t *pause_scene = scene_init();
    bool *play = malloc(sizeof(bool));
    *play = true;

    sdl_on_key((key_handler_t)on_key_push);

    while (!sdl_is_done(scene, scene_get_body(scene, 0), play)) {
        double dt = time_since_last_tick();
        sdl_render_scene(scene);

        // Shoot a power-up at an interval of time.
        
        scene_tick(scene, dt);
    }
    
   scene_free(scene);
}