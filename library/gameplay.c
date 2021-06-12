#include "gameplay.h"
#include "sound.h"
#include "screen_set.h"

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

game_outcomes_t gameplay_find_winner(scene_t *scene, tank_t *tank1, tank_t *tank2,
                                     bool *multi, bool *game_over) {
    if (!(*multi) && gameplay_enemies_gone(scene)) {
        // Condition where player 1 wins
        *game_over = true;
        return SINGLE_PLAYER_WIN;
    }
    if (body_get_lives(tank_get_body(tank1)) <= get_min_lives_limit()) {
        *game_over = true;
        play_remove_sound();
        if (*multi) {
            // Condition where player 2 wins
            return PLAYER2_WIN;
        }
        // Condition where player 1 loses
        return SINGLE_PLAYER_LOSS;
    }
    if (*multi && body_get_lives(tank_get_body(tank2)) <= get_min_lives_limit()) {
        *game_over = true;
        play_remove_sound();
        // Condition where player 1 wins
        return PLAYER1_WIN;
    }
    // Game is still going on
    return GAME_ONGOING;
}