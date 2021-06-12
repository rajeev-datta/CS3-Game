#include "screen_set.h"
#include "vector.h"
#include "animate.h"
#include <math.h>

static const double PAUSE_SCALE = 2.0/3;
static const double PAUSE_HEIGHT = 40;
static const double BUTTON_LENGTH = 300;
static const double BUTTON_HEIGHT = 50;
static const int CHOOSE_LEVEL_WIDTH = 500;
static const int CHOOSE_LEVEL_HEIGHT = 100;
static const double BUFFER = 10;
static const int LEVEL_BUFFER = 50;
static const int CHOOSE_PLAYER_BOX_HEIGHT = 30;
static const int CHOOSE_PLAYER_BOX_WIDTH = 340;
static const vector_t BOTTOM_LEFT_COORD = {0, 0};
static const vector_t TOP_RIGHT_COORD = {1000, 500};
static const vector_t TANK1_INIT_POS = {50, 250};
static const vector_t TANK2_INIT_POS = {950, 250};
static const vector_t TANK2_OFF_SCREEN = {1200, 700};
static const int INIT_LIVES = 3;
static const int MIN_LIVES_LIMIT = 0;

double get_button_length() {
    return BUTTON_LENGTH;
}

double get_button_height() {
    return BUTTON_HEIGHT;
}

int get_choose_level_width() {
    return CHOOSE_LEVEL_WIDTH;
}

int get_choose_level_height() {
    return CHOOSE_LEVEL_HEIGHT;
}

double get_buffer() {
    return BUFFER;
}

int get_level_buffer() {
    return LEVEL_BUFFER;
}

int get_choose_player_box_height() {
    return CHOOSE_PLAYER_BOX_HEIGHT;
}

int get_choose_player_box_width() {
    return CHOOSE_PLAYER_BOX_WIDTH;
}

vector_t get_bottom_left() {
    return BOTTOM_LEFT_COORD;
}

vector_t get_top_right() {
    return TOP_RIGHT_COORD;
}

vector_t get_tank1_init_pos() {
    return TANK1_INIT_POS;
}

vector_t get_tank2_init_pos() {
    return TANK2_INIT_POS;
}

vector_t get_tank2_off_screen() {
    return TANK2_OFF_SCREEN;
}

int get_init_lives() {
    return INIT_LIVES;
}

int get_min_lives_limit() {
    return MIN_LIVES_LIMIT;
}

void screen_set_pause_button(scene_t *scene) {
    double width = PAUSE_SCALE * PAUSE_HEIGHT;
    vector_t pause_center = {BOTTOM_LEFT_COORD.x + width/2.0 + 3 * BUFFER,
                             TOP_RIGHT_COORD.y - PAUSE_HEIGHT/2.0 - 3 * BUFFER};
    screen_set_rect(scene, pause_center, width, PAUSE_HEIGHT, PAUSE_BUTTON, color_get_red());
    screen_set_rect(scene, pause_center, width/3.0, PAUSE_HEIGHT, PAUSE_BUTTON_WHITE, color_get_white());
}

void screen_set_rect(scene_t *scene, vector_t center, int width, int height,
                       int information, rgb_color_t color) {
    list_t *rect = animate_rectangle(center, width, height);
    int *info = malloc(sizeof(int));
    *info = information;
    body_t *rect_body = body_init_with_info(rect, INFINITY, color, info, free);
    body_set_is_pause_button(rect_body, true);
    scene_add_body(scene, rect_body);
}

void screen_set_tanks(scene_t *scene, bool *multi, tank_t *tank1, tank_t* tank2) {
    body_set_centroid(scene_get_body(scene, TANK1), TANK1_INIT_POS);
    body_set_rotation(scene_get_body(scene, TANK1), 0);
    body_set_lives(scene_get_body(scene, TANK1), INIT_LIVES);
    tank_set_shooting_handler(tank1, NULL);
    tank_set_shooting_handler(tank2, NULL);

    if (*multi) {
        body_set_centroid(scene_get_body(scene, TANK2), TANK2_INIT_POS);
        body_set_rotation(scene_get_body(scene, TANK2), M_PI);
        body_set_lives(scene_get_body(scene, TANK2), INIT_LIVES);
    } else {
        body_set_centroid(scene_get_body(scene, TANK2), TANK2_OFF_SCREEN);
    }
}

void screen_set_pause_screen(scene_t *scene) {
    vector_t resume_center = {get_top_right().x / 2.0,
                              get_top_right().y - 1.5 * get_button_height()};
    screen_set_rect(scene, resume_center, get_button_length(), get_button_height(), RESUME_BUT,
                      color_get_maroon());
    vector_t restart_center = {resume_center.x,
                              resume_center.y - 1.5 * get_button_height()};
    screen_set_rect(scene, restart_center, get_button_length(), get_button_height(), RESTART_BUT,
                      color_get_maroon());

    double level_width = get_top_right().x / 3.0 - get_level_buffer();
    double level_height = level_width/2;
    vector_t medium_center = {get_top_right().x / 2.0, get_top_right().y/2 - 2.5 * get_button_height()};
    screen_set_rect(scene, medium_center, level_width, level_height, MEDIUM_BUT,
                      color_get_maroon());
    vector_t easy_center = {medium_center.x - get_level_buffer() - level_width, medium_center.y};
    screen_set_rect(scene, easy_center, level_width, level_height, EASY_BUT,
                      color_get_maroon());
    vector_t hard_center = {medium_center.x + get_level_buffer() + level_width, medium_center.y};
    screen_set_rect(scene, hard_center, level_width, level_height, HARD_BUT,
                      color_get_maroon());

    vector_t player_selection_center = {get_top_right().x / 2.0,
                                    restart_center.y - 1.5 * get_button_height()};
    screen_set_rect(scene, player_selection_center, get_choose_player_box_width(),
                      get_choose_player_box_height(), CHOOSE_PLAYER_BOX, color_get_maroon());
}

tank_t *screen_set_new_tank(scene_t *scene, body_types_t info, vector_t center) {
    body_types_t *tank_info = malloc(sizeof(body_types_t *));
    *tank_info = info;
    vector_t *tank_center = malloc(sizeof(vector_t));
    *tank_center = center;
    list_t *tank_points = animate_tank(tank_center);
    tank_t *tank = tank_init(tank_points, tank_info);
    body_set_is_tank(tank_get_body(tank), true);
    scene_add_body(scene, tank_get_body(tank));
    return tank;
}

void screen_set_new_enemy(scene_t *scene, vector_t *center) {
    body_types_t *enemy_tank_info = malloc(sizeof(body_types_t *));
    *enemy_tank_info = ENEMY_TANK;
    vector_t speed = (vector_t) {0, 150};
    list_t *enemy_tank_points = animate_tank(center);
    tank_t *enemy_tank = enemy_tank_init(enemy_tank_points, speed, enemy_tank_info);
    scene_add_body(scene, tank_get_body(enemy_tank));
}
