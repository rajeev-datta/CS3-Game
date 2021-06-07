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
#include "forces.h"

const vector_t BOTTOM_LEFT_COORD = {0, 0};
const vector_t TOP_RIGHT_COORD = {1000, 500};
double const INIT_VEL = 2;
double const K = 100;
double const GAMMA = 20;
double const NUM_BODIES = 50;
double const RADIUS = 15;
const int CIRCLE_PTS = 16;
const double BODY_MASS = 100;

rgb_color_t random_color() {
    float red = (float)rand()/RAND_MAX;
    float green = (float)rand()/RAND_MAX;
    float blue = (float)rand()/RAND_MAX;
    rgb_color_t color = {red, green, blue};
    return color;
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();

    int offset = 1;
    
    for (int i = 0; i < NUM_BODIES; i++) {

        vector_t invis_body_center = {RADIUS * offset, TOP_RIGHT_COORD.y / 2};
        list_t *invis_body_circle = animate_circle(invis_body_center, 1, CIRCLE_PTS);
        body_t *invis_body = body_init(invis_body_circle, INFINITY, color_get_white());
        scene_add_body(scene, invis_body);

        vector_t body_center = {RADIUS * offset,
                                    (TOP_RIGHT_COORD.y - 5*i) / 2};
        list_t *body_circle = animate_circle(body_center, RADIUS, CIRCLE_PTS);
        body_t *body = body_init(body_circle, BODY_MASS, random_color());
        body_set_velocity(body, (vector_t) {0, (NUM_BODIES - i) * INIT_VEL});
        scene_add_body(scene, body);
        
        create_spring(scene, K, body, invis_body);
        create_drag(scene, GAMMA * i/2, body);
        offset += 2;
    }
    

    while (!sdl_is_done(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
        sdl_show();
    }
    
    scene_free(scene);
}
