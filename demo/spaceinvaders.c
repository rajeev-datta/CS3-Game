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

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {500, 1000};
const int NUM_PELLETS = 14;
const double PELLET_RADIUS = 10;
const int CIRCLE_PTS = 16;
const double BULLET_MASS = 10;
const rgb_color_t GREY_COLOR = {128.0/255, 128.0/255, 128.0/255};
const double INVADER_MASS = 100;
const double SPACESHIP_MASS = 100;
double const ACCELERATION = 100;
const vector_t INIT_VEL = {500, 500};
double const INVADER_BULLET_TIME = 1;
const int NUM_INVADERS = 24;
const double INVADER_RADIUS = 20;
const double SPACING = 20;
const double BULLET_WIDTH = 5;
const double BULLET_LENGTH = 10;
const vector_t BULLET_INIT_VEL = {0,600};
const char INVADER_INFO = 'i';
const char INVADER_BULLET = 'v';
const char SPACESHIP_INFO = 's';
const char SPACESHIP_BULLET = 'b';
const double SPACESHIP_MAJOR = 40;
const double SPACESHIP_MINOR = 20;

void spaceship_shoot(scene_t *scene, void *body) {
    list_t *bullet = animate_rectangle(body_get_centroid(body), BULLET_WIDTH,
                                       BULLET_LENGTH);
    char *space_ship_bullet_info = malloc(sizeof(char *));
    *space_ship_bullet_info = SPACESHIP_BULLET;
    body_t *bullet_body = body_init_with_info(bullet, BULLET_MASS,
                                        color_get_green(), space_ship_bullet_info, free);
    body_set_velocity(bullet_body, BULLET_INIT_VEL);
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(char *) body_get_info(scene_get_body(scene, i)) == INVADER_INFO) {
            create_destructive_collision(scene, bullet_body, scene_get_body(scene, i));
        }
    }
    scene_add_body(scene, bullet_body);
}

void invader_shoot(scene_t *scene, void *body) {
    list_t *bullet = animate_rectangle(body_get_centroid(body),
                                       BULLET_WIDTH, BULLET_LENGTH);
    char *invader_bullet_info = malloc(sizeof(char *));
    *invader_bullet_info = INVADER_BULLET;
    body_t *bullet_body = body_init_with_info(bullet, BULLET_MASS, GREY_COLOR,
                                              invader_bullet_info, free);
    body_set_velocity(bullet_body, vec_negate(BULLET_INIT_VEL));
    scene_add_body(scene, bullet_body);
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(char *) body_get_info(scene_get_body(scene, i)) == SPACESHIP_INFO) {
            create_destructive_collision(scene, bullet_body, scene_get_body(scene, i));
        }
    }
}

void is_shoot(scene_t *scene, body_t *body) {
    double min_distance = 100000;
    body_t *closest = scene_get_body(scene, 0);
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(char *) body_get_info(scene_get_body(scene, i)) == INVADER_INFO) {
            double curr_dist = fabs(body_get_centroid(body).x
                                   - body_get_centroid(scene_get_body(scene, i)).x);
            if (curr_dist < min_distance) {
                min_distance = curr_dist;
                closest = scene_get_body(scene, i);
            }
        } 
    }
    invader_shoot(scene, closest);
}

void on_key_press(char key, key_event_type_t type, double held_time,
                  void *object, scene_t *scene, bool *play) {
    if (object != NULL && !body_is_removed(object)
        && *(char *)body_get_info(object) == SPACESHIP_INFO) {
        if (type == KEY_RELEASED) {
            body_set_velocity(object, (vector_t){0, 0});
        } else if (type == KEY_PRESSED) {
            vector_t speed = {0, 0}; 
            switch (key) {
                case LEFT_ARROW:
                    speed.x = -INIT_VEL.x;
                    break;
                case RIGHT_ARROW:
                    speed.x = INIT_VEL.x;
                    break;
                case ' ':
                    spaceship_shoot(scene, object);
                    break;
            }
            body_set_velocity(object, speed);
        }
    }
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();

    vector_t *initial_pos = malloc(sizeof(vector_t));
    initial_pos->x = TOP_RIGHT_COORD.x/2;
    initial_pos->y = TOP_RIGHT_COORD.y/10;
    list_t *spaceship = animate_ellipse(initial_pos, SPACESHIP_MAJOR, SPACESHIP_MINOR, CIRCLE_PTS);
    char *spaceship_info = malloc(sizeof(char *));
    *spaceship_info = SPACESHIP_INFO;
    body_t *spaceship_body = body_init_with_info(spaceship,
                                SPACESHIP_MASS, color_get_green(), spaceship_info, free);
    scene_add_body(scene, spaceship_body);

    double invader_center_x = INVADER_RADIUS + SPACING;
    double invader_center_y = TOP_RIGHT_COORD.y - INVADER_RADIUS;
    for (int i = 0; i < NUM_INVADERS; i++) {
        if (i % 3 == 0) {
            invader_center_x += INVADER_RADIUS + SPACING;
            invader_center_y = TOP_RIGHT_COORD.y - INVADER_RADIUS;
        }
        else {
           invader_center_y -= 2 * INVADER_RADIUS + SPACING;
        }
        vector_t *invader_center = malloc(sizeof(vector_t));
        invader_center->x = invader_center_x;
        invader_center->y = invader_center_y;
        list_t *invader = animate_invader(invader_center, INVADER_RADIUS, CIRCLE_PTS);
        char *invader_info = malloc(sizeof(char *));
        *invader_info = INVADER_INFO;
        body_t *invader_body = body_init_with_info(invader, INVADER_MASS, GREY_COLOR, invader_info, free);
        body_set_velocity(invader_body, (vector_t) {100, 0});
        scene_add_body(scene, invader_body);
        create_destructive_collision(scene, invader_body, scene_get_body(scene, 0));
    }

    sdl_on_key((key_handler_t)on_key_press);
    double time_passed = 0;
    while (*(char *)body_get_info(scene_get_body(scene, 0)) == SPACESHIP_INFO && 
            !sdl_is_done(scene, scene_get_body(scene, 0), NULL, NULL, NULL, NULL, NULL,
                         NULL, NULL, NULL, NULL, NULL, NULL)) {
        double dt = time_since_last_tick();
        wrapping_boundary(scene, TOP_RIGHT_COORD, INVADER_RADIUS, SPACING);
        stop_boundary(scene, TOP_RIGHT_COORD, BOTTOM_LEFT_COORD, SPACESHIP_MAJOR);
        sdl_render_scene(scene);
        sdl_show();
        time_passed += dt;
        if (time_passed > INVADER_BULLET_TIME && scene_get_body(scene, 0) != NULL
            && *(char *)body_get_info(scene_get_body(scene, 0)) == SPACESHIP_INFO) {
            is_shoot(scene, scene_get_body(scene, 0));
            time_passed = 0;
        }
        scene_tick(scene, dt);
    }
    
   scene_free(scene);
}