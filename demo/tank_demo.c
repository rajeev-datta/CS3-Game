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
const double BUTTON_LENGTH = 500;
const double BUTTON_HEIGHT = 100;
const double INIT_POWERUP_LENGTH = 10;
const double INIT_POWERUP_HEIGHT = 10;
const double POWERUP_MASS = 50;
const int NUM_POWERUPS = 6;
const int TANK_POWER_UP_TIME = 10;
double const INIT_VEL = 400;
double const ANGLE_OFFSET = (10 * M_PI)/180;
const double ELASTICITY = 1;
const int NUM_SCENES = 2;

typedef enum pause_scene{RESUME_BUT, RESTART_BUT};
typedef enum info{
    PAUSE,
    RESUME,
    RESTART
} info_t;

typedef enum scene_indices{
    PAUSE_BUTTON,
};

typedef enum powerups{
    MACHINE_GUN,
    FRAG_BOMB,
    LAND_MINE,
    FORCE_FIELD,
    LASER,
    REMOTE_MISSILE,
} powerups_t;

typedef enum bodies {
    PLAYER_TANK,
    ENEMY_TANK,
    WALL,
    BULLET
} body_t;

void put_forces(scene_t *scene) { //should work for different levels because scene is argument
    for(size_t i = 0; i < scene_bodies(scene); i++) {
        for(size_t j = 0; j < scene_bodies(scene); j++) {
            if(scene_get_body(i)->type == BULLET) {
                if(scene_get_body(j)->type == PLAYER_TANK || scene_get_body(j)->type == ENEMY_TANK) {
                    //bullet disappears because tanks have lives, so they survive
                    //need to write code to check for lives
                    create_partial_destructive_collision(scene, scene_get_body(j), scene_get_body(i));
                }
                else if(scene_get_body(j)->type == WALL) {
                    create_physics_collision(scene, ELASTICITY, scene_get_body(i), scene_get_body(j));
                }
            }
        }
    }

}

void make_pause_button(scene_t *scene) {
    double width = PAUSE_SCALE * PAUSE_HEIGHT;
    vector_t pause_center = {BOTTOM_LEFT_COORD.x + width/2.0 + 3 * BUFFER,
                             TOP_RIGHT_COORD.y - PAUSE_HEIGHT/2.0 - 3 * BUFFER};
    list_t *big_rect = animate_rectangle(pause_center, width, PAUSE_HEIGHT);
    info_t *pause_info = malloc(sizeof(info_t *));
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
    body_type_t *tank_info = malloc(sizeof(body_type_t *));
    tank_info = TANK_1;
    list_t *tank = animate_tank((vector_t) {100, TOP_RIGHT_COORD.y/2});
    body_t *tank_body = body_init_with_info(tank, 50, RED, tank_info, free);
    scene_add_body(scene, tank_body);

    body_type_t *wall_info = malloc(sizeof(body_type_t *));
    wall_info = WALL;
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

void on_mouse(scene_t *scene, vector_t point, bool *play, scene_t **scenes) {
    if (*play) {
        if (within_rect(scene_get_body(scenes[0], PAUSE_BUTTON), point)){
            *play = false;
        }
    } else {
        if (within_rect(scene_get_body(scenes[1], RESUME_BUT), point)) {
            *play = true;
        } else if (within_rect(scene_get_body(scenes[1], RESTART_BUT), point)) {
            printf("clicked restart\n");
            erase_old_scene(scenes);
            make_pause_button(scenes[0]);
            level_1(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scenes[0]);
            *play = true;
        }
    }
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

void make_tank_power_up(scene_t *scene, powerups_t type) {
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
    powerups_t *type_pt = malloc(sizeof(powerups_t));
    *type_pt = type;
    body_t *power_up_body = body_init_with_info(power_up, POWERUP_MASS, color, type_pt, free);
    body_set_velocity(power_up_body, (vector_t) {0, 0});
    scene_add_body(scene, power_up_body);
    assert(*(body_type_t *)body_get_info(scene_get_body(scene, 0)) == TANK_1); // tank 1 is most likely not at index 0 anymore
    create_tank_powerup_collision(scene, scene_get_body(scene, 0), power_up_body, type);
    create_partial_destructive_collision(scene, scene_get_body(scene, 0), power_up_body);
    assert(*(body_type_t *)body_get_info(scene_get_body(scene, 1)) == TANK_2); // tank 2 is most likely not at index 0 anymore
    create_tank_powerup_collision(scene, scene_get_body(scene, 1), power_up_body, type);
    create_partial_destructive_collision(scene, scene_get_body(scene, 1), power_up_body);
}

void check_bullet_ranges(scene_t *scene, tank_t *tank) {
    double curr_range = tank_get_curr_range(tank);
    
    for (size_t i; i < scene_bodies; i++) {
        if (*(body_type_t *)body_get_info(scene_get_body(scene, i)) == BULLET) {
            double curr_time = body_get_time(scene_get_body(scene, i));

            if (curr_time > curr_range) {
                body_remove(scene_get_body(scene, i));
            }
        }
    }
}

void update_bullet_times(scene_t *scene, double dt) {
    for (size_t i; i < scene_bodies; i++) {
        if (*(body_type_t *)body_get_info(scene_get_body(scene, i)) == BULLET) {
            body_increase_time(scene_get_body(scene, i), dt);
        }
    }
}

void update_tank_times(scene_t *scene, double dt) {
    for (size_t i; i < scene_bodies; i++) {
        if (*(body_type_t *)body_get_info(scene_get_body(scene, i)) == TANK_1 ||
            *(body_type_t *)body_get_info(scene_get_body(scene, i)) == TANK_2) {
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
    
    for (size_t i; i < scene_bodies; i++) {
        if (*(body_type_t *)body_get_info(scene_get_body(scene, i)) == FRAG_BOMB) {
            body_increase_time(scene_get_body(scene, i), dt);
            double curr_time = body_get_time(scene_get_body(scene, i));

            if (curr_time > curr_range) {
                body_detonate(scene, scene_get_body(scene, i));
            }
        }
    }
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

    vector_t *tank_center = malloc(sizeof(vector_t));
    vector_t center = {rand() % (int)TOP_RIGHT_COORD.x, rand() % (int)TOP_RIGHT_COORD.y};
    *tank_center = center;

    list_t *tank_pts = animate_tank(tank_center);
    tank_t *tank1 = tank_init(tank_center, NULL);

    scene_add_body(scene, tank_get_body(tank1));

    while (!sdl_is_done(scene, scene_get_body(scene, 0), play, scenes)) {
        double dt = time_since_last_tick();
        time_passed += dt;

        check_powerup_time(tank1, dt);

        update_bullet_times(scene, dt);
        check_bullet_ranges(scene, tank1);

        check_detonation(scene, tank1, dt);

        if (*play) {
            temp_scene = scene;
        } else {
            temp_scene = pause_scene;
            time_passed = 0;
        }

        sdl_render_scene(temp_scene);
        sdl_show();

        // Shoot a power-up at an interval of time.
        if (time_passed > TANK_POWER_UP_TIME) {
            make_tank_power_up(temp_scene, rand() % NUM_POWERUPS);
            time_passed = 0;
        }
        
        scene_tick(temp_scene, dt);
    }
    free(play);
    scene_free(pause_scene);
    scene_free(scene);
    free(scenes);
}