#ifndef __LEVELS_H__
#define __LEVELS_H__

#include "scene.h"
#include "vector.h"
#include <stdbool.h>

void put_forces(scene_t *scene);

void set_boundaries(vector_t top_right, scene_t *scene);

void level_1(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

void level_2(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

void level_3(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

int get_first_level();

int get_second_level();

int get_third_level();

#endif // #ifndef __LEVELS_H__
