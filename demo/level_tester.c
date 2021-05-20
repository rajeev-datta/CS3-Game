#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include "sdl_wrapper.h"
#include "animate.h"
#include "polygon.h"
#include "star.h"
#include "color.h"
#include "list.h"
#include "vector.h"
#include "collision.h"
#include "forces.h"
#include <assert.h>

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {1000, 500};
const double LEVEL_1_WALL_LENGTH = 300;
const double LEVEL_1_WALL_HEIGHT = 10;
const rgb_color_t RED = {1, 0, 0};
const char WALL_INFO = 'w';

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();
    list_t *walls = animate_level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT);
    char *wall_info = malloc(sizeof(char *));
    wall_info = WALL_INFO;
    for (int i = 0; i < list_size(walls); i++) {
        body_t *wall_body = body_init_with_info(list_get(walls, i), INFINITY, RED, wall_info, free);
        scene_add_body(scene, list_get(walls, i));
    }

    while (!sdl_is_done(scene, scene_get_body(scene, 0), NULL)) {
        double dt = time_since_last_tick();
        sdl_render_scene(scene);

        // Shoot a power-up at an interval of time.
        
        scene_tick(scene, dt);
    }
    
   scene_free(scene);
}