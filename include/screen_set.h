#ifndef __SCREEN_SET_H__
#define __SCREEN_SET_H__

#include "scene.h"
#include "tank.h"

// An enum to keep track of the indices of the bodies on the pause scene.
typedef enum pause_scene{
    RESUME_BUT,
    RESTART_BUT,
    MEDIUM_BUT,
    EASY_BUT,
    HARD_BUT,
    CHOOSE_PLAYER_BOX,
    WHITE_SCREEN
} pause_scene_t;

// An enum to keep track of the indices of the bodies on the play scene.
typedef enum scene_indices{
    PAUSE_BUTTON,
    PAUSE_BUTTON_WHITE,
    TANK1,
    TANK2
} scene_indices_t;

// An enum to keep track of the outcomes of the game.
typedef enum game_outcomes{
    GAME_ONGOING,
    SINGLE_PLAYER_LOSS,
    PLAYER1_WIN,
    PLAYER2_WIN,
    SINGLE_PLAYER_WIN
} game_outcomes_t;

// Returns the length of the button.
double get_button_length();

// Returns the height of the button.
double get_button_height();

// Returns the width of the choose-level button.
int get_choose_level_width();

// Returns the height of the choose-level button.
int get_choose_level_height();

// Returns the buffer length
double get_buffer();

// Returns the buffer between levels.
int get_level_buffer();

// Returns the height of the button that lets you switch between single and multiplayer.
int get_choose_player_box_height();

// Returns the height of the button that lets you switch between single and multiplayer.
int get_choose_player_box_width();

// Returns the bottom left coordinate of the scene.
vector_t get_bottom_left();

// Returns the top right coordinate of the scene.
vector_t get_top_right();

// Returns Player 1's initial position.
vector_t get_tank1_init_pos();

// Returns Player 2's initial position.
vector_t get_tank2_init_pos();

// Returns the position of Tank 2 when the game is single player.
vector_t get_tank2_off_screen();

// Returns the initial number of lives that players start with.
int get_init_lives();

// Returns the number of lives that will lead to a tank losing.
int get_min_lives_limit();

// Makes the pause button and adds it to the scene
void screen_set_pause_button(scene_t *scene);

// Makes a rectangle and adds it to the scene
void screen_set_rect(scene_t *scene, vector_t center, int width, int height,
                       int information, rgb_color_t color);

// Places the tanks in their correct positions at the start of a match
void screen_set_tanks(scene_t *scene, bool *multi, tank_t *tank1, tank_t* tank2);

// Sets up the pause screen
void screen_set_pause_screen(scene_t *scene);

// Makes a tank and adds it to the scene
tank_t *screen_set_new_tank(scene_t *scene, body_types_t info, vector_t center);

// Makes a new enemy tank and adds it to the scene
void screen_set_new_enemy(scene_t *scene, vector_t *center);

#endif // #ifndef __SCREEN_SET_H__