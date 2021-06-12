#include <stdlib.h>
#include <stdio.h>
#include "sdl_wrapper.h"
#include <stdbool.h>
#include "animate.h"
#include "color.h"
#include "list.h"
#include "tank.h"
#include "vector.h"
#include "scene.h"
#include "forces.h"
#include "screen_set.h"
#include "levels.h"

static const double ELASTICITY = 1;
static const int FIRST_LEVEL = 1;
static const int SECOND_LEVEL = 2;
static const int THIRD_LEVEL = 3;

void put_forces(scene_t *scene) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        for (size_t j = 0; j < scene_bodies(scene); j++) {
            if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == BULLET) {
                if (*(body_types_t *) body_get_info(scene_get_body(scene, j)) == TANK_1
                || *(body_types_t *) body_get_info(scene_get_body(scene, j)) == TANK_2
                || *(body_types_t *) body_get_info(scene_get_body(scene, j)) == ENEMY_TANK) {
                    create_partial_destructive_collision(scene, scene_get_body(scene, j), scene_get_body(scene, i));
                }
                if (*(body_types_t *) body_get_info(scene_get_body(scene, j)) == WALL) {
                    create_physics_collision(scene, ELASTICITY, scene_get_body(scene, i), scene_get_body(scene, j));
                }
            }
            if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_1
            || *(body_types_t *) body_get_info(scene_get_body(scene, j)) == TANK_2
            || *(body_types_t *) body_get_info(scene_get_body(scene, j)) == ENEMY_TANK) {
                if (*(body_types_t *) body_get_info(scene_get_body(scene, j)) == WALL) {
                    create_physics_collision(scene, ELASTICITY, scene_get_body(scene, i), scene_get_body(scene, j));
                }
            }
        }
    }
}

void set_boundaries(vector_t top_right, scene_t *scene) {
    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    *wall_info = WALL;

    list_t *top_boundary = animate_rectangle((vector_t) {top_right.x/2, top_right.y+5}, top_right.x, 5);
    body_t *top_boundary_body = body_init_with_info(top_boundary, INFINITY, color_get_white(), wall_info, free);
    scene_add_body(scene, top_boundary_body);

    list_t *bottom_boundary = animate_rectangle((vector_t) {top_right.x/2, -5}, top_right.x, 5);
    body_t *bottom_boundary_body = body_init_with_info(bottom_boundary, INFINITY, color_get_white(), wall_info, free);
    scene_add_body(scene, bottom_boundary_body);

    list_t *left_boundary = animate_rectangle((vector_t) {-5, top_right.y/2}, 5, top_right.y);
    body_t *left_boundary_body = body_init_with_info(left_boundary, INFINITY, color_get_white(), wall_info, free);
    scene_add_body(scene, left_boundary_body);
    
    list_t *right_boundary = animate_rectangle((vector_t) {top_right.x+5, top_right.y/2}, 5, top_right.y);
    body_t *right_boundary_body = body_init_with_info(right_boundary, INFINITY, color_get_white(), wall_info, free);
    scene_add_body(scene, right_boundary_body);
}

void level_1(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi) {
    if (!*multi) {
        vector_t *enemy_center = malloc(sizeof(vector_t));
        *enemy_center = (vector_t){900, get_top_right().y/2};
        screen_set_new_enemy(scene, enemy_center);
    }

    set_boundaries(top_right, scene);

    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    *wall_info = WALL;

    list_t *center_wall = animate_rectangle((vector_t) {top_right.x/2, top_right.y/2}, wall_length, wall_height*2);
    body_t *center_wall_body = body_init_with_info(center_wall, INFINITY, color_get_lime(), wall_info, free);
    scene_add_body(scene, center_wall_body);

    list_t *left_top_wall = animate_rectangle((vector_t) {top_right.x/4, top_right.y - (wall_height/2)}, wall_length, wall_height);
    body_t *left_top_wall_body = body_init_with_info(left_top_wall, INFINITY, color_get_lime(), wall_info, free);
    scene_add_body(scene, left_top_wall_body);

    list_t *left_bottom_wall = animate_rectangle((vector_t) {top_right.x/4, wall_height/2}, wall_length, wall_height);
    body_t *left_bottom_wall_body = body_init_with_info(left_bottom_wall, INFINITY, color_get_lime(), wall_info, free);
    scene_add_body(scene, left_bottom_wall_body);

    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*3)/4, top_right.y - (wall_height/2)}, wall_length, wall_height);
    body_t *right_top_wall_body = body_init_with_info(right_top_wall, INFINITY, color_get_lime(), wall_info, free);
    scene_add_body(scene, right_top_wall_body);

    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/4, wall_height/2}, wall_length, wall_height);
    body_t *right_bottom_wall_body = body_init_with_info(right_bottom_wall, INFINITY, color_get_lime(), wall_info, free);
    scene_add_body(scene, right_bottom_wall_body);

    put_forces(scene);
}

void level_2(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi) {
    if (!*multi) {
        vector_t *enemy_center = malloc(sizeof(vector_t));
        *enemy_center = (vector_t){top_right.x * 4/10, get_top_right().y};
        screen_set_new_enemy(scene, enemy_center);

        vector_t *enemy2_center = malloc(sizeof(vector_t));
        *enemy2_center = (vector_t){600, 0};
        screen_set_new_enemy(scene, enemy2_center);
    }
    
    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    *wall_info = WALL;

    set_boundaries(top_right, scene);

    list_t *center_top_wall = animate_rectangle((vector_t) {top_right.x/2, (top_right.y*3.5)/10}, wall_height, wall_length);
    body_t *center_top_wall_body = body_init_with_info(center_top_wall, INFINITY, color_get_teal(), wall_info, free);
    scene_add_body(scene, center_top_wall_body);

    list_t *center_bottom_wall = animate_rectangle((vector_t) {top_right.x/2, (top_right.y*6.5)/10}, wall_height, wall_length);
    body_t *center_bottom_wall_body = body_init_with_info(center_bottom_wall, INFINITY, color_get_teal(), wall_info, free);
    scene_add_body(scene, center_bottom_wall_body);

    list_t *left_top_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y*4)/5}, wall_height, wall_length);
    body_t *left_top_wall_body = body_init_with_info(left_top_wall, INFINITY, color_get_teal(), wall_info, free);
    scene_add_body(scene, left_top_wall_body);

    list_t *left_center_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y)/2}, wall_length, wall_height);
    body_t *left_center_wall_body = body_init_with_info(left_center_wall, INFINITY, color_get_teal(), wall_info, free);
    scene_add_body(scene, left_center_wall_body);

    list_t *left_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/10, (top_right.y)/5}, wall_height, wall_length);
    body_t *left_bottom_wall_body = body_init_with_info(left_bottom_wall, INFINITY, color_get_teal(), wall_info, free);
    scene_add_body(scene, left_bottom_wall_body);

    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y*4)/5}, wall_height, wall_length);
    body_t *right_top_wall_body = body_init_with_info(right_top_wall, INFINITY, color_get_teal(), wall_info, free);
    scene_add_body(scene, right_top_wall_body);

    list_t *right_center_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y)/2}, wall_length, wall_height);
    body_t *right_center_wall_body = body_init_with_info(right_center_wall, INFINITY, color_get_teal(), wall_info, free);
    scene_add_body(scene, right_center_wall_body);

    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*7)/10, (top_right.y)/5}, wall_height, wall_length);
    body_t *right_bottom_wall_body = body_init_with_info(right_bottom_wall, INFINITY, color_get_teal(), wall_info, free);
    scene_add_body(scene, right_bottom_wall_body);

    put_forces(scene);
}

void level_3(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi) {
    if (!*multi) {
        vector_t *enemy_center = malloc(sizeof(vector_t));
        *enemy_center = (vector_t){250, get_top_right().y/2};
        screen_set_new_enemy(scene, enemy_center);

        vector_t *enemy2_center = malloc(sizeof(vector_t));
        *enemy2_center = (vector_t){550, 0};
        screen_set_new_enemy(scene, enemy2_center);
        
        vector_t *enemy3_center = malloc(sizeof(vector_t));
        *enemy3_center = (vector_t){900, get_top_right().y};
        screen_set_new_enemy(scene, enemy3_center);
    }
    
    body_types_t *wall_info = malloc(sizeof(body_types_t *));
    *wall_info = WALL;

    set_boundaries(top_right, scene);   

    list_t *left_center_top_wall = animate_rectangle((vector_t) {(top_right.x*7)/20, (top_right.y*3)/4}, wall_length, wall_height*2);
    body_t *left_center_top_wall_body = body_init_with_info(left_center_top_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, left_center_top_wall_body);

    list_t *left_center_bottom_wall = animate_rectangle((vector_t) {(top_right.x*7)/20, (top_right.y)/4}, wall_length, wall_height);
    body_t *left_center_bottom_wall_body = body_init_with_info(left_center_bottom_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, left_center_bottom_wall_body);

    list_t *right_center_top_wall = animate_rectangle((vector_t) {(top_right.x*13)/20, (top_right.y*3)/4}, wall_length, wall_height);
    body_t *right_center_top_wall_body = body_init_with_info(right_center_top_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, right_center_top_wall_body);

    list_t *right_center_bottom_wall = animate_rectangle((vector_t) {(top_right.x*13)/20, (top_right.y)/4}, wall_length, wall_height*2);
    body_t *right_center_bottom_wall_body = body_init_with_info(right_center_bottom_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, right_center_bottom_wall_body);

    list_t *center_wall = animate_rectangle((vector_t) {(top_right.x)/2, (top_right.y)/2}, wall_length, wall_height*3.5);
    body_t *center_wall_body = body_init_with_info(center_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, center_wall_body);

    list_t *left_top_wall = animate_rectangle((vector_t) {(top_right.x)*3/20, (top_right.y*9)/10}, wall_length, wall_height);
    body_t *left_top_wall_body = body_init_with_info(left_top_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, left_top_wall_body);

    list_t *left_center_wall = animate_rectangle((vector_t) {(top_right.x*3)/20, (top_right.y)/2}, wall_length, wall_height);
    body_t *left_center_wall_body = body_init_with_info(left_center_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, left_center_wall_body);

    list_t *left_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/20, (top_right.y)/10}, wall_length, wall_height);
    body_t *left_bottom_wall_body = body_init_with_info(left_bottom_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, left_bottom_wall_body);

    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*17)/20, (top_right.y*9)/10}, wall_length, wall_height);
    body_t *right_top_wall_body = body_init_with_info(right_top_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, right_top_wall_body);

    list_t *right_center_wall = animate_rectangle((vector_t) {(top_right.x*17)/20, (top_right.y)/2}, wall_length, wall_height);
    body_t *right_center_wall_body = body_init_with_info(right_center_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, right_center_wall_body);

    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*17)/20, (top_right.y)/10}, wall_length, wall_height);
    body_t *right_bottom_wall_body = body_init_with_info(right_bottom_wall, INFINITY, color_get_purple(), wall_info, free);
    scene_add_body(scene, right_bottom_wall_body);
}

int get_first_level() {
    return FIRST_LEVEL;
}

int get_second_level() {
    return SECOND_LEVEL;
}

int get_third_level() {
    return THIRD_LEVEL;
}
