#include "gameplay.h"
#include "sound.h"

bool gameplay_enemies_gone(scene_t *scene) {
    bool all_gone = true;
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == ENEMY_TANK) {
            all_gone = false;
            break;
        }
    }
    return all_gone;
}

int gameplay_find_winner(scene_t *scene, tank_t *tank1, tank_t *tank2, bool *multi,
                bool *game_over) {
    if (!(*multi) && gameplay_enemies_gone(scene)) {
        // Condition where player 1 wins
        *game_over = true;
        return 3;
    }
    if (body_get_lives(tank_get_body(tank1)) <= 0) {
        *game_over = true;
        play_remove_sound();
        if (*multi) {
            // Condition where player 2 wins
            return 2;
        }
        // Condition where player 1 loses
        return 0;
    }
    if (*multi && body_get_lives(tank_get_body(tank2)) <= 0) {
        *game_over = true;
        play_remove_sound();
        // Condition where player 1 wins
        return 1;
    }
    // Game is still going on
    return -1;
}