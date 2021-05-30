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
const int NUM_BRICK_IN_ROW = 10;
const int NUM_ROWS = 3;
const double BUFFER = 10;
const double RECT_HEIGHT = 30;
const double INIT_BALL_RADIUS = 10;
const double INIT_PADDLE_LENGTH = 100;
const rgb_color_t RED = {1, 0, 0};
const rgb_color_t WHITE = {1, 1, 1};
const int BALL_PTS = 16;
const double BALL_MASS = 50;
const vector_t INIT_BALL_VEL = {200, 200};
const char BALL_INFO = 'b';
const char BRICK_INFO = 'k';
const char PADDLE_INFO = 'p';
const char WALL_INFO = 'w';
double const INIT_VEL = 800;
int const POWER_UP_TIME = 6;
int const NUM_POWER_UPS = 3;
vector_t const POWER_UP_VEL = {0, -50};
const double ELASTICITY = 1;

list_t *make_colors() {
    list_t *colors = list_init(NUM_BRICK_IN_ROW, free);
    rgb_color_t *red = malloc(sizeof(rgb_color_t));
    *red = RED;
    list_add(colors, red);
    rgb_color_t *orange = malloc(sizeof(rgb_color_t));
    *orange = (rgb_color_t) {1.0, 127.0/255, 0};
    list_add(colors, orange);
    rgb_color_t *yellow = malloc(sizeof(rgb_color_t));
    *yellow = (rgb_color_t) {1.0, 1.0, 0};
    list_add(colors, yellow);
    rgb_color_t *green = malloc(sizeof(rgb_color_t));
    *green = (rgb_color_t) {0, 1.0, 0};
    list_add(colors, green);
    rgb_color_t *teal = malloc(sizeof(rgb_color_t));
    *teal = (rgb_color_t) {0.0, 128.0/255, 128.0/255};
    list_add(colors, teal);
    rgb_color_t *light_blue = malloc(sizeof(rgb_color_t));
    *light_blue = (rgb_color_t) {191.0/255, 239.0/255, 1.0};
    list_add(colors, light_blue);
    rgb_color_t *blue = malloc(sizeof(rgb_color_t));
    *blue = (rgb_color_t) {0.0, 0.0, 1.0};
    list_add(colors, blue);
    rgb_color_t *indigo = malloc(sizeof(rgb_color_t));
    *indigo = (rgb_color_t) {58.0/255, 50.0/255, 155.0/255};
    list_add(colors, indigo);
    rgb_color_t *violet = malloc(sizeof(rgb_color_t));
    *violet = (rgb_color_t) {111.0/255, 50.0/255, 155.0/255};
    list_add(colors, violet);
    rgb_color_t *pink = malloc(sizeof(rgb_color_t));
    *pink = (rgb_color_t) {238.0/255, 130.0/255, 238.0/255};
    list_add(colors, pink);
    return colors;
}

void set_up_board(scene_t *scene) {
    // Create paddle at the center & bottom of the screen.
    vector_t paddle_center = {.x = TOP_RIGHT_COORD.x / 2,
                              .y = BOTTOM_LEFT_COORD.y + BUFFER + RECT_HEIGHT/2};
    list_t *paddle = animate_rectangle(paddle_center, INIT_PADDLE_LENGTH, RECT_HEIGHT);
    char *paddle_info = malloc(sizeof(char *));
    *paddle_info = PADDLE_INFO;
    body_t *paddle_body = body_init_with_info(paddle, INFINITY, RED, paddle_info, free);
    scene_add_body(scene, paddle_body);

    // Create a ball right above the paddle with a diagonal, upward velocity.
    vector_t ball_center = {.x = paddle_center.x,
                            .y = paddle_center.y + RECT_HEIGHT/2 + INIT_BALL_RADIUS + BUFFER};
    list_t *ball = animate_circle(ball_center, INIT_BALL_RADIUS, BALL_PTS);
    char *ball_info = malloc(sizeof(char *));
    *ball_info = BALL_INFO;
    body_t *ball_body = body_init_with_info(ball, BALL_MASS, RED, ball_info, free);
    body_set_velocity(ball_body, INIT_BALL_VEL);
    scene_add_body(scene, ball_body);
    create_physics_collision(scene, ELASTICITY, ball_body, paddle_body);
    

    // Create the walls.
    vector_t left_wall_center = {.x = BOTTOM_LEFT_COORD.x + BUFFER/2,
                                 .y = TOP_RIGHT_COORD.y / 2};
    list_t *left_wall = animate_rectangle(left_wall_center, BUFFER, TOP_RIGHT_COORD.y);
    char *wall_info = malloc(sizeof(char *));
    *wall_info = WALL_INFO;
    body_t *left_wall_body = body_init_with_info(
                                left_wall, INFINITY, WHITE, wall_info, free);
    scene_add_body(scene, left_wall_body);
    create_physics_collision(scene, ELASTICITY, ball_body, left_wall_body);

    vector_t right_wall_center = {.x = TOP_RIGHT_COORD.x - BUFFER/2,
                                 .y = TOP_RIGHT_COORD.y / 2};
    list_t *right_wall = animate_rectangle(right_wall_center, BUFFER, TOP_RIGHT_COORD.y);
    body_t *right_wall_body = body_init_with_info(
                                        right_wall, INFINITY, WHITE, wall_info, free);
    scene_add_body(scene, right_wall_body);
    create_physics_collision(scene, ELASTICITY, ball_body, right_wall_body);

    vector_t top_wall_center = {.x = TOP_RIGHT_COORD.x / 2,
                                 .y = TOP_RIGHT_COORD.y - BUFFER/2};
    list_t *top_wall = animate_rectangle(top_wall_center, TOP_RIGHT_COORD.x, BUFFER);
    body_t *top_wall_body = body_init_with_info(
                                        top_wall, INFINITY, WHITE, wall_info, free);
    scene_add_body(scene, top_wall_body);
    create_physics_collision(scene, ELASTICITY, ball_body, top_wall_body);

    vector_t bottom_wall_center = {.x = TOP_RIGHT_COORD.x / 2,
                                 .y = BOTTOM_LEFT_COORD.y - BUFFER - 2*INIT_BALL_RADIUS};
    list_t *bottom_wall = animate_rectangle(bottom_wall_center, TOP_RIGHT_COORD.x, BUFFER);
    body_t *bottom_wall_body = body_init_with_info(
                                        bottom_wall, INFINITY, WHITE, wall_info, free);
    scene_add_body(scene, bottom_wall_body);

    // Create the bricks.
    list_t *colors = make_colors();
    double brick_length = (TOP_RIGHT_COORD.x - 11 * BUFFER) / NUM_BRICK_IN_ROW;
    char *brick_info = malloc(sizeof(char *));
    *brick_info = BRICK_INFO;

    for (int j = 0; j < NUM_ROWS; j++) {
        for (int i = 0; i < NUM_BRICK_IN_ROW; i++) {
            vector_t brick_center = 
                {.x = BOTTOM_LEFT_COORD.x + BUFFER * (i + 1) + brick_length * (i + 0.5),
                 .y = TOP_RIGHT_COORD.y - BUFFER * (j + 1) - RECT_HEIGHT * (j + 0.5)};
            list_t *brick = animate_rectangle(brick_center, brick_length, RECT_HEIGHT);
            body_t *brick_body = body_init_with_info(brick, INFINITY,
                                *(rgb_color_t *)list_get(colors, i), brick_info, free);
            scene_add_body(scene, brick_body);
            create_physics_collision(scene, ELASTICITY, ball_body, brick_body);
            create_partial_destructive_collision(scene, ball_body, brick_body);
        }
    }
    list_free(colors);
}

void on_key_tap(char key, key_event_type_t type, double held_time,
                void *object, scene_t *scene, bool *play) {
    if (type == KEY_RELEASED) {
        body_set_velocity(object, (vector_t){0, 0});
    } else if (type == KEY_PRESSED) {
        vector_t speed = {0, 0};
        switch (key) {
            case LEFT_ARROW:
                if (body_get_velocity(object).x == 0) {
                    speed.x = -INIT_VEL;
                }
                break;
            case RIGHT_ARROW:
                if (body_get_velocity(object).x == 0) {
                    speed.x = INIT_VEL;
                }
                break;
        }
        body_set_velocity(object, speed);
    }
}

void make_power_up(scene_t *scene, int type) {
    rgb_color_t color;
    if (type == 0) {
        color = (rgb_color_t) {0.0, 0.0, 0.0};
    } else if (type == 1) {
        color = (rgb_color_t) {0.0, 0.5, 0.0};
    } else {
        color = (rgb_color_t) {165.0/255, 104.0/255, 42.0/255};
    }
    vector_t power_up_center = {rand() % (int)TOP_RIGHT_COORD.x,
                                TOP_RIGHT_COORD.y - 3 * RECT_HEIGHT};
    list_t *power_up = animate_circle(power_up_center, INIT_BALL_RADIUS, BALL_PTS);
    char *type_pt = malloc(sizeof(char));
    *type_pt = (char) type;
    body_t *power_up_body = body_init_with_info(power_up, BALL_MASS, color, type_pt, free);
    body_set_velocity(power_up_body, POWER_UP_VEL);
    scene_add_body(scene, power_up_body);
    assert(*(char *)body_get_info(scene_get_body(scene, 0)) == PADDLE_INFO);
    create_powerup_collision(scene, scene_get_body(scene, 0), power_up_body, scene_get_body(scene, 1), type);
    create_partial_destructive_collision(scene, scene_get_body(scene, 5), power_up_body);
}

int main(int argc, char *argv[]) {
    sdl_init(BOTTOM_LEFT_COORD, TOP_RIGHT_COORD);
    scene_t *scene = scene_init();

    set_up_board(scene);

    sdl_on_key((key_handler_t)on_key_tap);
    double time_passed = 0;
    while (!sdl_is_done(scene, scene_get_body(scene, 0), NULL, NULL, NULL)) {
        double dt = time_since_last_tick();

        stop_boundary(scene, TOP_RIGHT_COORD, BOTTOM_LEFT_COORD, BUFFER);
        if (body_get_centroid(scene_get_body(scene, 1)).y <= BOTTOM_LEFT_COORD.y) {
            scene_free(scene);
            scene = scene_init();
            set_up_board(scene);
            time_passed = 0;
        }
        sdl_render_scene(scene);
        sdl_show();
        time_passed += dt;

        // Shoot a power-up at an interval of time.
        if (time_passed > POWER_UP_TIME) {
            make_power_up(scene, rand() % NUM_POWER_UPS);
            time_passed = 0;
        }
        
        scene_tick(scene, dt);
    }
    
   scene_free(scene);
}
