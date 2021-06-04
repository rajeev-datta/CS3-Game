#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL_mixer.h>
#include "animate.h"
#include "polygon.h"
#include "star.h"
#include "color.h"
#include "list.h"
#include "tank.h"
#include "powerup.h"
#include "vector.h"
#include "scene.h"
#include "forces.h"

static const vector_t BOTTOM_LEFT_COORD = {0, 0};
static const vector_t TOP_RIGHT_COORD = {1000, 500};
static const double LEVEL_1_WALL_LENGTH = 10;
static const double LEVEL_1_WALL_HEIGHT = 100;
static const rgb_color_t BACKGROUND = {1, 1, 1};
static const double BUFFER = 10;
static const double PAUSE_SCALE = 2.0/3;
static const double PAUSE_HEIGHT = 40;
static const double BUTTON_LENGTH = 300;
static const double BUTTON_HEIGHT = 50;
static const double INIT_POWERUP_LENGTH = 10;
static const double INIT_POWERUP_HEIGHT = 10;
static const double POWERUP_MASS = 50;
static const int NUM_POWERUPS = 5;
static const int TANK_POWER_UP_TIME = 5;
static double const INIT_VEL = 200;
static double const ANGLE_OFFSET = (10 * M_PI)/180;
static const double ELASTICITY = 1;
static const int NUM_SCENES = 2;
static const int FONT_SIZE = 100;
static const double TEXT_SCALE = 0.8;
static const SDL_Color WHITE_TEXT = {255, 255, 255};
static const SDL_Color BLACK_TEXT = {0, 0, 0};
static const SDL_Color MAROON_TEXT = {128, 0, 0};
static const SDL_Color RED_TEXT = {255, 0, 0};
static const SDL_Color BLUE_TEXT = {0, 0, 255};
static const int LEVEL_BUFFER = 50;
static const int CIRC_PTS = 16;
static const double FORCE_FIELD_INNER_RADIUS = 60;
static const double FORCE_FIELD_OUTER_RADIUS = 70;
static const double FORCE_FIELD_MASS = 50;
static const double IMG_X_SCALE = 0.9;
static const double IMG_Y_SCALE = 0.8;
static const char *FONT = "fonts/AnticSlab-Regular.ttf";
static const int CHOOSE_LEVEL_WIDTH = 500;
static const int CHOOSE_LEVEL_HEIGHT = 100;
static const int CHOOSE_PLAYER_BOX_WIDTH = 340;
static const int CHOOSE_PLAYER_BOX_HEIGHT = 30;
static const int EASY_TEXT_WIDTH = 50;
static const int MEDIUM_TEXT_WIDTH = 90;
static const int HARD_TEXT_WIDTH = 50;
static const int LEVEL_TEXT_HEIGHT = 20;
static const vector_t TANK1_INIT_POS = {50, 250};
static const vector_t TANK2_INIT_POS = {950, 250};
static const vector_t TANK2_OFF_SCREEN = {1200, 700};
static const int FIRST_REMOVABLE_INDEX = 4;
static const int INIT_LIVES = 3;
static const int LIVES_WIDTH = 100;
static const int LIVES_HEIGHT = 50;

typedef enum pause_scene{
    RESUME_BUT,
    RESTART_BUT,
    MEDIUM_BUT,
    EASY_BUT,
    HARD_BUT,
    CHOOSE_PLAYER_BOX,
    WHITE_SCREEN
} pause_scene_t;

typedef enum scene_indices{
    PAUSE_BUTTON,
    PAUSE_BUTTON_WHITE,
    TANK1,
    TANK2
} scene_indices_t;

typedef enum scenes{
    PLAY,
    PAUSE
} scenes_t;

void put_forces(scene_t *scene) { //should work for different levels because scene is argument
    for(size_t i = 0; i < scene_bodies(scene); i++) {
        for(size_t j = 0; j < scene_bodies(scene); j++) {
            if(*(body_types_t *) body_get_info(scene_get_body(scene, i)) == BULLET) {
                if(*(body_types_t *) body_get_info(scene_get_body(scene, j)) == TANK_1
                || *(body_types_t *) body_get_info(scene_get_body(scene, j)) == TANK_2
                || *(body_types_t *) body_get_info(scene_get_body(scene, j)) == ENEMY_TANK) {
                    //bullet disappears because tanks have lives, so they survive
                    //need to write code to check for lives
                    create_partial_destructive_collision(scene, scene_get_body(scene, j), scene_get_body(scene, i));
                }
                else if(*(body_types_t *) body_get_info(scene_get_body(scene, j)) == WALL) {
                    create_physics_collision(scene, ELASTICITY, scene_get_body(scene, i), scene_get_body(scene, j));
                }
            }
            if(*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_1
            || *(body_types_t *) body_get_info(scene_get_body(scene, j)) == TANK_2
            || *(body_types_t *) body_get_info(scene_get_body(scene, j)) == ENEMY_TANK) {
                if(*(body_types_t *) body_get_info(scene_get_body(scene, j)) == WALL) {
                    create_physics_collision(scene, ELASTICITY, scene_get_body(scene, i), scene_get_body(scene, j));
                }
            }
        }
    }
}

void add_rect_to_scene(scene_t *scene, vector_t center, int width, int height,
                       int information, rgb_color_t color) {
    list_t *rect = animate_rectangle(center, width, height);
    scene_indices_t *info = malloc(sizeof(scene_indices_t));
    *info = information;
    body_t *rect_body = body_init_with_info(rect, INFINITY, color, info, free);
    body_set_is_pause_button(rect_body, true);
    scene_add_body(scene, rect_body);
}

void make_pause_button(scene_t *scene) {
    double width = PAUSE_SCALE * PAUSE_HEIGHT;
    vector_t pause_center = {BOTTOM_LEFT_COORD.x + width/2.0 + 3 * BUFFER,
                             TOP_RIGHT_COORD.y - PAUSE_HEIGHT/2.0 - 3 * BUFFER};
    add_rect_to_scene(scene, pause_center, width, PAUSE_HEIGHT, PAUSE_BUTTON, color_get_red());
    add_rect_to_scene(scene, pause_center, width/3.0, PAUSE_HEIGHT, PAUSE_BUTTON_WHITE, BACKGROUND);
}

void on_key_push(char key, key_event_type_t type, double held_time,
                 void *object, scene_t *scene, bool *play, bool *multi,
                 tank_t *tank1, tank_t *tank2) {
    if (*play) {
        body_t *tank1_body = tank_get_body(tank1);

        // for (size_t i=0; i < scene_bodies(scene); i++) {
        //     if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_1) {
        //         tank1_body = scene_get_body(scene, i);
        //     }
        // }

        if (tank_get_weapon(tank1) == (shooting_handler_t) remote_missile_shoot) {
            body_set_velocity(tank1_body, (vector_t){0, 0});
            bool missile_exists = false;
            for (size_t i=0; i < scene_bodies(scene); i++) {
                if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_REMOTE_MISSILE) {
                    missile_exists = true;
                    body_t *missile = scene_get_body(scene, i);

                    if (type == KEY_RELEASED) {
                        body_set_velocity(missile, (vector_t){0, 0});
                    } else if (type == KEY_PRESSED) {
                        vector_t speed = {0, 0};
                        switch (key) {
                            case DOWN_ARROW:
                                speed.x = -INIT_VEL * cos(body_get_orientation(missile));
                                speed.y = -INIT_VEL * sin(body_get_orientation(missile));
                                break;
                            case UP_ARROW:
                                speed.x = INIT_VEL * cos(body_get_orientation(missile));
                                speed.y = INIT_VEL * sin(body_get_orientation(missile));
                                break;
                            case RIGHT_ARROW:
                                body_set_rotation(missile, body_get_orientation(missile) + ANGLE_OFFSET);
                                break;
                            case LEFT_ARROW:
                                body_set_rotation(missile, body_get_orientation(missile) - ANGLE_OFFSET);
                                break;
                        }
                        body_set_velocity(missile, speed);
                    }
                }
            }

            if (!missile_exists) {
                tank_shoot(scene, tank1);
            }
        }
        else {
            if (type == KEY_RELEASED) {
                body_set_velocity(tank1_body, (vector_t){0, 0});
            } else if (type == KEY_PRESSED) {
                vector_t speed = {0, 0};
                switch (key) {
                    case DOWN_ARROW:
                        speed.x = -INIT_VEL * cos(body_get_orientation(tank1_body));
                        speed.y = -INIT_VEL * sin(body_get_orientation(tank1_body));
                        break;
                    case UP_ARROW:
                        speed.x = INIT_VEL * cos(body_get_orientation(tank1_body));
                        speed.y = INIT_VEL * sin(body_get_orientation(tank1_body));
                        break;
                    case RIGHT_ARROW:
                        body_set_rotation(tank1_body, body_get_orientation(tank1_body) + ANGLE_OFFSET);
                        break;
                    case LEFT_ARROW:
                        body_set_rotation(tank1_body, body_get_orientation(tank1_body) - ANGLE_OFFSET);
                        break;
                    case ' ':
                        if (body_get_time(tank1_body) > tank_get_curr_reload(tank1)) {
                            tank_shoot(scene, tank1);
                            tank_set_body_time(tank1, 0);
                        }
                        break;
                }
                body_set_velocity(tank1_body, speed);
            }
        }
    }
}

tank_t *add_tank_to_scene(scene_t *scene, body_types_t info, vector_t center) {
    body_types_t *tank_info = malloc(sizeof(body_types_t *));
    *tank_info = info;
    vector_t *tank_center = malloc(sizeof(vector_t));
    *tank_center = center;
    list_t *tank_points = animate_tank(tank_center);
    tank_t *tank = tank_init(tank_points, tank_info);
    body_set_is_tank(tank_get_body(tank), true);
    scene_add_body(scene, tank_get_body(tank));
    return tank;
}

void add_enemy_to_scene(scene_t *scene, vector_t *center) {
    body_types_t *enemy_tank_info = malloc(sizeof(body_types_t *));
    *enemy_tank_info = ENEMY_TANK;
    vector_t *speed = malloc(sizeof(vector_t));
    *speed = (vector_t) {0, 150};
    list_t *enemy_tank_points = animate_tank(center);
    tank_t *enemy_tank = enemy_tank_init(enemy_tank_points, speed, enemy_tank_info);
    scene_add_body(scene, tank_get_body(enemy_tank));
    
}

void level_1(vector_t top_right, double wall_length, double wall_height, scene_t *scene) {
    vector_t *enemy_center = malloc(sizeof(vector_t));
    *enemy_center = {900, TOP_RIGHT_COORD.y/2};
    add_enemy_to_scene(scene, enemy_center);

    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    *wall_info = WALL;
    list_t *center_wall = animate_rectangle((vector_t) {top_right.x/2, top_right.y/2}, wall_length, wall_height*2);
    body_t *center_wall_body = body_init_with_info(center_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, center_wall_body);

    list_t *left_top_wall = animate_rectangle((vector_t) {top_right.x/4, top_right.y - (wall_height/2)}, wall_length, wall_height);
    body_t *left_top_wall_body = body_init_with_info(left_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_top_wall_body);

    list_t *left_bottom_wall = animate_rectangle((vector_t) {top_right.x/4, wall_height/2}, wall_length, wall_height);
    body_t *left_bottom_wall_body = body_init_with_info(left_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_bottom_wall_body);

    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*3)/4, top_right.y - (wall_height/2)}, wall_length, wall_height);
    body_t *right_top_wall_body = body_init_with_info(right_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_top_wall_body);

    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/4, wall_height/2}, wall_length, wall_height);
    body_t *right_bottom_wall_body = body_init_with_info(right_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_bottom_wall_body);

    put_forces(scene);
}

void level_2(vector_t top_right, double wall_length, double wall_height, scene_t *scene) {
    vector_t *enemy_center = malloc(sizeof(vector_t));
    *enemy_center = {300, TOP_RIGHT_COORD.y/2};
    add_enemy_to_scene(scene, enemy_center);

    vector_t *enemy2_center = malloc(sizeof(vector_t));
    *enemy2_center = {600, TOP_RIGHT_COORD.y/2};
    add_enemy_to_scene(scene, enemy2_center);
    
    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    *wall_info = WALL;
    list_t *center_top_wall = animate_rectangle((vector_t) {top_right.x/2, (top_right.y*3.5)/10}, wall_height, wall_length);
    body_t *center_top_wall_body = body_init_with_info(center_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, center_top_wall_body);

    list_t *center_bottom_wall = animate_rectangle((vector_t) {top_right.x/2, (top_right.y*6.5)/10}, wall_height, wall_length);
    body_t *center_bottom_wall_body = body_init_with_info(center_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, center_bottom_wall_body);

    list_t *left_top_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y*4)/5}, wall_height, wall_length);
    body_t *left_top_wall_body = body_init_with_info(left_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_top_wall_body);

    list_t *left_center_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y)/2}, wall_length, wall_height);
    body_t *left_center_wall_body = body_init_with_info(left_center_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_center_wall_body);

    list_t *left_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y)/5}, wall_height, wall_length);
    body_t *left_bottom_wall_body = body_init_with_info(left_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_bottom_wall_body);

    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y*4)/5}, wall_height, wall_length);
    body_t *right_top_wall_body = body_init_with_info(right_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_top_wall_body);

    list_t *right_center_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y)/2}, wall_length, wall_height);
    body_t *right_center_wall_body = body_init_with_info(right_center_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_center_wall_body);

    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y)/5}, wall_height, wall_length);
    body_t *right_bottom_wall_body = body_init_with_info(right_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_bottom_wall_body);

    put_forces(scene);
}

void level_3(vector_t top_right, double wall_length, double wall_height, scene_t *scene) {
    vector_t *enemy_center = malloc(sizeof(vector_t));
    *enemy_center = {250, TOP_RIGHT_COORD.y/2};
    add_enemy_to_scene(scene, enemy_center);

    vector_t *enemy2_center = malloc(sizeof(vector_t));
    *enemy2_center = {550, TOP_RIGHT_COORD.y/2};
    add_enemy_to_scene(scene, enemy2_center);
    
    vector_t *enemy3_center = malloc(sizeof(vector_t));
    *enemy3_center = {900, TOP_RIGHT_COORD.y/2};
    add_enemy_to_scene(scene, enemy3_center);
    
    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    *wall_info = WALL;
    list_t *left_center_top_wall = animate_rectangle((vector_t) {(top_right.x*7)/20, (top_right.y*3)/4}, wall_length, wall_height*2);
    body_t *left_center_top_wall_body = body_init_with_info(left_center_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_center_top_wall_body);
    // create_physics_collision(scene, ELASTICITY, tank_body, center_top_wall_body);

    list_t *left_center_bottom_wall = animate_rectangle((vector_t) {(top_right.x*7)/20, (top_right.y)/4}, wall_length, wall_height);
    body_t *left_center_bottom_wall_body = body_init_with_info(left_center_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_center_bottom_wall_body);
    // create_physics_collision(scene, ELASTICITY, tank_body, center_bottom_wall_body);

    list_t *right_center_top_wall = animate_rectangle((vector_t) {(top_right.x*13)/20, (top_right.y*3)/4}, wall_length, wall_height);
    body_t *right_center_top_wall_body = body_init_with_info(right_center_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_center_top_wall_body);

    list_t *right_center_bottom_wall = animate_rectangle((vector_t) {(top_right.x*13)/20, (top_right.y)/4}, wall_length, wall_height*2);
    body_t *right_center_bottom_wall_body = body_init_with_info(right_center_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_center_bottom_wall_body);

    list_t *center_wall = animate_rectangle((vector_t) {(top_right.x)/2, (top_right.y)/2}, wall_length, wall_height*3.5);
    body_t *center_wall_body = body_init_with_info(center_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, center_wall_body);

    list_t *left_top_wall = animate_rectangle((vector_t) {(top_right.x)*3/20, (top_right.y*9)/10}, wall_length, wall_height);
    body_t *left_top_wall_body = body_init_with_info(left_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_top_wall_body);
    // create_physics_collision(scene, ELASTICITY, tank_body, left_top_wall_body);

    list_t *left_center_wall = animate_rectangle((vector_t) {(top_right.x*3)/20, (top_right.y)/2}, wall_length, wall_height);
    body_t *left_center_wall_body = body_init_with_info(left_center_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_center_wall_body);
    // create_physics_collision(scene, ELASTICITY, tank_body, left_center_wall_body);

    list_t *left_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/20, (top_right.y)/10}, wall_length, wall_height);
    body_t *left_bottom_wall_body = body_init_with_info(left_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, left_bottom_wall_body);
    // create_physics_collision(scene, ELASTICITY, tank_body, left_bottom_wall_body);

    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*17)/20, (top_right.y*9)/10}, wall_length, wall_height);
    body_t *right_top_wall_body = body_init_with_info(right_top_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_top_wall_body);
    // create_physics_collision(scene, ELASTICITY, tank_body, right_top_wall_body);

    list_t *right_center_wall = animate_rectangle((vector_t) {(top_right.x*17)/20, (top_right.y)/2}, wall_length, wall_height);
    body_t *right_center_wall_body = body_init_with_info(right_center_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_center_wall_body);
    // create_physics_collision(scene, ELASTICITY, tank_body, right_center_wall_body);

    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*17)/20, (top_right.y)/10}, wall_length, wall_height);
    body_t *right_bottom_wall_body = body_init_with_info(right_bottom_wall, INFINITY, color_get_red(), wall_info, free);
    scene_add_body(scene, right_bottom_wall_body);
    // create_physics_collision(scene, ELASTICITY, tank_body, right_bottom_wall_body);
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

void place_tanks(scene_t *scene, bool *multi) {
    body_set_centroid(scene_get_body(scene, TANK1), TANK1_INIT_POS);
    body_set_rotation(scene_get_body(scene, TANK1), 0);
    body_set_lives(scene_get_body(scene, TANK1), INIT_LIVES);
    if (*multi) {
        body_set_centroid(scene_get_body(scene, TANK2), TANK2_INIT_POS);
        body_set_rotation(scene_get_body(scene, TANK2), M_PI);
        body_set_lives(scene_get_body(scene, TANK2), INIT_LIVES);
    } else {
        body_set_centroid(scene_get_body(scene, TANK2), TANK2_OFF_SCREEN);
    }
}

void on_mouse(scene_t *scene, vector_t point, bool *play, scene_t **scenes, int *level,
              bool *multi, bool *choosing_level) {
    if (*play) {
        if (within_rect(scene_get_body(scenes[PLAY], PAUSE_BUTTON), point)){
            *play = false;
        }
    } else {
        if (!(*choosing_level) && within_rect(scene_get_body(scenes[PAUSE], RESUME_BUT), point)) {
            *play = true;
        } else if (!(*choosing_level) && within_rect(scene_get_body(scenes[PAUSE], RESTART_BUT), point)) {
            printf("clicked restart\n");
            scene_erase_some(scenes[PLAY], FIRST_REMOVABLE_INDEX);
            place_tanks(scenes[PLAY], multi);
            if (*level == 1) {
                level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[PLAY]);
            } else if (*level == 2) {
                level_2(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[PLAY]);
            } else if (*level == 3) {
                level_3(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[PLAY]);
            }
            *play = true;
        } else if (within_rect(scene_get_body(scenes[PAUSE], EASY_BUT), point)) {
            printf("clicked easy\n");
            if (*choosing_level) {
                scene_remove_body(scene, WHITE_SCREEN);
                *choosing_level = false;
            }
            scene_erase_some(scenes[PLAY], FIRST_REMOVABLE_INDEX);
            place_tanks(scenes[PLAY], multi);
            level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[PLAY]);
            *level = 1;
            *play = true;
        } else if (within_rect(scene_get_body(scenes[PAUSE], MEDIUM_BUT), point)) {
            printf("clicked medium\n");
            if (*choosing_level) {
                scene_remove_body(scene, WHITE_SCREEN);
                *choosing_level = false;
            }
            scene_erase_some(scenes[PLAY], FIRST_REMOVABLE_INDEX);
            place_tanks(scenes[PLAY], multi);
            level_2(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[PLAY]);
            *level = 2;
            *play = true;
        } else if (within_rect(scene_get_body(scenes[PAUSE], HARD_BUT), point)) {
            printf("clicked hard\n");
            if (*choosing_level) {
                scene_remove_body(scene, WHITE_SCREEN);
                *choosing_level = false;
            }
            scene_erase_some(scenes[PLAY], FIRST_REMOVABLE_INDEX);
            place_tanks(scenes[PLAY], multi);
            level_3(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[PLAY]);
            *level = 3;
            *play = true;
        } else if (!(*choosing_level) && within_rect(scene_get_body(scenes[PAUSE], CHOOSE_PLAYER_BOX), point)) {
            *multi = !(*multi);
            *choosing_level = true;
            vector_t center = {TOP_RIGHT_COORD.x/2.0, 3*TOP_RIGHT_COORD.y/4.0};
            add_rect_to_scene(scene, center, TOP_RIGHT_COORD.x,
                              TOP_RIGHT_COORD.y/2, WHITE_SCREEN, color_get_white());
        }
    }
}

void set_up_pause_screen(scene_t *scene) {
    vector_t resume_center = {TOP_RIGHT_COORD.x / 2.0,
                              TOP_RIGHT_COORD.y - 1.5 * BUTTON_HEIGHT};
    add_rect_to_scene(scene, resume_center, BUTTON_LENGTH, BUTTON_HEIGHT, RESUME_BUT,
                      color_get_maroon());
    vector_t restart_center = {resume_center.x,
                              resume_center.y - 1.5 * BUTTON_HEIGHT};
    add_rect_to_scene(scene, restart_center, BUTTON_LENGTH, BUTTON_HEIGHT, RESTART_BUT,
                      color_get_maroon());

    double level_width = TOP_RIGHT_COORD.x / 3.0 - LEVEL_BUFFER;
    double level_height = level_width/2;
    vector_t medium_center = {TOP_RIGHT_COORD.x / 2.0, TOP_RIGHT_COORD.y/2 - 2.5 * BUTTON_HEIGHT};
    add_rect_to_scene(scene, medium_center, level_width, level_height, MEDIUM_BUT,
                      color_get_maroon());
    vector_t easy_center = {medium_center.x - LEVEL_BUFFER - level_width, medium_center.y};
    add_rect_to_scene(scene, easy_center, level_width, level_height, EASY_BUT,
                      color_get_maroon());
    vector_t hard_center = {medium_center.x + LEVEL_BUFFER + level_width, medium_center.y};
    add_rect_to_scene(scene, hard_center, level_width, level_height, HARD_BUT,
                      color_get_maroon());

    vector_t player_selection_center = {TOP_RIGHT_COORD.x / 2.0,
                                    restart_center.y - 1.5 * BUTTON_HEIGHT};
    add_rect_to_scene(scene, player_selection_center, CHOOSE_PLAYER_BOX_WIDTH,
                      CHOOSE_PLAYER_BOX_HEIGHT, CHOOSE_PLAYER_BOX, color_get_maroon());
}

void add_pause_screen_text(scene_t *scene, bool *multi, TTF_Font *font,
                           bool *choosing_level) {
    list_t *resume_shape = body_get_shape(scene_get_body(scene, RESUME_BUT));
    assert(list_size(resume_shape) == 4);
    int x = ((vector_t *)list_get(resume_shape, 0))->x + BUTTON_LENGTH * (1 - TEXT_SCALE)/2;
    int y = ((vector_t *)list_get(resume_shape, 1))->y - BUTTON_HEIGHT * (1 - TEXT_SCALE)/2;
    int width = BUTTON_LENGTH * TEXT_SCALE;
    int height = BUTTON_HEIGHT * TEXT_SCALE;
    char *resume_text = "Resume";
    sdl_write(x, y, width, height, font, WHITE_TEXT, resume_text);

    list_t *restart_shape = body_get_shape(scene_get_body(scene, RESTART_BUT));
    assert(list_size(restart_shape) == 4);
    y = ((vector_t *)list_get(restart_shape, 1))->y - BUTTON_HEIGHT * (1 - TEXT_SCALE)/2;
    char *restart_text = "Restart";
    sdl_write(x, y, width, height, font, WHITE_TEXT, restart_text);

    char *medium_text = "MEDIUM";
    int medium_x = (TOP_RIGHT_COORD.x - MEDIUM_TEXT_WIDTH) / 2.0;
    int levels_y = (TOP_RIGHT_COORD.y - BUTTON_HEIGHT) / 2.0;
    sdl_write(medium_x,levels_y, MEDIUM_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, font,
              BLACK_TEXT, medium_text);

    char *easy_text = "EASY";
    int easy_x = medium_x - (TOP_RIGHT_COORD.x - LEVEL_BUFFER)/3.0;
    sdl_write(easy_x,levels_y, EASY_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, font,
              BLACK_TEXT, easy_text);

    char *hard_text = "HARD";
    int hard_x = medium_x + (TOP_RIGHT_COORD.x + HARD_TEXT_WIDTH)/3.0;
    sdl_write(hard_x,levels_y, HARD_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, font,
              BLACK_TEXT, hard_text);

    list_t *choose_players_shape = body_get_shape(scene_get_body(scene, CHOOSE_PLAYER_BOX));
    assert(list_size(restart_shape) == 4);
    double cp_x = ((vector_t *)list_get(choose_players_shape, 0))->x + BUFFER;
    double cp_y = ((vector_t *)list_get(choose_players_shape, 1))->y;
    if (*multi) {
        char *cp_text = "Switch to Single Player";
        sdl_write(cp_x, cp_y, CHOOSE_PLAYER_BOX_WIDTH - 2* BUFFER, CHOOSE_PLAYER_BOX_HEIGHT,
                  font, WHITE_TEXT, cp_text);
    } else {
        char *cp_text = "Switch to Multiplayer";
        sdl_write(cp_x, cp_y, CHOOSE_PLAYER_BOX_WIDTH - 2* BUFFER, CHOOSE_PLAYER_BOX_HEIGHT,
                  font, WHITE_TEXT, cp_text);
    }

    if (*choosing_level) {
        char *text = "Choose Your Level:";
        sdl_write(TOP_RIGHT_COORD.x/2 - CHOOSE_LEVEL_WIDTH/2,
                  TOP_RIGHT_COORD.y - CHOOSE_LEVEL_HEIGHT/2,
                  CHOOSE_LEVEL_WIDTH, CHOOSE_LEVEL_HEIGHT, font, MAROON_TEXT, text);
    }

    list_free(resume_shape);
    list_free(restart_shape);
    list_free(choose_players_shape);
}

void add_pause_screen_images(scene_t *scene, SDL_Surface *level1, SDL_Surface *level2,
                             SDL_Surface *level3) {
    double level_width = TOP_RIGHT_COORD.x / 3.0 - LEVEL_BUFFER;
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

    list_free(easy_shape);
    list_free(medium_shape);
    list_free(hard_shape);
}

void add_play_screen_text(scene_t *scene, bool *multi, TTF_Font *font, tank_t *tank1,
                          tank_t *tank2) {
    char *tank1_lives_text;
    int tank1_lives = body_get_lives(tank_get_body(tank1));
    if (tank1_lives == 0) {
        tank1_lives_text = "Lives: 0";
    } else if (tank1_lives == 1) {
        tank1_lives_text = "Lives: 1";
    } else if (tank1_lives == 2) {
        tank1_lives_text = "Lives: 2";
    } else {
        tank1_lives_text = "Lives: 3";
    }
    int x = TOP_RIGHT_COORD.x/2 - LIVES_WIDTH/2;
    int y1 = TOP_RIGHT_COORD.y - BUFFER;
    sdl_write(x, y1, LIVES_WIDTH, LIVES_HEIGHT, font,
              RED_TEXT, tank1_lives_text);
    
    if (*multi) {
        char *tank2_lives_text;
        int tank2_lives = body_get_lives(tank_get_body(tank2));
        if (tank2_lives == 0) {
            tank2_lives_text = "Lives: 0";
        } else if (tank2_lives == 1) {
            tank2_lives_text = "Lives: 1";
        } else if (tank2_lives == 2) {
            tank2_lives_text = "Lives: 2";
        } else {
            tank2_lives_text = "Lives: 3";
        }
        int y2 = BOTTOM_LEFT_COORD.y + BUFFER + LIVES_HEIGHT;
        sdl_write(x, y2, LIVES_WIDTH, LIVES_HEIGHT, font,
                BLUE_TEXT, tank2_lives_text);
    }
}

void make_tank_power_up(scene_t *scene, int type, tank_t * tank) {
    rgb_color_t color;
    powerups_t powerup_type;
    if (type == 0) {
        powerup_type = MACHINE_GUN;
        color = (rgb_color_t) {0.0, 0.0, 0.0};
    } else if (type == 1) {
        powerup_type = FRAG_BOMB;
        color = (rgb_color_t) {0.0, 0.5, 0.0};
    } else if (type == 2) {
        powerup_type = LAND_MINE;
        color = (rgb_color_t) {0.0, 0.0, 0.5};
    } else if (type == 3) {
        powerup_type = FORCE_FIELD;
        color = (rgb_color_t) {0.3, 0.0, 0.4};
    }
    else {
        powerup_type = REMOTE_MISSILE;
        color = (rgb_color_t) {165.0/255, 104.0/255, 42.0/255};
    }
    vector_t power_up_center = {rand() % (int)TOP_RIGHT_COORD.x,
                                rand() % (int)TOP_RIGHT_COORD.y};
    list_t *power_up = animate_rectangle(power_up_center, INIT_POWERUP_LENGTH, INIT_POWERUP_HEIGHT);
    powerups_t *type_pt = malloc(sizeof(powerups_t));
    *type_pt = powerup_type;
    body_t *power_up_body = body_init_with_info(power_up, POWERUP_MASS, color, type_pt, free);
    body_set_velocity(power_up_body, (vector_t) {0, 0});
    body_set_is_powerup(power_up_body, true);
    scene_add_body(scene, power_up_body);

    create_tank_powerup_collision(scene, tank, power_up_body, type);
    create_partial_destructive_collision(scene, tank_get_body(tank), power_up_body);

    // create_tank_powerup_collision(scene, scene_get_body(scene, 1), power_up_body, type);
    // create_partial_destructive_collision(scene, scene_get_body(scene, 1), power_up_body);
}

void update_and_check_projectiles_and_tanks(scene_t *scene, tank_t *tank, double dt) {
    double curr_range = tank_get_curr_range(tank);
    
    for (size_t i=0; i < scene_bodies(scene); i++) {
        if (!body_is_powerup(scene_get_body(scene, i)) && !body_is_pause_button(scene_get_body(scene, i)) &&
            !body_is_tank(scene_get_body(scene, i))) {
            if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == BULLET) {
                body_increase_time(scene_get_body(scene, i), dt);
                double curr_time = body_get_time(scene_get_body(scene, i));

                if (curr_time > curr_range) {
                    // printf("remove bullet\n");
                    body_remove(scene_get_body(scene, i));
                }
            }

            // if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_1 ||
            //     *(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_2) {
            //     body_increase_time(scene_get_body(scene, i), dt);
            // }

            if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_FRAG_BOMB) {
                body_increase_time(scene_get_body(scene, i), dt);
                double curr_time = body_get_time(scene_get_body(scene, i));

                if (curr_time > curr_range) {
                    scene_body_detonate(scene, scene_get_body(scene, i));
                }
            }

            if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_LAND_MINE) {
                body_increase_time(scene_get_body(scene, i), dt);
                double curr_time = body_get_time(scene_get_body(scene, i));

                if (curr_time > curr_range) {
                    // printf("remove land_mine\n");
                    body_remove(scene_get_body(scene, i));
                }
            }

            if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_REMOTE_MISSILE) {
                body_increase_time(scene_get_body(scene, i), dt);
                double curr_time = body_get_time(scene_get_body(scene, i));

                if (curr_time > curr_range) {
                    // printf("remove missile");
                    body_remove(scene_get_body(scene, i));
                }
            }
        } else {
            if (body_is_tank(scene_get_body(scene, i))) {
                body_increase_time(scene_get_body(scene, i), dt);
            }
        }
    }

    if (tank_get_weapon(tank) != NULL) {
                tank_increase_powerup_time(tank, dt);
                double curr_time = tank_get_curr_powerup_time(tank);

                if (curr_time > tank_get_total_powerup_time(tank)) {
                    tank_set_shooting_handler(tank, NULL);
                    tank_set_powerup_time(tank, 0);
                }
            }
}

// void check_bullet_ranges(scene_t *scene, tank_t *tank) {
//     double curr_range = tank_get_curr_range(tank);
    
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == BULLET) {
//             double curr_time = body_get_time(scene_get_body(scene, i));

//             if (curr_time > curr_range) {
//                 body_remove(scene_get_body(scene, i));
//             }
//         }
//     }
// }

// void update_bullet_times(scene_t *scene, double dt) {
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == BULLET) {
//             body_increase_time(scene_get_body(scene, i), dt);
//         }
//     }
// }

// void update_tank_times(scene_t *scene, double dt) {
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_1 ||
//             *(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_2) {
//             body_increase_time(scene_get_body(scene, i), dt);
//         }
//     }
// }

// void check_powerup_time(tank_t *tank, double dt) {
//     if (tank_get_weapon(tank) != NULL) {
//         tank_increase_powerup_time(tank, dt);
//         double curr_time = tank_get_curr_powerup_time(tank);

//         if (curr_time > tank_get_total_powerup_time(tank)) {
//             tank_set_shooting_handler(tank, NULL);
//             tank_set_powerup_time(tank, 0);
//         }
//     }
// }

// void check_detonation(scene_t *scene, tank_t *tank, double dt) {
//     double curr_range = tank_get_curr_range(tank);
    
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == FRAG_BOMB) {
//             body_increase_time(scene_get_body(scene, i), dt);
//             double curr_time = body_get_time(scene_get_body(scene, i));

//             if (curr_time > curr_range) {
//                 scene_body_detonate(scene, scene_get_body(scene, i));
//             }
//         }
//     }
// }

// void update_land_mine_times(scene_t *scene, double dt) {
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == LAND_MINE) {
//             body_increase_time(scene_get_body(scene, i), dt);
//         }
//     }
// }

// void check_land_mine_times(scene_t *scene, tank_t *tank) {
//     double curr_range = tank_get_curr_range(tank);
    
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == LAND_MINE) {
//             double curr_time = body_get_time(scene_get_body(scene, i));

//             if (curr_time > curr_range) {
//                 body_remove(scene_get_body(scene, i));
//             }
//         }
//     }
// }

// void update_missile_times(scene_t *scene, double dt) {
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_REMOTE_MISSILE) {
//             body_increase_time(scene_get_body(scene, i), dt);
//         }
//     }
// }

// void check_missile_times(scene_t *scene, tank_t *tank) {
//     double curr_range = tank_get_curr_range(tank);
    
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_REMOTE_MISSILE) {
//             double curr_time = body_get_time(scene_get_body(scene, i));

//             if (curr_time > curr_range) {
//                 body_remove(scene_get_body(scene, i));
//             }
//         }
//     }
// }

// bool check_for_force_field(scene_t *scene) {
//     for (size_t i=0; i < scene_bodies(scene); i++) {
//         if (!body_is_powerup(scene_get_body(scene, 1))) {
//             if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_FORCE_FIELD) {
//                 return true;
//             }
//         }
//     }
//     return false;
// }

body_t *create_new_force_field(scene_t *scene, tank_t *tank) {
    body_t *tank_body = tank_get_body(tank);
    list_t *force_field_pts = animate_ring(body_get_centroid(tank_body), FORCE_FIELD_INNER_RADIUS, FORCE_FIELD_OUTER_RADIUS, CIRC_PTS);

    body_types_t *tank_force_field_info = malloc(sizeof(body_types_t *));
    *tank_force_field_info = TANK_FORCE_FIELD;

    body_t *force_field_body = body_init_with_info(force_field_pts, FORCE_FIELD_MASS,
                                              color_get_blue(), tank_force_field_info, free);

    body_set_velocity(force_field_body, (vector_t) {0, 0});
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (!body_is_powerup(scene_get_body(scene, i))) {
            if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == BULLET) {
                create_partial_destructive_collision(scene, force_field_body, scene_get_body(scene, i));
            }
        }
    }
    scene_add_body(scene, force_field_body);

    return force_field_body;
}

void handle_force_field(scene_t *scene, tank_t *tank, double dt) {
    if (tank_get_weapon(tank) == (shooting_handler_t) force_field_shoot) {
        bool force_field_exists = false;
        for (size_t i=0; i < scene_bodies(scene); i++) {
            if (!body_is_powerup(scene_get_body(scene, i))) {
                if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_FORCE_FIELD) {
                    force_field_exists = true;
                    body_t *force_field = scene_get_body(scene, i);
                    body_increase_time(force_field, dt);
                    double curr_time = body_get_time(force_field);
                    
                    if (curr_time > tank_get_curr_range(tank)) {
                        body_remove(force_field);
                    } else {
                        body_remove(force_field);
                        body_t *new_force_field = create_new_force_field(scene, tank);
                        body_set_time(new_force_field, curr_time);
                    }
                }
            }
        }
        if (!force_field_exists) {
            create_new_force_field(scene, tank);
        }
    }
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();
    make_pause_button(scene);
    tank_t *tank1 = add_tank_to_scene(scene, (body_types_t) TANK_1,
                                    TANK1_INIT_POS);
    body_set_lives(tank_get_body(tank1), INIT_LIVES);
    tank_t *tank2 = add_tank_to_scene(scene, (body_types_t) TANK_2,
                                    TANK2_OFF_SCREEN);
    level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scene);
    int *level = malloc(sizeof(int));
    *level = 1;

    scene_t *pause_scene = scene_init();
    set_up_pause_screen(pause_scene);
    bool *play = malloc(sizeof(bool));
    *play = true;
    bool *multi = malloc(sizeof(bool));
    *multi = false;
    bool *choosing_level = malloc(sizeof(bool));
    *choosing_level = false;
    TTF_Font *font = TTF_OpenFont(FONT, FONT_SIZE);
    if (!font) {
        printf("TTF_OpenFontRW: %s\n", TTF_GetError());
    }
    SDL_Surface *level1 = IMG_Load("images/level1.png");
    if (!level1) {
        printf("IMG_LoadRW: %s\n", IMG_GetError());
    }
    SDL_Surface *level2 = IMG_Load("images/level2.png");
    if (!level2) {
        printf("IMG_LoadRW: %s\n", IMG_GetError());
    }
    SDL_Surface *level3 = IMG_Load("images/level3.png");
    if (!level3) {
        printf("IMG_LoadRW: %s\n", IMG_GetError());
    }
    SDL_Surface *wall = IMG_Load("images/wall.png");
    if (!wall) {
        printf("IMG_LoadRW: %s\n", IMG_GetError());
    }
    
    sdl_on_key((key_handler_t)on_key_push);
    sdl_on_mouse((mouse_handler_t)on_mouse);

    double time_passed = 0;
    scene_t *temp_scene = malloc(sizeof(scene_t *));
    temp_scene = scene;
    scene_t **scenes = malloc(sizeof(scene_t *) * NUM_SCENES);
    scenes[PLAY] = scene;
    scenes[PAUSE] = pause_scene;

    vector_t *tank_center = malloc(sizeof(vector_t));
    vector_t center = {rand() % (int)TOP_RIGHT_COORD.x, rand() % (int)TOP_RIGHT_COORD.y};
    *tank_center = center;

    while (!sdl_is_done(temp_scene, scene_get_body(temp_scene, 0), play, scenes, level, multi,
                        choosing_level, tank1, tank2, NULL)) {
        double dt = time_since_last_tick();
        time_passed += dt;

        if (*play) {
            temp_scene = scene;

            // tank_increase_body_time(tank1, dt);

            // update_tank_times(scene, dt);

            // check_powerup_time(tank1, dt);

            // update_bullet_times(scene, dt);
            // update_land_mine_times(scene, dt);
            // update_missile_times(scene, dt);

            // check_bullet_ranges(scene, tank1);
            // check_land_mine_times(scene, tank1);
            // check_missile_times(scene, tank1);

            // check_detonation(scene, tank1, dt);

            update_and_check_projectiles_and_tanks(scene, tank1, dt);

            handle_force_field(scene, tank1, dt);
            
            side_boundary(scene, TOP_RIGHT_COORD, BOTTOM_LEFT_COORD, 25.0);
            wall_boundary(scene, tank1);

            for(size_t i = 0; i < scene_bodies(scene); i++) { 
                if(*(body_types_t *)body_get_info(scene_get_body(scene, i)) == ENEMY_TANK) {
                    tank_hit_boundary_check(scene_get_body(scene, i), BOTTOM_LEFT_COORD, TOP_RIGHT_COORD, dt);
                }
            }

            // Shoot a power-up at an interval of time.
            if (time_passed > TANK_POWER_UP_TIME) {
                // make_tank_power_up(temp_scene, rand() % NUM_POWERUPS, tank1);
                make_tank_power_up(temp_scene, 4, tank1);
                time_passed = 0;
            }
        } else {
            temp_scene = pause_scene;
            time_passed = 0;
        }

        sdl_render_scene(temp_scene);
        if (!*play) {
            add_pause_screen_text(temp_scene, multi, font, choosing_level);
            add_pause_screen_images(temp_scene, level1, level2, level3);
        } else {
            add_play_screen_text(temp_scene, multi, font, tank1, tank2);
        }
        sdl_show();
        
        scene_tick(temp_scene, dt);
    }

    SDL_FreeSurface(level1);
    SDL_FreeSurface(level2);
    SDL_FreeSurface(level3);
    SDL_FreeSurface(wall);
    TTF_CloseFont(font);
    free(play);
    free(multi);
    free(choosing_level);
    scene_free(pause_scene);
    scene_free(scene);
    free(scenes);
}
