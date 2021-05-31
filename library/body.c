#include <assert.h>
#include <stdlib.h>
#include "list.h"
#include "vector.h"
#include "color.h"
#include "polygon.h"
#include <math.h>
#include <stdbool.h>

typedef struct body {
    list_t *shape;
    vector_t velocity;
    double mass;
    rgb_color_t color;
    vector_t centroid;
    double orientation;
    vector_t force;
    vector_t impulse;
    void *info;
    free_func_t info_freer;
    bool remove;
    double time;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    body_t *body_obj = malloc(sizeof(body_t));
    assert(body_obj != NULL);
    body_obj->velocity.x = 0;
    body_obj->velocity.y = 0;
    body_obj->shape = shape;
    body_obj->color.r = color.r;
    body_obj->color.g = color.g;
    body_obj->color.b = color.b;
    body_obj->centroid = polygon_centroid(body_obj->shape);
    body_obj->orientation = 0;
    body_obj->mass = mass;
    body_obj->force = VEC_ZERO;
    body_obj->impulse = VEC_ZERO;
    body_obj->remove = false;
    body_obj->info_freer = NULL;
    body_obj->time = 0;
    return body_obj;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color, void *info,
                            free_func_t info_freer) {
    body_t *body_obj = body_init(shape, mass, color);
    body_obj->info = info;
    return body_obj;
}

void body_free(body_t *body) {
    list_free(body->shape);
    if (body->info_freer != NULL) {
        body->info_freer(body->info);
    }
    free(body);
}

list_t *body_get_shape(body_t *body) {
    list_t *copy_shape = list_init(list_capacity(body->shape), free);
    for (size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *vector = malloc(sizeof(vector_t));
        *vector = *(vector_t *)list_get(body->shape, i);
        list_add(copy_shape, vector);
    }
    return copy_shape;
}

list_t *body_get_real_shape(body_t *body) {
    return body->shape;
}

vector_t body_get_centroid(body_t *body) {
    return body->centroid;
}

vector_t body_get_velocity(body_t *body) {
    return body->velocity;
}

double body_get_mass(body_t *body) {
    return body->mass;
}

rgb_color_t body_get_color(body_t *body) {
    return body->color;
}

void *body_get_info(body_t *body) {
    // assert(body->info != NULL);
    return body->info;
}

double body_get_orientation(body_t *body) {
    return body->orientation;
}

void body_set_centroid(body_t *body, vector_t x) {
    vector_t translation = vec_subtract(x, body->centroid);
    polygon_translate(body->shape, translation);
    body->centroid = x;
}

void body_set_shape(body_t *body, list_t *shape) {
    body->shape = shape;
}

void body_set_velocity(body_t *body, vector_t v) {
    body->velocity = v;
}

void body_set_rotation(body_t *body, double angle) {
    polygon_rotate(body_get_real_shape(body), angle - body->orientation, body_get_centroid(body));
    body->orientation = angle;
}

void body_set_info(body_t *body, void *new_info) {
    body->info = new_info;
}

void body_add_force(body_t *body, vector_t force) {
    if (body->force.x == 0 && body->force.y == 0) {
        body->force = force;
    }
    else {
        vector_t new_force = vec_add(body->force, force);
        body->force = new_force;
    }
}

void body_add_impulse(body_t *body, vector_t impulse) {
    if (body->impulse.x == 0 && body->impulse.y == 0) {
        body->impulse = impulse;
    }
    else {
        vector_t new_impulse = vec_add(body->impulse, impulse);
        body->impulse = new_impulse;
    }
}

void body_set_impulse(body_t *body, vector_t new_impulse) {
    body->impulse = new_impulse;
}

void body_set_force(body_t *body, vector_t new_force) {
    body->force = new_force;
}

void body_tick(body_t *body, double dt) {
    vector_t before_velocity = body->velocity;
    vector_t net_force = body->force;
    body_add_impulse(body, vec_multiply(dt, net_force));
    vector_t change_velocity = vec_multiply(1.0 / (body->mass), body->impulse);
    vector_t after_velocity = vec_add(before_velocity, change_velocity);
    vector_t translate_velocity = vec_multiply(1.0 / 2.0, vec_add(before_velocity, after_velocity));
    body_set_velocity(body, after_velocity);
    vector_t translation = vec_multiply(dt, translate_velocity);
    body_set_centroid(body, vec_add(body->centroid, translation));
    body_set_impulse(body, VEC_ZERO);
    body_set_force(body, VEC_ZERO);
}

void body_remove(body_t *body) {
    body->remove = true;
}

bool body_is_removed(body_t *body) {
    return body->remove;
}

double body_get_time(body_t *body) {
    return body->time;
}

void body_set_time(body_t *body, double time) {
    body->time = time;
}

void body_increase_time(body_t *body, double time_increment) {
    double curr_time = body_get_time(body);
    curr_time += time_increment;
    body_set_time(body, curr_time);
}
