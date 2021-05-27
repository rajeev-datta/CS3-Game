#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "animate.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "star.h"

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {1000, 500};
const double VELOCITY = 500;
const vector_t CENTER = {250, 250};
const double INNER_RADIUS = 30;
const double OUTER_RADIUS = 80;
const float RED_VAL = 1;
const float GREEN_VAL = 0;
const float BLUE_VAL = 0;
const size_t STAR_POINTS = 12;
const double ROTATE_ANGLE = M_PI / 360.0;

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);

    star_t *star = star_init(STAR_POINTS, RED_VAL, GREEN_VAL, BLUE_VAL);
    star_set_vertices(star, make_star(CENTER, INNER_RADIUS, OUTER_RADIUS, STAR_POINTS));
    star_get_velocity(star)->x = VELOCITY;
    star_get_velocity(star)->y = VELOCITY;

    while(!sdl_is_done(NULL, NULL, NULL, NULL)) {
        double dt = time_since_last_tick();
        hit_boundary_check(star, BOTTOM_LEFT_COORD, TOP_RIGHT_COORD, dt);
        vector_t translation = {star_get_velocity(star)->x * dt,
                                star_get_velocity(star)->y * dt};
        polygon_translate(star_get_vertices(star), translation);
        polygon_rotate(star_get_vertices(star), ROTATE_ANGLE,
                       polygon_centroid(star_get_vertices(star)));
        sdl_clear();
        sdl_draw_polygon(star_get_vertices(star), *star_get_color(star));
        sdl_show();
    }

    star_free(star);
}
