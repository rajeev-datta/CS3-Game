#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include <stdbool.h>
#include "scene.h"
#include "tank.h"

/**
 * Checks to see if all of the enemy tanks in the game are gone"
 */
bool gameplay_enemies_gone(scene_t *scene);

/**
 * Sees if there is a winner to the game
 * @return -1 if the game is ongoing and there is no winner
 *          0 The game is single player, and the player loses
 *          1 The game is multiplayer, and player 1 wins
 *          2 The game is multiplayer, and player 2 wins
 *          3 The game is multiplayer, and player 1 wins
 */
int gameplay_find_winner(scene_t *scene, tank_t *tank1, tank_t *tank2, bool *multi,
                bool *game_over);

#endif // #ifndef __GAMEPLAY_H__