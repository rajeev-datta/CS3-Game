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
const double NUM_POWERUPS = 6;
const int TANK_POWER_UP_TIME = 10;

enum info{pause, resume, restart};

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

void on_mouse(scene_t *scene, double x, double y, bool *play) {
    if (*play) {
        // Checks if the mouse is clicking the pause button
        if (x >= 3 * BUFFER && x <= 3 * BUFFER + PAUSE_SCALE * PAUSE_HEIGHT
            && y >= 3 * BUFFER && y <= PAUSE_HEIGHT + 3 * BUFFER) {
            *play = false;
            printf("it is now paused\n");
            return;
        }
    } else {
        if (x >= TOP_RIGHT_COORD.x/2.0 - BUTTON_LENGTH/2.0 && x <= TOP_RIGHT_COORD.x/2.0 + BUTTON_LENGTH/2.0) {
            // Checks if the mouse is clicking the resume button
            *play = true;
        }
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

void set_up_pause_screen(scene_t *scene) {
    vector_t resume_center = {TOP_RIGHT_COORD.x / 2,
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
    rgb_color_t color;
    if (type == MACHINE_GUN) {
        color = (rgb_color_t) {0.0, 0.0, 0.0};
    } else if (type == FRAG_BOMB) {
        color = (rgb_color_t) {0.0, 0.5, 0.0};
    } else if (type == LAND_MINE) {
        color = (rgb_color_t) {0.0, 0.0, 0.5};
    } else if (type == FORCE_FIELD) {
        color = (rgb_color_t) {0.3, 0.0, 0.4};
    } else if (type == LASER) {
        color = (rgb_color_t) {0.4, 0.6, 0.2};
    }
    else {
        color = (rgb_color_t) {165.0/255, 104.0/255, 42.0/255};
    }
    vector_t power_up_center = {rand() % (int)TOP_RIGHT_COORD.x,
                                rand() % (int)TOP_RIGHT_COORD.y};
    list_t *power_up = animate_rectangle(power_up_center, INIT_POWERUP_LENGTH, INIT_POWERUP_HEIGHT);
    char *type_pt = malloc(sizeof(char));
    *type_pt = (char) type;
    body_t *power_up_body = body_init_with_info(power_up, POWERUP_MASS, color, type_pt, free);
    body_set_velocity(power_up_body, (vector_t) {0, 0});
    scene_add_body(scene, power_up_body);
    assert(*(char *)body_get_info(scene_get_body(scene, 0)) == TANK_1_INFO);
    create_tank_powerup_collision(scene, scene_get_body(scene, 0), power_up_body, type);
    create_partial_destructive_collision(scene, scene_get_body(scene, 0), power_up_body);
    assert(*(char *)body_get_info(scene_get_body(scene, 1)) == TANK_2_INFO);
    create_tank_powerup_collision(scene, scene_get_body(scene, 1), power_up_body, type);
    create_partial_destructive_collision(scene, scene_get_body(scene, 1), power_up_body);
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

        // Shoot a power-up at an interval of time.
        if (time_passed > TANK_POWER_UP_TIME) {
            make_tank_power_up(temp_scene, rand() % NUM_POWER_UPS);
            time_passed = 0;
        }
        
        scene_tick(temp_scene, dt);
    }
    free(play;)
    scene_free(pause_scene);
    scene_free(scene);
}