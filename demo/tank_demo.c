#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL_mixer.h>
#include "sdl_wrapper.h"
#include "animate.h"
#include "polygon.h"
#include "star.h"
#include "color.h"
#include "list.h"
#include "tank.h"
#include "powerup.h"
#include "vector.h"
#include "scene.h"

static const vector_t BOTTOM_LEFT_COORD = {0, 0};
static const vector_t TOP_RIGHT_COORD = {1000, 500};
static const double LEVEL_1_WALL_LENGTH = 10;
static const double LEVEL_1_WALL_HEIGHT = 100;
static const rgb_color_t BACKGROUND = {1, 1, 1};
static const double BUFFER = 10;
static const double PAUSE_SCALE = 2.0/3;
static const double PAUSE_HEIGHT = 40;
static const double BUTTON_LENGTH = 500;
static const double BUTTON_HEIGHT = 100;
static const double INIT_POWERUP_LENGTH = 10;
static const double INIT_POWERUP_HEIGHT = 10;
static const double POWERUP_MASS = 50;
static const int NUM_POWERUPS = 5;
static const int TANK_POWER_UP_TIME = 10;
static double const INIT_VEL = 400;
static double const ANGLE_OFFSET = (10 * M_PI)/180;
static const double ELASTICITY = 1;
static const int NUM_SCENES = 2;
static const int FONT_SIZE = 100;
static const double TEXT_SCALE = 0.8;
static const SDL_Color WHITE_TEXT = {255, 255, 255};
static const int LEVEL_BUFFER = 50;
static const int CIRC_PTS = 16;
static const double FORCE_FIELD_INNER_RADIUS = 7;
static const double FORCE_FIELD_OUTER_RADIUS = 8;
static const double FORCE_FIELD_MASS = 50;
static const double IMG_X_SCALE = 0.9;
static const double IMG_Y_SCALE = 0.8;
const char *FONT = "fonts/AnticSlab-Regular.ttf";
const int CHOOSE_LEVEL_WIDTH = 500;
const int CHOOSE_LEVEL_HEIGHT = 100;

typedef enum pause_scene{
    RESUME_BUT,
    RESTART_BUT,
    MEDIUM_BUT,
    EASY_BUT,
    HARD_BUT,
    CHOOSE_PLAYER_BOX,
    WHITE_SCREEN
} pause_scene_t;

typedef enum info{
    PAUSE,
    RESUME,
    RESTART
} info_t;

typedef enum scene_indices{
    PAUSE_BUTTON
};

void put_forces(scene_t *scene) { //should work for different levels because scene is argument
    for(size_t i = 0; i < scene_bodies(scene); i++) {
        for(size_t j = 0; j < scene_bodies(scene); j++) {
            if(body_get_info(scene_get_body(scene, i)) == BULLET) {
                if(body_get_info(scene_get_body(scene, j)) == TANK_1 || body_get_info(scene_get_body(scene, j)) == TANK_2
                || body_get_info(scene_get_body(scene, j)) == ENEMY_TANK) {
                    //bullet disappears because tanks have lives, so they survive
                    //need to write code to check for lives
                    create_partial_destructive_collision(scene, scene_get_body(scene, j), scene_get_body(scene, i));
                }
                else if(body_get_info(scene_get_body(scene, j)) == WALL) {
                    create_physics_collision(scene, ELASTICITY, scene_get_body(scene, i), scene_get_body(scene, j));
                }
            }
            if(body_get_info(scene_get_body(scene, i)) == TANK_1 || body_get_info(scene_get_body(scene, j)) == TANK_2
            || body_get_info(scene_get_body(scene, j)) == ENEMY_TANK) {
                if(body_get_info(scene_get_body(scene, j)) == WALL) {
                    create_physics_collision(scene, ELASTICITY, scene_get_body(scene, i), scene_get_body(scene, j));
                }
            }
        }
    }

}

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
    add_rect_to_scene(scene, pause_center, width, PAUSE_HEIGHT, PAUSE, color_get_red());
    add_rect_to_scene(scene, pause_center, width/3.0, PAUSE_HEIGHT, PAUSE, BACKGROUND);
}

void level_1(vector_t top_right, double wall_length, double wall_height, scene_t *scene) {
    body_types_t *tank_info = malloc(sizeof(body_types_t *));
    tank_info = TANK_1;
    vector_t *tank_center = malloc(sizeof(vector_t));
    vector_t center = {100, (int) TOP_RIGHT_COORD.y/2};
    *tank_center = center;
    list_t *tank_points = animate_tank(tank_center);
    tank_t *tank = tank_init(tank_points, info);
    scene_add_body(scene, tank_get_body(tank));

    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    wall_info = WALL;
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
    body_types_t *tank_info = malloc(sizeof(body_types_t *));
    tank_info = TANK_1;
    list_t *tank_points = animate_tank((vector_t) {100, TOP_RIGHT_COORD.y/2});
    tank_t *tank = tank_init(tank_points, info);
    scene_add_body(scene, tank_get_body(tank));
    
    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    wall_info = WALL;
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

// how to include a time constraint on how often space can be used to shoot
// can find the reload time from tank, but not sure how to get the time between key pushes

void on_key_push(char key, key_event_type_t type, double held_time,
                 void *object, scene_t *scene, bool *play) {
    if (*play) {
        double curr_rot = 0;
        if (type == KEY_RELEASED) {
            body_set_velocity(object, (vector_t){0, 0});
        } else if (type == KEY_PRESSED) {
            vector_t speed = {0, 0};
        switch (key) {
            case LEFT_ARROW:
                speed.x = -INIT_VEL * cos(body_get_orientation(object));
                speed.y = -INIT_VEL * sin(body_get_orientation(object));
                break;
            case RIGHT_ARROW:
                speed.x = INIT_VEL * cos(body_get_orientation(object));
                speed.y = INIT_VEL * sin(body_get_orientation(object));
                break;
            case UP_ARROW:
                body_set_rotation(object, body_get_orientation(object) + ANGLE_OFFSET);
                break;
            case DOWN_ARROW:
                body_set_rotation(object, body_get_orientation(object) - ANGLE_OFFSET);
                break;
            case ' ':
                if (body_get_time(tank_get_body((tank_t *) object)) > tank_get_curr_reload((tank_t *) object)) {
                    tank_shoot(scene, (tank_t *) object);
                    body_set_time(tank_get_body((tank_t *) object), 0);
                }
                break;
        }
        body_set_velocity(object, speed);
        }
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

void erase_old_scene(scene_t **scenes) {
    for (size_t i = 0; i < scene_bodies(scenes[0]); i++) {
        scene_remove_body(scenes[0], i);
    }
}

void on_mouse(scene_t *scene, vector_t point, bool *play, scene_t **scenes, int *level,
              bool *multi, bool *choosing_level) {
    if (*play) {
        if (within_rect(scene_get_body(scene, PAUSE_BUTTON), point)){
            *play = false;
        }
    } else {
        if (within_rect(scene_get_body(scene, RESUME_BUT), point) && !(*choosing_level)) {
            *play = true;
        } else if (within_rect(scene_get_body(scene, RESTART_BUT), point) && !(*choosing_level)) {
            printf("clicked restart\n");
            erase_scene(scenes[0]);
            make_pause_button(scenes[0]);
            if (*level == 1) {
                level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            } else if (*level == 2) {
                level_2(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            } else if (*level == 3) {
                level_3(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            }
            *play = true;
        } else if (within_rect(scene_get_body(scene, EASY_BUT), point)) {
            printf("clicked easy\n");
            if (*choosing_level) {
                scene_remove_body(scene, WHITE_SCREEN);
                *choosing_level = false;
            }
            erase_scene(scenes[0]);
            make_pause_button(scenes[0]);
            level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            *level = 1;
            *play = true;
        } else if (within_rect(scene_get_body(scene, MEDIUM_BUT), point)) {
            printf("clicked medium\n");
            if (*choosing_level) {
                scene_remove_body(scene, WHITE_SCREEN);
                *choosing_level = false;
            }
            erase_scene(scenes[0]);
            make_pause_button(scenes[0]);
            level_2(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            *level = 2;
            *play = true;
        } else if (within_rect(scene_get_body(scene, HARD_BUT), point)) {
            printf("clicked hard\n");
            if (*choosing_level) {
                scene_remove_body(scene, WHITE_SCREEN);
                *choosing_level = false;
            }
            erase_scene(scenes[0]);
            make_pause_button(scenes[0]);
            level_3(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            *level = 3;
            *play = true;
        } else if (within_rect(scene_get_body(scene, CHOOSE_PLAYER_BOX), point) && !(*choosing_level)) {
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
    add_rect_to_scene(scene, resume_center, BUTTON_LENGTH, BUTTON_HEIGHT, RESUME,
                      color_get_maroon());
    vector_t restart_center = {resume_center.x,
                              resume_center.y - 1.5 * BUTTON_HEIGHT};
    add_rect_to_scene(scene, restart_center, BUTTON_LENGTH, BUTTON_HEIGHT, RESTART,
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

void add_pause_screen_text(scene_t *scene, bool *multi) {
    list_t *resume_shape = body_get_shape(scene_get_body(scene, RESUME_BUT));
    assert(list_size(resume_shape) == 4);
    int x = ((vector_t *)list_get(resume_shape, 0))->x + BUTTON_LENGTH * (1 - TEXT_SCALE)/2;
    int y = ((vector_t *)list_get(resume_shape, 1))->y - BUTTON_HEIGHT * (1 - TEXT_SCALE)/2;
    int width = BUTTON_LENGTH * TEXT_SCALE;
    int height = BUTTON_HEIGHT * TEXT_SCALE;
    char *resume_text = "Resume";
    sdl_write(x, y, width, height, FONT, FONT_SIZE, WHITE_TEXT, resume_text);

    list_t *restart_shape = body_get_shape(scene_get_body(scene, RESTART_BUT));
    assert(list_size(restart_shape) == 4);
    y = ((vector_t *)list_get(restart_shape, 1))->y - BUTTON_HEIGHT * (1 - TEXT_SCALE)/2;
    char *restart_text = "Restart";
    sdl_write(x, y, width, height, FONT, FONT_SIZE, WHITE_TEXT, restart_text);

    char *medium_text = "MEDIUM";
    int medium_x = (TOP_RIGHT_COORD.x - MEDIUM_TEXT_WIDTH) / 2.0;
    int levels_y = (TOP_RIGHT_COORD.y - BUTTON_HEIGHT) / 2.0;
    sdl_write(medium_x,levels_y, MEDIUM_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, FONT, FONT_SIZE,
              BLACK_TEXT, medium_text);

    char *easy_text = "EASY";
    int easy_x = medium_x - (TOP_RIGHT_COORD.x - LEVEL_BUFFER)/3.0;
    sdl_write(easy_x,levels_y, EASY_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, FONT, FONT_SIZE,
              BLACK_TEXT, easy_text);

    char *hard_text = "HARD";
    int hard_x = medium_x + (TOP_RIGHT_COORD.x + HARD_TEXT_WIDTH)/3.0;
    sdl_write(hard_x,levels_y, HARD_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, FONT, FONT_SIZE,
              BLACK_TEXT, hard_text);

    list_t *choose_players_shape = body_get_shape(scene_get_body(scene, CHOOSE_PLAYER_BOX));
    assert(list_size(restart_shape) == 4);
    double cp_x = ((vector_t *)list_get(choose_players_shape, 0))->x + BUFFER;
    double cp_y = ((vector_t *)list_get(choose_players_shape, 1))->y;
    if (*multi) {
        char *cp_text = "Switch to Single Player";
        sdl_write(cp_x, cp_y, CHOOSE_PLAYER_BOX_WIDTH - 2* BUFFER, CHOOSE_PLAYER_BOX_HEIGHT,
                  FONT, FONT_SIZE, WHITE_TEXT, cp_text);
    } else {
        char *cp_text = "Switch to Multiplayer";
        sdl_write(cp_x, cp_y, CHOOSE_PLAYER_BOX_WIDTH - 2* BUFFER, CHOOSE_PLAYER_BOX_HEIGHT,
                  FONT, FONT_SIZE, WHITE_TEXT, cp_text);
    }

    list_free(resume_shape);
    list_free(restart_shape);
    list_free(choose_players_shape);
}

void add_pause_screen_images(scene_t *scene, bool *choosing_level) {
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

    list_t *hard_shape = body_get_shape(scene_get_body(scene, HARD_BUT));
    assert(list_size(hard_shape) == 4);
    int hard_x = ((vector_t *)list_get(hard_shape, 0))->x + level_width * (1 - IMG_X_SCALE)/2;
    int hard_y = ((vector_t *)list_get(hard_shape, 1))->y - level_height * (1 - IMG_Y_SCALE)/2;
    char *hard_image = "images/level3.png";
    sdl_image(hard_image, hard_x, hard_y, image_width, image_height);

    if (*choosing_level) {
        char *text = "Choose Your Level:";
        sdl_write(TOP_RIGHT_COORD.x/2 - CHOOSE_LEVEL_WIDTH/2, TOP_RIGHT_COORD.y - CHOOSE_LEVEL_HEIGHT/2,
                  CHOOSE_LEVEL_WIDTH, CHOOSE_LEVEL_HEIGHT, FONT, FONT_SIZE,
                  MAROON_TEXT, text);
    }

    list_free(easy_shape);
    list_free(medium_shape);
    list_free(hard_shape);
}

void make_tank_power_up(scene_t *scene, powerups_t type, tank_t * tank) {
    rgb_color_t color;
    if (type == MACHINE_GUN) {
        color = (rgb_color_t) {0.0, 0.0, 0.0};
    } else if (type == FRAG_BOMB) {
        color = (rgb_color_t) {0.0, 0.5, 0.0};
    } else if (type == LAND_MINE) {
        color = (rgb_color_t) {0.0, 0.0, 0.5};
    } else if (type == FORCE_FIELD) {
        color = (rgb_color_t) {0.3, 0.0, 0.4};
    }
    else {
        color = (rgb_color_t) {165.0/255, 104.0/255, 42.0/255};
    }
    vector_t power_up_center = {rand() % (int)TOP_RIGHT_COORD.x,
                                rand() % (int)TOP_RIGHT_COORD.y};
    list_t *power_up = animate_rectangle(power_up_center, INIT_POWERUP_LENGTH, INIT_POWERUP_HEIGHT);
    powerups_t *type_pt = malloc(sizeof(powerups_t));
    *type_pt = type;
    body_t *power_up_body = body_init_with_info(power_up, POWERUP_MASS, color, type_pt, free);
    body_set_velocity(power_up_body, (vector_t) {0, 0});
    scene_add_body(scene, power_up_body);

    create_tank_powerup_collision(scene, tank, power_up_body, type);
    create_partial_destructive_collision(scene, tank_get_body(tank), power_up_body);

    // create_tank_powerup_collision(scene, scene_get_body(scene, 1), power_up_body, type);
    // create_partial_destructive_collision(scene, scene_get_body(scene, 1), power_up_body);
}

void check_bullet_ranges(scene_t *scene, tank_t *tank) {
    double curr_range = tank_get_curr_range(tank);
    
    for (size_t i=0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == BULLET) {
            double curr_time = body_get_time(scene_get_body(scene, i));

            if (curr_time > curr_range) {
                body_remove(scene_get_body(scene, i));
            }
        }
    }
}

void update_bullet_times(scene_t *scene, double dt) {
    for (size_t i=0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == BULLET) {
            body_increase_time(scene_get_body(scene, i), dt);
        }
    }
}

void update_tank_times(scene_t *scene, double dt) {
    for (size_t i=0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_1 ||
            *(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_2) {
            body_increase_time(scene_get_body(scene, i), dt);
        }
    }
}

void check_powerup_time(tank_t *tank, double dt) {
    if (tank_get_weapon(tank) != NULL) {
        tank_increase_powerup_time(tank, dt);
        double curr_time = tank_get_curr_powerup_time(tank);

        if (curr_time > tank_get_total_powerup_time(tank)) {
            tank_set_shooting_handler(tank, NULL);
            tank_set_powerup_time(tank, 0);
        }
    }
}

void check_detonation(scene_t *scene, tank_t *tank, double dt) {
    double curr_range = tank_get_curr_range(tank);
    
    for (size_t i=0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == FRAG_BOMB) {
            body_increase_time(scene_get_body(scene, i), dt);
            double curr_time = body_get_time(scene_get_body(scene, i));

            if (curr_time > curr_range) {
                scene_body_detonate(scene, scene_get_body(scene, i));
            }
        }
    }
}

void update_land_mine_times(scene_t *scene, double dt) {
    for (size_t i=0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == LAND_MINE) {
            body_increase_time(scene_get_body(scene, i), dt);
        }
    }
}

void check_land_mine_times(scene_t *scene, tank_t *tank) {
    double curr_range = tank_get_curr_range(tank);
    
    for (size_t i=0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == LAND_MINE) {
            double curr_time = body_get_time(scene_get_body(scene, i));

            if (curr_time > curr_range) {
                body_remove(scene_get_body(scene, i));
            }
        }
    }
}

bool check_for_force_field(scene_t *scene) {
    for (size_t i=0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_FORCE_FIELD) {
            return true;
        }
    }
    return false;
}

body_t *create_new_force_field(scene_t *scene, tank_t *tank) {
    body_t *tank_body = tank_get_body(tank);
    list_t *force_field_pts = animate_ring(body_get_centroid(tank_body), FORCE_FIELD_INNER_RADIUS, FORCE_FIELD_OUTER_RADIUS, CIRC_PTS);

    body_types_t *tank_force_field_info = malloc(sizeof(body_types_t *));
    *tank_force_field_info = TANK_FORCE_FIELD;

    body_t *force_field_body = body_init_with_info(force_field_pts, FORCE_FIELD_MASS,
                                              color_get_blue(), tank_force_field_info, free);

    body_set_velocity(force_field_body, (vector_t) {0, 0});
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == BULLET) {
            create_partial_destructive_collision(scene, force_field_body, scene_get_body(scene, i));
        }
    }
    scene_add_body(scene, force_field_body);

    return force_field_body;
}

void handle_force_field(scene_t *scene, tank_t *tank, double dt) {
    if (tank_get_weapon(tank) == (shooting_handler_t) force_field_shoot) {
        if (check_for_force_field(scene)) {
            for (size_t i=0; i < scene_bodies(scene); i++) {
                if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == TANK_FORCE_FIELD) {
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
    } else {
        create_new_force_field(scene, tank);
    }
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
    bool *multi = malloc(sizeof(bool));
    *multi = false;
    bool *choosing_level = malloc(sizeof(bool));
    *choosing_level = false;

    sdl_on_key((key_handler_t)on_key_push);
    sdl_on_mouse((mouse_handler_t)on_mouse);

    int time_passed = 0;
    scene_t *temp_scene = malloc(sizeof(scene_t *));
    temp_scene = scene;
    scene_t **scenes = malloc(sizeof(scene_t *) * NUM_SCENES);
    scenes[0] = scene;
    scenes[1] = pause_scene;

    vector_t *tank_center = malloc(sizeof(vector_t));
    vector_t center = {rand() % (int)TOP_RIGHT_COORD.x, rand() % (int)TOP_RIGHT_COORD.y};
    *tank_center = center;

    list_t *tank_pts = animate_tank(tank_center);
    tank_t *tank1 = tank_init(tank_pts);

    scene_add_body(scene, tank_get_body(tank1));

    while (!sdl_is_done(scene, scene_get_body(scene, 0), play, scenes, level, multi,
                        choosing_level)) {
        double dt = time_since_last_tick();
        time_passed += dt;

        check_powerup_time(tank1, dt);

        update_bullet_times(scene, dt);
        update_land_mine_times(scene, dt);

        check_bullet_ranges(scene, tank1);
        check_land_mine_times(scene, tank1);

        check_detonation(scene, tank1, dt);

        handle_force_field(scene, tank1, dt);

        if (*play) {
            temp_scene = scene;
        } else {
            temp_scene = pause_scene;
            time_passed = 0;
        }

        sdl_render_scene(temp_scene);
        if (!*play) {
            add_pause_screen_text(temp_scene, multi);
            add_pause_screen_images(temp_scene, choosing_level);
        }
        sdl_show();

        // Shoot a power-up at an interval of time.
        if (time_passed > TANK_POWER_UP_TIME) {
            make_tank_power_up(temp_scene, rand() % NUM_POWERUPS, tank1);
            time_passed = 0;
        }
        
        scene_tick(temp_scene, dt);
    }
    free(play);
    scene_free(pause_scene);
    scene_free(scene);
    free(scenes);
}