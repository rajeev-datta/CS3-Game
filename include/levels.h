#ifndef __LEVELS_H__
#define __LEVELS_H__

#include "scene.h"
#include "vector.h"
#include <stdbool.h>

/**
 *  @brief Applies forces to the tanks and the bullets
 *
 *  @param scene a scene_t representing the scene
 */
void put_forces(scene_t *scene);

/**
 *  @brief Creates boundaries on the sides of the screen
 *
 *  @param scene a scene_t representing the scene
 */
void set_boundaries(vector_t top_right, scene_t *scene);

/**
 *  @brief Creates the first level's walls and places any enemy tanks in their
 *  specific locations
 *
 *  @param top_right a vector_t representing the top right coordinate of the screen
 *  @param scene a scene_t representing the scene
 */
void level_1(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

/**
 *  @brief Creates the second level's walls and places any enemy tanks in their
 *  specific locations
 *
 *  @param top_right a vector_t representing the top right coordinate of the screen
 *  @param scene a scene_t representing the scene
 */
void level_2(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

/**
 *  @brief Creates the third level's walls and places any enemy tanks in their
 *  specific locations
 *
 *  @param top_right a vector_t representing the top right coordinate of the screen
 *  @param scene a scene_t representing the scene
 */
void level_3(vector_t top_right, double wall_length, double wall_height, scene_t *scene, bool *multi);

/**
 *  @brief Returns the integer associated with the first level
 */
int get_first_level();

/**
 *  @brief Returns the integer associated with the second level
 */
int get_second_level();

/**
 *  @brief Returns the integer associated with the third level
 */
int get_third_level();

#endif // #ifndef __LEVELS_H__
