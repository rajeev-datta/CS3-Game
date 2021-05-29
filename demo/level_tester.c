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
const double LEVEL_1_WALL_LENGTH = 10;
const double LEVEL_1_WALL_HEIGHT = 100;
const rgb_color_t RED = {1, 0, 0};
const char WALL_INFO = 'w';
const char TANK_INFO = 't';
double const INIT_VEL = 400;
double const ANGLE_OFFSET = (10 * M_PI)/180;
const double ELASTICITY = 1;

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

void on_key_press(char key, key_event_type_t type, double held_time,
                void *object, scene_t *scene, bool *play) {
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

                break;
        }
        body_set_velocity(object, speed);
    }
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();
    level_2(TOP_RIGHT_COORD, LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scene);
    sdl_on_key((key_handler_t)on_key_press);

    while (!sdl_is_done(scene, scene_get_body(scene, 0), NULL, NULL)) {
        double dt = time_since_last_tick();
        sdl_clear();
        sdl_render_scene(scene);      
        scene_tick(scene, dt);
    }
    
   scene_free(scene);
}
