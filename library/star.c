#include <assert.h>
#include <stdlib.h>
#include "vector.h"
#include "list.h"
#include "color.h"

typedef struct star {
    list_t *vertices;
    vector_t *velocity;
    rgb_color_t *color;
} star_t;

void star_free(star_t *star) {
    list_free(star->vertices);
    free(star->color);
    free(star->velocity);
    free(star);
}

star_t *star_init(size_t initial_size, float r, float g, float b) {
    star_t *star_obj = malloc(sizeof(star_t));
    assert(star_obj != NULL);
    star_obj->velocity = malloc(sizeof(vector_t));
    assert(star_obj->velocity != NULL);
    star_obj->velocity->x = 0;
    star_obj->velocity->y = 0;
    star_obj->vertices = list_init(initial_size, free);
    star_obj->color = malloc(sizeof(rgb_color_t));
    assert(star_obj->color != NULL);
    star_obj->color->r = r;
    star_obj->color->g = g;
    star_obj->color->b = b;
    return star_obj;
}

void star_set_vertices(star_t *star, list_t *verts) {
    star->vertices = verts;
}

vector_t *star_get_velocity(star_t *star) {
    return star->velocity;
}

list_t *star_get_vertices(star_t *star) {
    return star->vertices;
}

rgb_color_t *star_get_color(star_t *star) {
    return star->color;
}
