#include "text.h"
#include "sdl_wrapper.h"
#include "screen_set.h"
#include <assert.h>

static const double TEXT_SCALE = 0.8;
static const SDL_Color WHITE_TEXT = {255, 255, 255};
static const SDL_Color BLACK_TEXT = {0, 0, 0};
static const SDL_Color MAROON_TEXT = {128, 0, 0};
static const SDL_Color RED_TEXT = {255, 0, 0};
static const SDL_Color BLUE_TEXT = {0, 0, 255};
static const int EASY_TEXT_WIDTH = 50;
static const int MEDIUM_TEXT_WIDTH = 90;
static const int HARD_TEXT_WIDTH = 50;
static const int LEVEL_TEXT_HEIGHT = 20;
static const int LIVES_TXT = 9;
static const int LIVES_DIGIT = 4;
static const int LIVES_WIDTH = 100;
static const int LIVES_HEIGHT = 50;
static const char *CHOSEN_FONT = "fonts/AnticSlab-Regular.ttf";
static const int FONT_SIZE = 100;
TTF_Font *FONT;

void text_open_font() {
    FONT = TTF_OpenFont(CHOSEN_FONT, FONT_SIZE);
    if (!FONT) {
        printf("TTF_OpenFontRW: %s\n", TTF_GetError());
    }
}

void text_for_pause(scene_t *scene, bool *multi, bool *choosing_level, int win, bool *game_started) {
    list_t *resume_shape = body_get_shape(scene_get_body(scene, RESUME_BUT));
    assert(list_size(resume_shape) == 4);
    int x = ((vector_t *)list_get(resume_shape, 0))->x + get_button_length() * (1 - TEXT_SCALE)/2;
    int y = ((vector_t *)list_get(resume_shape, 1))->y - get_button_height() * (1 - TEXT_SCALE)/2;
    int width = get_button_length() * TEXT_SCALE;
    int height = get_button_height() * TEXT_SCALE;
    char *resume_text = "Resume";
    sdl_write(x, y, width, height, FONT, WHITE_TEXT, resume_text);

    list_t *restart_shape = body_get_shape(scene_get_body(scene, RESTART_BUT));
    assert(list_size(restart_shape) == 4);
    y = ((vector_t *)list_get(restart_shape, 1))->y - get_button_height() * (1 - TEXT_SCALE)/2;
    char *restart_text = "Restart";
    sdl_write(x, y, width, height, FONT, WHITE_TEXT, restart_text);

    char *medium_text = "MEDIUM";
    int medium_x = (get_top_right().x - MEDIUM_TEXT_WIDTH) / 2.0;
    int levels_y = (get_top_right().y - get_button_height()) / 2.0;
    sdl_write(medium_x,levels_y, MEDIUM_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, FONT,
              BLACK_TEXT, medium_text);

    char *easy_text = "EASY";
    int easy_x = medium_x - (get_top_right().x - get_level_buffer())/3.0;
    sdl_write(easy_x,levels_y, EASY_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, FONT,
              BLACK_TEXT, easy_text);

    char *hard_text = "HARD";
    int hard_x = medium_x + (get_top_right().x + HARD_TEXT_WIDTH)/3.0;
    sdl_write(hard_x,levels_y, HARD_TEXT_WIDTH, LEVEL_TEXT_HEIGHT, FONT,
              BLACK_TEXT, hard_text);

    list_t *choose_players_shape = body_get_shape(scene_get_body(scene, CHOOSE_PLAYER_BOX));
    assert(list_size(restart_shape) == 4);
    double cp_x = ((vector_t *)list_get(choose_players_shape, 0))->x + get_buffer();
    double cp_y = ((vector_t *)list_get(choose_players_shape, 1))->y;
    if (*multi) {
        char *cp_text = "Switch to Single Player";
        sdl_write(cp_x, cp_y, get_choose_player_box_width() - 2* get_buffer(), get_choose_player_box_height(),
                  FONT, WHITE_TEXT, cp_text);
    } else {
        char *cp_text = "Switch to Multiplayer";
        sdl_write(cp_x, cp_y, get_choose_player_box_width() - 2* get_buffer(), get_choose_player_box_height(),
                  FONT, WHITE_TEXT, cp_text);
    }

    if (*choosing_level) {
        char *text = "Choose Your Level:";
        sdl_write(get_top_right().x/2 - get_choose_level_width()/2,
                  get_top_right().y - get_choose_level_height()/2,
                  get_choose_level_width(), get_choose_level_height(), FONT, MAROON_TEXT, text);
    } else {
        if (win == 0) {
            char *text = "You Lose!";
            sdl_write(get_top_right().x/2 - get_choose_level_width()/2,
                    get_top_right().y - get_choose_level_height()/2,
                    get_choose_level_width(), get_choose_level_height(), FONT, MAROON_TEXT, text);
        } else if (win == 1) {
            char *text = "Red Player Wins!";
            sdl_write(get_top_right().x/2 - get_choose_level_width()/2,
                    get_top_right().y - get_choose_level_height()/2,
                    get_choose_level_width(), get_choose_level_height(), FONT, MAROON_TEXT, text);
        } else if (win == 2) {
            char *text = "Blue Player Wins!";
            sdl_write(get_top_right().x/2 - get_choose_level_width()/2,
                    get_top_right().y - get_choose_level_height()/2,
                    get_choose_level_width(), get_choose_level_height(), FONT, MAROON_TEXT, text);
        } else if (win == 3) {
            char *text = "You Win!";
            sdl_write(get_top_right().x/2 - get_choose_level_width()/2,
                    get_top_right().y - get_choose_level_height()/2,
                    get_choose_level_width(), get_choose_level_height(), FONT, MAROON_TEXT, text);
        } else if (*game_started) {
            char *text = "Start Game";
            sdl_write(get_top_right().x/2 - get_choose_level_width()/2,
                    get_top_right().y - get_choose_level_height()/2,
                    get_choose_level_width(), get_choose_level_height(), FONT, MAROON_TEXT, text);
        }
    }

    list_free(resume_shape);
    list_free(restart_shape);
    list_free(choose_players_shape);
}

void text_for_play(scene_t *scene, bool *multi, tank_t *tank1,
                          tank_t *tank2) {
    char *tank1_lives_text = malloc(sizeof(char) * LIVES_TXT + LIVES_DIGIT);
    strcpy(tank1_lives_text, "Lives: ");
    int tank1_lives = body_get_lives(tank_get_body(tank1));
    char *num1 = malloc(sizeof(char) * LIVES_DIGIT);
    sprintf(num1, "%u", tank1_lives);
    tank1_lives_text = strcat(tank1_lives_text, num1);

    int x = get_top_right().x/2 - LIVES_WIDTH/2;
    int y1 = get_top_right().y - get_buffer();
    sdl_write(x, y1, LIVES_WIDTH, LIVES_HEIGHT, FONT,
              RED_TEXT, tank1_lives_text);
    free(tank1_lives_text);
    free(num1);
    
    if (*multi) {
        char *tank2_lives_text = malloc(sizeof(char) * LIVES_TXT + LIVES_DIGIT);
        strcpy(tank2_lives_text, "Lives: ");
        int tank2_lives = body_get_lives(tank_get_body(tank2));
        char *num2 = malloc(sizeof(char) * LIVES_DIGIT);
        sprintf(num2, "%u", tank2_lives);
        tank2_lives_text = strcat(tank2_lives_text, num2);

        int y2 = get_bottom_left().y + get_buffer() + LIVES_HEIGHT;
        sdl_write(x, y2, LIVES_WIDTH, LIVES_HEIGHT, FONT,
                BLUE_TEXT, tank2_lives_text);
        free(tank2_lives_text);
        free(num2);
    }
}

void text_free() {
    TTF_CloseFont(FONT);
}
