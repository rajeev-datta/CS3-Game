#include <assert.h>
#include <stdlib.h>
#include "list.h"
#include "vector.h"
#include "scene.h"
#include "body.h"
#include <math.h>
#include "test_util.h"
#include "collision.h"
#include "forces.h"
#include <float.h>
#include <stdbool.h>
#include "animate.h"

double const BALL_SLOW = 0.8;
double const BALL_GROW = 1.5;
double const PADDLE_GROW = 1.5;
const double RECTANGLE_HEIGHT = 30;
const int BALL_POINTS = 16;

typedef struct powerup_aux {
    body_t *ball;
    char type;
} powerup_aux_t;

typedef struct collision_aux {
    list_t *bodies;
    collision_handler_t handler;
    void *aux;
    bool just_collided;
} collision_aux_t;

void aux_free(aux_t *aux) {
    list_free(aux->bodies);
    free(aux);
}

aux_t *aux_init(list_t *bodies, double parameter_val) {
    aux_t *aux_obj = malloc(sizeof(aux_t));
    aux_obj->bodies = bodies;
    aux_obj->parameter = parameter_val;
    return aux_obj;
}

void collision_aux_free(collision_aux_t *aux) {
    list_free(aux->bodies);
    free(aux);
}

collision_aux_t *collision_aux_init(list_t *bodies, collision_handler_t handler, void *aux) {
    collision_aux_t *col_aux_obj = malloc(sizeof(collision_aux_t));
    col_aux_obj->bodies = bodies;
    col_aux_obj->handler = handler;
    col_aux_obj->aux = aux;
    col_aux_obj->just_collided = false;
    return col_aux_obj;
}

powerup_aux_t *powerup_aux_init(body_t *body, char type) {
    powerup_aux_t *pow_aux_obj = malloc(sizeof(powerup_aux_t));
    pow_aux_obj->ball = body;
    pow_aux_obj->type = type;
    return pow_aux_obj;
}

void gravity_fxn(aux_t *aux) {
    list_t *bodies = aux->bodies;
    body_t *body_1 = list_get(bodies, 0);
    body_t *body_2 = list_get(bodies, 1);
    //Force F_21
    vector_t radius = vec_subtract(body_get_centroid(body_2), body_get_centroid(body_1));
    double distance = vec_magnitude(radius);
    double denominator = distance * distance;
    vector_t unit_vec = vec_multiply(1.0 / distance, radius);
    double grav_force_mag = (aux->parameter) * (body_get_mass(body_1) *
                            body_get_mass(body_2)) / denominator;
    vector_t grav_force = vec_multiply(grav_force_mag, unit_vec);

    if (distance > 5) {
        body_add_force(body_2, vec_negate(grav_force));
        // Force_12
        body_add_force(body_1, grav_force);
    }
}

void spring_fxn(aux_t *aux) {
    list_t *bodies = aux->bodies;
    body_t *body_1 = list_get(bodies, 0);
    body_t *body_2 = list_get(bodies, 1);
    //Force F_21
    vector_t radius = vec_subtract(body_get_centroid(body_2), body_get_centroid(body_1));
    double distance = vec_magnitude(radius);
    vector_t unit_vec = vec_multiply(1.0 / distance, radius);
    vector_t spring_force = vec_multiply((-1) * (aux->parameter) * distance,
                                        unit_vec);
    body_add_force(body_2, spring_force);
    //Force F_12
    body_add_force(body_1, vec_negate(spring_force));
}

void drag_fxn(aux_t *aux) {
    list_t *bodies = aux->bodies;
    body_t *body = list_get(bodies, 0);
    vector_t velocity = body_get_velocity(body);
    vector_t drag_force = vec_negate(vec_multiply(aux->parameter, velocity));
    body_add_force(body, drag_force);
}

void destructive_fxn(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    // aux represents a bool determining whether this is a partial or complete destructive collision
    if (*(bool *)aux) {
        body_remove(body2);
    }
    else {
        body_remove(body1);
        body_remove(body2);
    }
}

void powerup_fxn(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    body_remove(body2);

    if (((powerup_aux_t *)aux)->type == (char) 0) {
        // Slow down the speed of the ball.
        body_set_velocity(((powerup_aux_t *)aux)->ball, vec_multiply(BALL_SLOW,
                            body_get_velocity(((powerup_aux_t *)aux)->ball)));
    } else if (((powerup_aux_t *)aux)->type == (char) 1) {
        // Increase the size of the paddle.
        list_t *points = body_get_shape(body1);
        double length = ((vector_t *)list_get(points, 1))->x
                         - ((vector_t *)list_get(points, 2))->x;
        length *= PADDLE_GROW;
        list_t *new_rect = animate_rectangle(body_get_centroid(body1), length, RECTANGLE_HEIGHT);
        body_set_shape(body1, new_rect);
    } else {
        // Increase the size of the ball.
        list_t *points = body_get_shape(((powerup_aux_t *)aux)->ball);
        double diameter = vec_magnitude(vec_subtract(*(vector_t *)list_get(points, 0),
                                        *(vector_t *)list_get(points, BALL_POINTS/2 - 1)));
        list_t *new_ball = animate_circle(body_get_centroid(((powerup_aux_t *)aux)->ball),
                                          0.5 * diameter * BALL_GROW, BALL_POINTS);
        body_set_shape(((powerup_aux_t *)aux)->ball, new_ball);
    }
}

void physics_fxn(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    vector_t u_b = vec_multiply(vec_dot(body_get_velocity(body2), axis), axis);
    vector_t u_a = vec_multiply(vec_dot(body_get_velocity(body1), axis), axis);

    if (body_get_mass(body1) == INFINITY && body_get_mass(body2) != INFINITY) {
        body_add_impulse(body1, (vector_t) {0,0});
        vector_t impulse_body1 = vec_multiply(body_get_mass(body2) * ((1.0 + *(double *)aux)),
                                              vec_subtract(u_b, u_a));
        body_add_impulse(body2, impulse_body1);

    }
    
    else if (body_get_mass(body2) == INFINITY && body_get_mass(body1) != INFINITY) {
        body_add_impulse(body2, (vector_t) {0,0});
        vector_t impulse_body1 = vec_multiply(body_get_mass(body1) * ((1.0 + *(double *)aux)),
                                              vec_subtract(u_b, u_a));
        body_add_impulse(body1, impulse_body1);
    }

    else {
        vector_t impulse_body1 = vec_multiply(((body_get_mass(body1) * body_get_mass(body2) * (1.0 + *(double *)aux)) /
                                        (body_get_mass(body1) + body_get_mass(body2))), vec_subtract(u_b, u_a));
        vector_t impulse_body2 = vec_negate(impulse_body1);
        body_add_impulse(body1, impulse_body1);
        body_add_impulse(body2, impulse_body2);
    }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2) {
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    aux_t *aux_obj = aux_init(bodies, G);
    scene_add_bodies_force_creator(scene, (force_creator_t)gravity_fxn, aux_obj,
                                   bodies, (free_func_t)aux_free);
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    aux_t *aux_obj = aux_init(bodies, k);
    scene_add_bodies_force_creator(scene, (force_creator_t)spring_fxn, aux_obj,
                                   bodies, (free_func_t)aux_free);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
    list_t *bodies = list_init(1, NULL);
    list_add(bodies, body);
    aux_t *aux_obj = aux_init(bodies, gamma);
    scene_add_bodies_force_creator(scene, (force_creator_t)drag_fxn, aux_obj,
                                   bodies, (free_func_t)aux_free);
}

void detect_collision(collision_aux_t *col_aux) {
    list_t *bodies = col_aux->bodies;
    body_t *body1 = list_get(bodies, 0);
    body_t *body2 = list_get(bodies, 1);
    collision_info_t collision = find_collision(body_get_real_shape(body1), body_get_real_shape(body2));
    if (body1 != NULL && body2 != NULL && collision.collided) {
        if (!col_aux->just_collided) {
            col_aux->handler(body1, body2, collision.axis, col_aux->aux);
            col_aux->just_collided = true;
        }
    }
    else {
        col_aux->just_collided = false;
    }
}

void create_collision(
    scene_t *scene,
    body_t *body1,
    body_t *body2,
    collision_handler_t handler,
    void *aux,
    free_func_t freer
) {
    list_t *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    collision_aux_t *aux_obj = collision_aux_init(bodies, (collision_handler_t) handler, aux);
    scene_add_bodies_force_creator(scene, (force_creator_t)detect_collision, aux_obj,
                                   bodies, (free_func_t)freer);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2) {
    bool *partial_destruction = malloc(sizeof(bool));
    *partial_destruction = false;
    create_collision(scene, body1, body2, (collision_handler_t) destructive_fxn,
                    partial_destruction, (free_func_t) free);
}

void create_partial_destructive_collision(scene_t *scene, body_t *body1, body_t *body2) {
    bool *partial_destruction = malloc(sizeof(bool));
    *partial_destruction = true;
    create_collision(scene, body1, body2, (collision_handler_t) destructive_fxn,
                    partial_destruction, (free_func_t) free);
}

void create_powerup_collision(scene_t *scene, body_t *body1, body_t *body2, body_t *ball, char type) {
    powerup_aux_t *pow_aux = powerup_aux_init(ball, type);
    create_collision(scene, body1, body2, (collision_handler_t) powerup_fxn,
                    pow_aux, (free_func_t) free);
}

void create_physics_collision(
    scene_t *scene,
    double elasticity,
    body_t *body1,
    body_t *body2
) {
    double *numbers = malloc(sizeof(double) * 2);
    numbers[0] = elasticity;
    numbers[1] = 0;
    create_collision(scene, body1, body2, (collision_handler_t) physics_fxn,
                    numbers, (free_func_t) free);
}
