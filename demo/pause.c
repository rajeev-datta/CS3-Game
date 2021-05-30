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
#include <string.h>

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
const int FONT_SIZE = 100;
const double TEXT_SCALE = 0.8;
const SDL_Color WHITE_TEXT = {255, 255, 255};
const SDL_Color BLACK_TEXT = {0, 0, 0};
const int LEVEL_BUFFER = 50;
const int EASY_TEXT_WIDTH = 50;
const int MEDIUM_TEXT_WIDTH = 90;
const int HARD_TEXT_WIDTH = 50;
const int LEVEL_TEXT_HEIGHT = 20;
const double IMG_X_SCALE = 0.9;
const double IMG_Y_SCALE = 0.8;

typedef enum pause_scene{
    RESUME_BUT,
    RESTART_BUT,
    MEDIUM_BUT,
    EASY_BUT,
    HARD_BUT
} pause_scene_t;

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

void add_rect_to_scene(scene_t *scene, vector_t center, int width, int height,
                       int information, rgb_color_t color) {
    list_t *rect = animate_rectangle(center, width, height);
    info_t *info = malloc(sizeof(info_t));
    *info = information;
    body_t *rect_body = body_init_with_info(rect, INFINITY, color, info, free);
    scene_add_body(scene, rect_body);
}

void make_pause_button(scene_t *scene) {
    double width = PAUSE_SCALE * PAUSE_HEIGHT;
    vector_t pause_center = {BOTTOM_LEFT_COORD.x + width/2.0 + 3 * BUFFER,
                             TOP_RIGHT_COORD.y - PAUSE_HEIGHT/2.0 - 3 * BUFFER};
    add_rect_to_scene(scene, pause_center, width, PAUSE_HEIGHT, PAUSE, RED);
    add_rect_to_scene(scene, pause_center, width/3.0, PAUSE_HEIGHT, PAUSE, BACKGROUND);
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

void level_2(vector_t top_right, double wall_length, double wall_height, scene_t *scene) {
    char *tank_info = malloc(sizeof(char *));
    tank_info = TANK_INFO;
    list_t *tank = animate_rectangle((vector_t) {100, TOP_RIGHT_COORD.y/2}, 50, 50);
    body_t *tank_body = body_init_with_info(tank, 50, RED, tank_info, free);
    scene_add_body(scene, tank_body);
    
    char *wall_info = malloc(sizeof(char *));
    wall_info = WALL_INFO;
    list_t *center_top_wall = animate_rectangle((vector_t) {top_right.x/2, (top_right.y*3.5)/10}, wall_height, wall_length);
    body_t *center_top_wall_body = body_init_with_info(center_top_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, center_top_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, center_top_wall_body);

    list_t *center_bottom_wall = animate_rectangle((vector_t) {top_right.x/2, (top_right.y*6.5)/10}, wall_height, wall_length);
    body_t *center_bottom_wall_body = body_init_with_info(center_bottom_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, center_bottom_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, center_bottom_wall_body);

    list_t *left_top_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y*4)/5}, wall_height, wall_length);
    body_t *left_top_wall_body = body_init_with_info(left_top_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, left_top_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, left_top_wall_body);

    list_t *left_center_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y)/2}, wall_length, wall_height);
    body_t *left_center_wall_body = body_init_with_info(left_center_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, left_center_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, left_center_wall_body);

    list_t *left_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y)/5}, wall_height, wall_length);
    body_t *left_bottom_wall_body = body_init_with_info(left_bottom_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, left_bottom_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, left_bottom_wall_body);

    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y*4)/5}, wall_height, wall_length);
    body_t *right_top_wall_body = body_init_with_info(right_top_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, right_top_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, right_top_wall_body);

    list_t *right_center_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y)/2}, wall_length, wall_height);
    body_t *right_center_wall_body = body_init_with_info(right_center_wall, INFINITY, RED, wall_info, free);
    scene_add_body(scene, right_center_wall_body);
    create_physics_collision(scene, ELASTICITY, tank_body, right_center_wall_body);

    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y)/5}, wall_height, wall_length);
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

void on_mouse(scene_t *scene, vector_t point, bool *play, scene_t **scenes, int *level) {
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
            if (*level == 1) {
                level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            } else if (*level == 2) {
                level_2(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            } else if (*level == 3) {
                printf("imagine level 3 is here\n");
                level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            }
            *play = true;
        } else if (within_rect(scene_get_body(scene, EASY_BUT), point)) {
            printf("clicked easy\n");
            erase_scene(scenes[0]);
            make_pause_button(scenes[0]);
            level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            *level = 1;
            *play = true;
        } else if (within_rect(scene_get_body(scene, MEDIUM_BUT), point)) {
            printf("clicked medium\n");
            erase_scene(scenes[0]);
            make_pause_button(scenes[0]);
            level_2(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            *level = 2;
            *play = true;
        } else if (within_rect(scene_get_body(scene, HARD_BUT), point)) {
            printf("clicked hard\n");
            *level = 3;
        }
    }
}

void add_pause_screen_text(scene_t *scene) {
    char *font = "fonts/AnticSlab-Regular.ttf";

    list_t *resume_shape = body_get_shape(scene_get_body(scene, RESUME_BUT));
    assert(list_size(resume_shape) == 4);
    int x = ((vector_t *)list_get(resume_shape, 0))->x + BUTTON_LENGTH * (1 - TEXT_SCALE)/2;
    int y = ((vector_t *)list_get(resume_shape, 1))->y - BUTTON_HEIGHT * (1 - TEXT_SCALE)/2;
    int width = BUTTON_LENGTH * TEXT_SCALE;
    int height = BUTTON_HEIGHT * TEXT_SCALE;
    char *resume_text = "Resume";
    sdl_write(x, y, width, height, font, FONT_SIZE, WHITE_TEXT, resume_text);

    list_t *restart_shape = body_get_shape(scene_get_body(scene, RESTART_BUT));
    assert(list_size(restart_shape) == 4);
    y = ((vector_t *)list_get(restart_shape, 1))->y - BUTTON_HEIGHT * (1 - TEXT_SCALE)/2;
    char *restart_text = "Restart";
    sdl_write(x, y, width, height, font, FONT_SIZE, WHITE_TEXT, restart_text);

    char *medium_text = "MEDIUM";
    int medium_x = (TOP_RIGHT_COORD.x - MEDIUM_TEXT_WIDTH) / 2.0;
    int levels_y = (TOP_RIGHT_COORD.y - BUTTON_HEIGHT) / 2.0;
    sdl_write(medium_x,levels_y, MEDIUM_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, font, FONT_SIZE,
              BLACK_TEXT, medium_text);

    char *easy_text = "EASY";
    int easy_x = medium_x - (TOP_RIGHT_COORD.x - LEVEL_BUFFER)/3.0;
    sdl_write(easy_x,levels_y, EASY_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, font, FONT_SIZE,
              BLACK_TEXT, easy_text);

    char *hard_text = "HARD";
    int hard_x = medium_x + (TOP_RIGHT_COORD.x + HARD_TEXT_WIDTH)/3.0;
    sdl_write(hard_x,levels_y, HARD_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, font, FONT_SIZE,
              BLACK_TEXT, hard_text);

    list_free(resume_shape);
    list_free(restart_shape);
}

void add_pause_screen_images(scene_t *scene) {
    double level_width = TOP_RIGHT_COORD.x / 3.0 - LEVEL_BUFFER;
    double level_height = level_width/2;
    double image_width = level_width * IMG_X_SCALE;
    double image_height = level_height * IMG_Y_SCALE;

    list_t *easy_shape = body_get_shape(scene_get_body(scene, EASY_BUT));
    assert(list_size(easy_shape) == 4);
    int easy_x = ((vector_t *)list_get(easy_shape, 0))->x + level_width * (1 - IMG_X_SCALE)/2;
    int easy_y = ((vector_t *)list_get(easy_shape, 1))->y - level_height * (1 - IMG_Y_SCALE)/2;
    char *easy_image = "images/level1.png";
    sdl_image(easy_image, easy_x, easy_y, image_width, image_height);

    list_t *medium_shape = body_get_shape(scene_get_body(scene, MEDIUM_BUT));
    assert(list_size(medium_shape) == 4);
    int medium_x = ((vector_t *)list_get(medium_shape, 0))->x + level_width * (1 - IMG_X_SCALE)/2;
    int medium_y = ((vector_t *)list_get(medium_shape, 1))->y - level_height * (1 - IMG_Y_SCALE)/2;
    char *medium_image = "images/level2.png";
    sdl_image(medium_image, medium_x, medium_y, image_width, image_height);
}

void set_up_pause_screen(scene_t *scene) {
    vector_t resume_center = {TOP_RIGHT_COORD.x / 2.0,
                              TOP_RIGHT_COORD.y - 1.5 * BUTTON_HEIGHT};
    add_rect_to_scene(scene, resume_center, BUTTON_LENGTH, BUTTON_HEIGHT, RESUME,
                      MAROON);
    vector_t restart_center = {resume_center.x,
                              resume_center.y - 2 * BUTTON_HEIGHT};
    add_rect_to_scene(scene, restart_center, BUTTON_LENGTH, BUTTON_HEIGHT, RESTART,
                      MAROON);

    double level_width = TOP_RIGHT_COORD.x / 3.0 - LEVEL_BUFFER;
    double level_height = level_width/2;
    vector_t medium_center = {TOP_RIGHT_COORD.x / 2, TOP_RIGHT_COORD.y/2 - 2.5 * BUTTON_HEIGHT};
    add_rect_to_scene(scene, medium_center, level_width, level_height, MEDIUM_BUT,
                      MAROON);
    vector_t easy_center = {medium_center.x - LEVEL_BUFFER - level_width, medium_center.y};
    add_rect_to_scene(scene, easy_center, level_width, level_height, EASY_BUT,
                      MAROON);
    vector_t hard_center = {medium_center.x + LEVEL_BUFFER + level_width, medium_center.y};
    add_rect_to_scene(scene, hard_center, level_width, level_height, HARD_BUT,
                      MAROON);
}

void make_tank_power_up(scene_t *scene, int type) {
    
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();
    make_pause_button(scene);
    level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scene);
    int *level = malloc(sizeof(int));
    *level = 1;

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


    while (!sdl_is_done(temp_scene, scene_get_body(scene, 0), play, scenes, level)) {
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
            add_pause_screen_images(temp_scene);
        }
        sdl_show();
        scene_tick(temp_scene, dt);
    }
    free(play);
    scene_free(pause_scene);
    scene_free(scene);
    free(scenes);
}

