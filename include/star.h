#ifndef __STAR_H__
#define __STAR_H__

#include <stdlib.h>
#include "color.h"
#include "vector.h"
#include "list.h"

typedef struct star star_t;

void star_free(star_t *star);

star_t *star_init(size_t initial_size, float r, float g, float b);

void star_set_vertices(star_t *star, list_t *verts);

vector_t* star_get_velocity(star_t *star);

list_t *star_get_vertices(star_t *star);

rgb_color_t *star_get_color(star_t *star);

#endif // #ifndef __STAR_H__
