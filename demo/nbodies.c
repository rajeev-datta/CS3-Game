#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "sdl_wrapper.h"
#include "animate.h"
#include "color.h"
#include "list.h"
#include "forces.h"

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {1000, 500};
const vector_t STAR_VELOCITY = {5, 5};
const int NUM_BODIES = 50;
const int NUM_PTS = 8;
const double G = 20;
const int MAX_RADIUS = 50;
const double MIN_RADIUS = 10;
const double MASS_RATIO = 5;
const double RADIUS_SCALE = 2;
const double ANGLE = M_PI/2.5;

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();

    for (int i = 0; i < NUM_BODIES; i++) {
        vector_t center = {TOP_RIGHT_COORD.x * rand()/RAND_MAX,
                                  TOP_RIGHT_COORD.y * rand()/RAND_MAX};
        double inner_radius = rand() % MAX_RADIUS + MIN_RADIUS;
        double outer_radius = inner_radius / RADIUS_SCALE;
        list_t *body_points = make_star(center, outer_radius, inner_radius, NUM_PTS);
        double mass = outer_radius * MASS_RATIO;
        body_t *body = body_init(body_points, mass, color_random());
        body_set_rotation(body, ANGLE);
        body_set_velocity(body, STAR_VELOCITY);
        scene_add_body(scene, body);
        for(int j = 0; j < i; j++) {
            create_newtonian_gravity(scene, G, body, scene_get_body(scene, j));
        }
    }

    while (!sdl_is_done(scene, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                        NULL, NULL)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
        sdl_show();
    }

   scene_free(scene);
}
