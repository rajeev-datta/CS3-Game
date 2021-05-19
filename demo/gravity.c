#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "animate.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "list.h"
#include "star.h"
#include <stdio.h>
#include "color.h"

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {1000, 500};
const double INNER_RADIUS = 20;
const double OUTER_RADIUS = 60;
const float RED_VAL = 1;
const float GREEN_VAL = 0;
const float BLUE_VAL = 0;
const size_t INIT_POINTS = 4;
const double INIT_X_VEL = 200;
const double INIT_Y_VEL = -500;
const double GRAVITY = 1000;
const double ELASTICITY = 0.67;
const size_t NUM_STARS = 8;
const double NEXT = 200;
const double ROTATE_ANGLE = M_PI / 360.0;

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);

    vector_t initial_pos = {OUTER_RADIUS, TOP_RIGHT_COORD.y - OUTER_RADIUS};

    list_t *stars = list_init(NUM_STARS, (free_func_t)star_free); 

    int num_pts = INIT_POINTS;
    float red = (float)rand()/RAND_MAX;
    float green = (float)rand()/RAND_MAX;
    float blue = (float)rand()/RAND_MAX;
    star_t *star1 = star_init(num_pts, red, green, blue);
    star_set_vertices(star1, make_star(initial_pos, INNER_RADIUS, OUTER_RADIUS, 
                                       num_pts));
    star_get_velocity(star1)->x = INIT_X_VEL;
    star_get_velocity(star1)->y = INIT_Y_VEL;
    list_add(stars, star1);

    while (!sdl_is_done(NULL, NULL)) {
        double dt = time_since_last_tick();
        star_t *last = list_get(stars, list_size(stars) - 1);
        if (polygon_centroid(star_get_vertices(last)).x >= NEXT) {
            num_pts += 2;
            red = (float)rand()/RAND_MAX;
            green = (float)rand()/RAND_MAX;
            blue = (float)rand()/RAND_MAX;
            star_t *star = star_init(num_pts, red, green, blue);
            star_set_vertices(star, make_star(initial_pos, INNER_RADIUS, OUTER_RADIUS,
                              num_pts));
            star_get_velocity(star)->x = INIT_X_VEL;
            star_get_velocity(star)->y = INIT_Y_VEL;
            list_add(stars, star);
        }

        star_t *oldest = list_get(stars, 0);
        if (polygon_centroid(star_get_vertices(oldest)).x >= TOP_RIGHT_COORD.x
                                                             + OUTER_RADIUS) {
            list_remove(stars, 0);
        }

        sdl_clear();
        for (int i = 0; i < list_size(stars); i++) {
            star_t *star = list_get(stars, i);
            sdl_draw_polygon(star_get_vertices(star), *star_get_color(star));
            bounce(star, GRAVITY, BOTTOM_LEFT_COORD, TOP_RIGHT_COORD, dt,
                   OUTER_RADIUS, ELASTICITY);
            vector_t translation = {star_get_velocity(star)->x * dt, 
                                    star_get_velocity(star)->y * dt};
            polygon_translate(star_get_vertices(star), translation);
            polygon_rotate(star_get_vertices(star), ROTATE_ANGLE,
                           polygon_centroid(star_get_vertices(star)));
        }
        sdl_show();
    }
    list_free(stars);
}
