#ifndef __SCREEN_SET_H__
#define __SCREEN_SET_H__

#include "scene.h"
#include "tank.h"

typedef enum pause_scene{
    RESUME_BUT,
    RESTART_BUT,
    MEDIUM_BUT,
    EASY_BUT,
    HARD_BUT,
    CHOOSE_PLAYER_BOX,
    WHITE_SCREEN
} pause_scene_t;

typedef enum scene_indices{
    PAUSE_BUTTON,
    PAUSE_BUTTON_WHITE,
    TANK1,
    TANK2
} scene_indices_t;

double get_button_length();

double get_button_height();

int get_choose_level_width();

int get_choose_level_height();

double get_buffer();

int get_level_buffer();

int get_choose_player_box_height();

int get_choose_player_box_width();

vector_t get_bottom_left();

vector_t get_top_right();

vector_t get_tank1_init_pos();

vector_t get_tank2_init_pos();

vector_t get_tank2_off_screen();

int get_init_lives();

// Makes the pause button and adds it to the scene
void screen_set_pause_button(scene_t *scene);

// Makes a rectangle and adds it to the scene
void screen_set_rect(scene_t *scene, vector_t center, int width, int height,
                       int information, rgb_color_t color);

// Places the tanks in their correct positions at the start of a match
void screen_set_tanks(scene_t *scene, bool *multi, tank_t *tank1, tank_t* tank2);

// Sets up the pause screen
void screen_set_pause_screen(scene_t *scene);

#endif // #ifndef __SCREEN_SET_H__