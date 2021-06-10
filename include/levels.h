#ifndef __LEVELS_H__
#define __LEVELS_H__

#include "scene.h"
#include "vector.h"
#include <stdbool.h>

void put_forces(scene_t *scene);

void level_1(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

void level_2(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

void level_3(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

#endif // #ifndef __LEVELS_H__
