#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "sdl_wrapper.h"
#include "animate.h"
#include "polygon.h"
#include "star.h"
#include "color.h"
#include "list.h"

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {1000, 500};
const int NUM_PELLETS = 14;
const double PELLET_RADIUS = 10;
const int CIRCLE_PTS = 16;
const double PELLET_MASS = 10;
const rgb_color_t YELLOW = {252.0/255, 227.0/255, 3.0/255};
const double PACMAN_RADIUS = 60;
const double PACMAN_MASS = 100;
double const ACCELERATION = 100;
double const INIT_VEL = 100;
double const PELLET_TIME = 1;

void on_key(char key, key_event_type_t type, double held_time, void *object,
            scene_t *scene, bool *play) {
    if (type == KEY_RELEASED) {
        body_set_velocity(object, (vector_t){0, 0});
    } else if (type == KEY_PRESSED) {
        vector_t speed = {0, 0};
        switch (key) {
            case LEFT_ARROW:
                if (body_get_velocity(object).x == 0) {
                    speed.x = -INIT_VEL;
                }
                body_set_rotation(object, M_PI);
                speed.x += body_get_velocity(object).x -
                           ACCELERATION * held_time; 
                break;
            case UP_ARROW:
                if (body_get_velocity(object).y == 0) {
                    speed.y = INIT_VEL;
                }
                body_set_rotation(object, M_PI/2);
                speed.y += body_get_velocity(object).y + 
                           ACCELERATION * held_time;
                break;
            case RIGHT_ARROW:
                if (body_get_velocity(object).x == 0) {
                    speed.x = INIT_VEL;
                }
                body_set_rotation(object, 0);
                speed.x += body_get_velocity(object).x + 
                           ACCELERATION * held_time;
                break;
            case DOWN_ARROW:
                if (body_get_velocity(object).y == 0) {
                    speed.y = -INIT_VEL;
                }
                body_set_rotation(object, 3*M_PI/2);
                speed.y += body_get_velocity(object).y - 
                           ACCELERATION * held_time;
                break;
        }
        body_set_velocity(object, speed);
    }
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();

    vector_t *initial_pos = malloc(sizeof(vector_t));
    initial_pos->x = TOP_RIGHT_COORD.x/2;
    initial_pos->y = TOP_RIGHT_COORD.y/2;
    list_t *pacman = animate_pacman(initial_pos, PACMAN_RADIUS, CIRCLE_PTS);
    body_t *pacman_body = body_init(pacman, PACMAN_MASS, YELLOW);
    scene_add_body(scene, pacman_body);

    for (int i = 0; i < NUM_PELLETS; i++) {
        vector_t pellet_center = {TOP_RIGHT_COORD.x * rand()/RAND_MAX,
                                  TOP_RIGHT_COORD.y * rand()/RAND_MAX};
        list_t *pellet = animate_circle(pellet_center, PELLET_RADIUS, CIRCLE_PTS);
        body_t *pellet_body = body_init(pellet, PELLET_MASS, YELLOW);
        scene_add_body(scene, pellet_body);
    }

    sdl_on_key((key_handler_t)on_key);
    double time_passed = 0;
    while (!sdl_is_done(scene, pacman_body, NULL, NULL)) {
        double dt = time_since_last_tick();
        remove_pellets(pacman_body, PACMAN_RADIUS, PELLET_RADIUS, scene);
        scene_tick(scene, dt);
        continuous_boundary(pacman_body, TOP_RIGHT_COORD);
        sdl_render_scene(scene);
        // generate new pellets
        time_passed += dt;
        if (time_passed > PELLET_TIME) {
            vector_t pellet_center = {TOP_RIGHT_COORD.x * rand()/RAND_MAX,
                                  TOP_RIGHT_COORD.y * rand()/RAND_MAX};
            list_t *pellet = animate_circle(pellet_center, PELLET_RADIUS, CIRCLE_PTS);
            body_t *pellet_body = body_init(pellet, PELLET_MASS, YELLOW);
            scene_add_body(scene, pellet_body);
            time_passed = 0;
        }
    }
    
   scene_free(scene);
}
