#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <assert.h>
#include "sdl_wrapper.h"
#include "animate.h"
#include "polygon.h"
#include "star.h"
#include "color.h"
#include "list.h"
#include "forces.h"

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {1000, 500};
const double LEVEL_1_WALL_LENGTH = 10;
const double LEVEL_1_WALL_HEIGHT = 100;
const rgb_color_t RED = {1, 0, 0};
const rgb_color_t MAROON = {128.0/255, 0, 0};
const rgb_color_t BACKGROUND = {1, 1, 1};
const char WALL_INFO = 'w';
const char TANK_INFO = 't';
const double BUFFER = 10;
const double PAUSE_SCALE = 2.0/3;
const double PAUSE_HEIGHT = 40;
const double BUTTON_LENGTH = 300;
const double BUTTON_HEIGHT = 50;
const double INIT_POWERUP_LENGTH = 10;
const double INIT_POWERUP_HEIGHT = 10;
const double POWERUP_MASS = 50;
const int NUM_POWERUPS = 6;
const int TANK_POWER_UP_TIME = 10;
double const INIT_VEL = 400;
double const ANGLE_OFFSET = (10 * M_PI)/180;
const double ELASTICITY = 1;
const int NUM_SCENES = 2;
const int FONT_SIZE = 24;
const SDL_Color BLACK_TEXT = {0, 0, 0};
typedef enum pause_scene{RESUME_BUT, RESTART_BUT} pause_scene_t;
typedef enum info{
    PAUSE,
    RESUME,
    RESTART
} info_t;

typedef enum scene_indices{
    PAUSE_BUTTON,
} scene_indices_t;

typedef enum powerups{
    MACHINE_GUN,
    FRAG_BOMB,
    LAND_MINE,
    FORCE_FIELD,
    LASER,
    REMOTE_MISSILE,
} powerups_t;

void make_pause_button(scene_t *scene) {
    double width = PAUSE_SCALE * PAUSE_HEIGHT;
    vector_t pause_center = {BOTTOM_LEFT_COORD.x + width/2.0 + 3 * BUFFER,
                             TOP_RIGHT_COORD.y - PAUSE_HEIGHT/2.0 - 3 * BUFFER};
    list_t *big_rect = animate_rectangle(pause_center, width, PAUSE_HEIGHT);
    info_t *pause_info = malloc(sizeof(info_t));
    *pause_info = PAUSE;
    body_t *big_rect_body = body_init_with_info(big_rect, INFINITY, RED,
                                                pause_info, free);
    scene_add_body(scene, big_rect_body);

    list_t *inner_rect = animate_rectangle(pause_center, width/3.0, PAUSE_HEIGHT);
    body_t *inner_rect_body = body_init_with_info(inner_rect, INFINITY,
                                                  BACKGROUND, pause_info, free);
    scene_add_body(scene, inner_rect_body);
}

void level_1(vector_t top_right, double wall_length, double wall_height, scene_t *scene) {
    char *tank_info = malloc(sizeof(char *));
    tank_info = TANK_INFO;
    list_t *tank = animate_rectangle((vector_t) {100, TOP_RIGHT_COORD.y/2}, 50, 50);
    body_t *tank_body = body_init_with_info(tank, 50, RED, tank_info, free);
    scene_add_body(scene, tank_body);

    char *wall_info = malloc(sizeof(char *));
    wall_info = WALL_INFO;
    list_t *center_wall = animate_rectangle((vector_t) {top_right.x/2, top_right.y/2}, wall_length, wall_height*2);
    body_t *center_wall_body = body_init_with_info(center_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, center_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, center_wall_body);

    list_t *left_top_wall = animate_rectangle((vector_t) {top_right.x/4, top_right.y - (wall_height/2)}, wall_length, wall_height);
    body_t *left_top_wall_body = body_init_with_info(left_top_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, left_top_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, left_top_wall_body);

    list_t *left_bottom_wall = animate_rectangle((vector_t) {top_right.x/4, wall_height/2}, wall_length, wall_height);
    body_t *left_bottom_wall_body = body_init_with_info(left_bottom_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, left_bottom_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, left_bottom_wall_body);

    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*3)/4, top_right.y - (wall_height/2)}, wall_length, wall_height);
    body_t *right_top_wall_body = body_init_with_info(right_top_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, right_top_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, right_top_wall_body);

    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/4, wall_height/2}, wall_length, wall_height);
    body_t *right_bottom_wall_body = body_init_with_info(right_bottom_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, right_bottom_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, right_bottom_wall_body);
}

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

void on_mouse(scene_t *scene, vector_t point, bool *play, scene_t **scenes) {
    if (*play) {
        if (within_rect(scene_get_body(scene, PAUSE_BUTTON), point)){
            *play = false;
        }
    } else {
        if (within_rect(scene_get_body(scene, RESUME_BUT), point)) {
            *play = true;
        } else if (within_rect(scene_get_body(scene, RESTART_BUT), point)) {
            printf("clicked restart\n");
            erase_scene(scenes[0]);
            make_pause_button(scenes[0]);
            level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            *play = true;
        }
    }
}

void add_pause_screen_text(scene_t *scene) {
    char *font = "Limelight.ttf";

    list_t *resume_shape = body_get_shape(scene_get_body(scene, RESUME_BUT));
    assert(list_size(resume_shape) == 4);
    int x = ((vector_t *)list_get(resume_shape, 0))->x;
    int y = ((vector_t *)list_get(resume_shape, 1))->y;
    int width = BUTTON_LENGTH;
    int height = BUTTON_HEIGHT;
    char *resume_text = "Resume";
    sdl_write(x, y, width, height, font, FONT_SIZE, BLACK_TEXT, resume_text);

    list_t *restart_shape = body_get_shape(scene_get_body(scene, RESTART_BUT));
    assert(list_size(restart_shape) == 4);
    y = ((vector_t *)list_get(restart_shape, 1))->y;
    char *restart_text = "Restart";
    sdl_write(x, y, width, height, font, FONT_SIZE, BLACK_TEXT, restart_text);

    list_free(resume_shape);
    list_free(restart_shape);
}

void set_up_pause_screen(scene_t *scene) {
    vector_t resume_center = {TOP_RIGHT_COORD.x / 2.0,
                              TOP_RIGHT_COORD.y - 1.5 * BUTTON_HEIGHT};
    list_t *resume = animate_rectangle(resume_center, BUTTON_LENGTH, BUTTON_HEIGHT);
    info_t *resume_info = malloc(sizeof(info_t));
    *resume_info = RESUME;
    body_t *resume_body = body_init_with_info(resume, INFINITY, MAROON,
                                                resume_info, free);
    scene_add_body(scene, resume_body);

    vector_t restart_center = {resume_center.x,
                              resume_center.y - 2 * BUTTON_HEIGHT};
    list_t *restart = animate_rectangle(restart_center, BUTTON_LENGTH, BUTTON_HEIGHT);
    info_t *restart_info = malloc(sizeof(info_t));
    *restart_info = RESTART;
    body_t *restart_body = body_init_with_info(restart, INFINITY, MAROON,
                                                restart_info, free);
    scene_add_body(scene, restart_body);
}

void make_tank_power_up(scene_t *scene, int type) {
    
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();
    make_pause_button(scene);
    level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scene);

    scene_t *pause_scene = scene_init();
    set_up_pause_screen(pause_scene);
    bool *play = malloc(sizeof(bool));
    *play = true;

    sdl_on_key((key_handler_t)on_key_push);
    sdl_on_mouse((mouse_handler_t)on_mouse);

    int time_passed = 0;
    scene_t *temp_scene = malloc(sizeof(scene_t *));
    temp_scene = scene;
    scene_t **scenes = malloc(sizeof(scene_t *) * NUM_SCENES);
    scenes[0] = scene;
    scenes[1] = pause_scene;


    while (!sdl_is_done(temp_scene, scene_get_body(scene, 0), play, scenes)) {
        double dt = time_since_last_tick();
        time_passed += dt;

        if (*play) {
            temp_scene = scene;
        } else {
            temp_scene = pause_scene;
            time_passed = 0;
        }

        sdl_render_scene(temp_scene);
        if (!*play) {
            add_pause_screen_text(temp_scene);
        }
        
        scene_tick(temp_scene, dt);
    }
    free(play);
    scene_free(pause_scene);
    scene_free(scene);
    free(scenes);
}

