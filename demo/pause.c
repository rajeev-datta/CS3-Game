#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <assert.h>
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
const rgb_color_t MAROON = {128.0/255, 0, 0};
const rgb_color_t BACKGROUND = {1, 1, 1};
const char WALL_INFO = 'w';
const double BUFFER = 10;
const double PAUSE_SCALE = 2.0/3;
const double PAUSE_HEIGHT = 40;
const double BUTTON_LENGTH = 500;
const double BUTTON_HEIGHT = 100;
const double INIT_POWERUP_LENGTH = 10;
const double INIT_POWERUP_HEIGHT = 10;
const double POWERUP_MASS = 50;
const int NUM_POWERUPS = 6;
const int TANK_POWER_UP_TIME = 10;
enum pause_scene{RESUME_BUT, RESTART_BUT};
enum info{PAUSE, RESUME, RESTART};

enum scene_indices{
    PAUSE_BUTTON = 0
};

enum powerups{
    MACHINE_GUN = 0,
    FRAG_BOMB = 1,
    LAND_MINE = 2,
    FORCE_FIELD = 3,
    LASER = 4,
    REMOTE_MISSLE = 5,
};

void on_key_push(char key, key_event_type_t type, double held_time,
                 void *object, scene_t *scene, bool *play) {
    if (*play) {

    }
}

bool within_rect(body_t *body, vector_t point) {
    list_t *list = body_get_shape(body);
    assert(list_size(list) == 4);
    bool within = (point.x >= ((vector_t *)list_get(list, 0))->x
                  && point.x <= ((vector_t *)list_get(list, 2))->x
                  && point.y <= ((vector_t *)list_get(list, 1))->y
                  && point.y >= ((vector_t *)list_get(list, 0))->y);

    list_free(list);
    return within;
}

void on_mouse(scene_t *scene, vector_t point, bool *play) {
    if (*play) {
        if (within_rect(scene_get_body(scene, PAUSE_BUTTON), point)){
            *play = false;
        }
    } else {
        if (within_rect(scene_get_body(scene, RESUME_BUT), point)) {
            *play = true;
        } else if (within_rect(scene_get_body(scene, RESTART_BUT), point)) {
            printf("clicked restart\n");
        }
    }
}

void make_pause_button(scene_t *scene) {
    double width = PAUSE_SCALE * PAUSE_HEIGHT;
    vector_t pause_center = {BOTTOM_LEFT_COORD.x + width/2.0 + 3 * BUFFER,
                             TOP_RIGHT_COORD.y - PAUSE_HEIGHT/2.0 - 3 * BUFFER};
    list_t *big_rect = animate_rectangle(pause_center, width, PAUSE_HEIGHT);
    int *pause_info = malloc(sizeof(int *));
    *pause_info = PAUSE;
    body_t *big_rect_body = body_init_with_info(big_rect, INFINITY, RED,
                                                pause_info, free);
    scene_add_body(scene, big_rect_body);

    list_t *inner_rect = animate_rectangle(pause_center, width/3.0, PAUSE_HEIGHT);
    body_t *inner_rect_body = body_init_with_info(inner_rect, INFINITY,
                                                  BACKGROUND, pause_info, free);
    scene_add_body(scene, inner_rect_body);
}

void set_up_pause_screen(scene_t *scene) {
    vector_t resume_center = {TOP_RIGHT_COORD.x / 2.0,
                              TOP_RIGHT_COORD.y - 1.5 * BUTTON_HEIGHT};
    list_t *resume = animate_rectangle(resume_center, BUTTON_LENGTH, BUTTON_HEIGHT);
    int *resume_info = malloc(sizeof(int *));
    *resume_info = resume;
    body_t *resume_body = body_init_with_info(resume, INFINITY, MAROON,
                                                resume_info, free);
    scene_add_body(scene, resume_body);

    vector_t restart_center = {resume_center.x,
                              resume_center.y - 2 * BUTTON_HEIGHT};
    list_t *restart = animate_rectangle(restart_center, BUTTON_LENGTH, BUTTON_HEIGHT);
    int *restart_info = malloc(sizeof(int *));
    *restart_info = restart;
    body_t *restart_body = body_init_with_info(restart, INFINITY, MAROON,
                                                restart_info, free);
    scene_add_body(scene, restart_body);
}

void make_tank_power_up(scene_t *scene, int type) {
    
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
    set_up_pause_screen(pause_scene);
    bool *play = malloc(sizeof(bool));
    *play = true;

    sdl_on_key((key_handler_t)on_key_push);
    sdl_on_mouse((mouse_handler_t)on_mouse);

    int time_passed = 0;
    scene_t *temp_scene = malloc(sizeof(scene_t *));
    temp_scene = scene;

    while (!sdl_is_done(temp_scene, scene_get_body(scene, 0), play)) {
        double dt = time_since_last_tick();
        time_passed += dt;
        if (*play) {
            temp_scene = scene;
        } else {
            temp_scene = pause_scene;
            time_passed = 0;
        }

        sdl_render_scene(temp_scene);
        
        scene_tick(temp_scene, dt);
    }
    free(play);
    scene_free(pause_scene);
    scene_free(scene);
}
