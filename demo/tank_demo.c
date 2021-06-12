#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "animate.h"
#include "polygon.h"
#include "star.h"
#include "color.h"
#include "list.h"
#include "tank.h"
#include "powerup.h"
#include "vector.h"
#include "scene.h"
#include "forces.h"
#include "sound.h"
#include "gameplay.h"
#include "text.h"
#include "screen_set.h"
#include "levels.h"
#include "image.h"

static const double LEVEL_1_WALL_LENGTH = 10;
static const double LEVEL_1_WALL_HEIGHT = 100;
static const int NUM_POWERUPS = 6;
static const int TANK_POWER_UP_TIME = 3;
static const int TANK_SHOOT_TIME = 1;
static double const INIT_VEL = 200;
static double const ANGLE_OFFSET = (10 * M_PI)/180;
static const int NUM_SCENES = 2;
static const int FIRST_REMOVABLE_INDEX = 4;
static const int START_TIME = 0;

typedef enum scenes{
    PLAY,
    PAUSE
} scenes_t;

void on_key_push(char key, key_event_type_t type, double held_time,
                 void *object, scene_t *scene, bool *play, bool *multi,
                 tank_t *tank1, tank_t *tank2, list_t *keys_pressed) {
    if (*play) {
        if (!*multi) {
            body_t *tank1_body = tank_get_body(tank1);
            if (tank_get_weapon(tank1) == (shooting_handler_t) remote_missile_shoot) {
                bool missile_exists = false;
                list_clear(keys_pressed);
                body_set_velocity(tank1_body, VEC_ZERO);
                for (size_t i=0; i < scene_bodies(scene); i++) {
                    if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                        == TANK_REMOTE_MISSILE_1) {
                        body_t *missile = scene_get_body(scene, i);
                        missile_exists = true;
                        list_clear(keys_pressed);
                        if (type == KEY_RELEASED) {
                            body_set_velocity(missile, VEC_ZERO);
                            list_clear(keys_pressed);
                        } else if (type == KEY_PRESSED) {
                            wall_boundary(scene, missile);
                            vector_t speed = VEC_ZERO;
                            char *key_tap = malloc(sizeof(char));
                            *key_tap = key;
                            list_add(keys_pressed, key_tap);
                            for (size_t i = 0; i < list_size(keys_pressed); i++) {
                                char key_pressed = *(char *) list_get(keys_pressed, i);
                                if (key_pressed == 's') {
                                    speed.x = -INIT_VEL
                                              * cos(body_get_orientation(missile));
                                    speed.y = -INIT_VEL
                                              * sin(body_get_orientation(missile));
                                }
                                if (key_pressed == 'w') {
                                    speed.x = INIT_VEL
                                              * cos(body_get_orientation(missile));
                                    speed.y = INIT_VEL
                                              * sin(body_get_orientation(missile));
                                }
                                if (key_pressed == 'd') {
                                    body_set_rotation(missile, 
                                        body_get_orientation(missile)
                                        + ANGLE_OFFSET / (2*i + 1));
                                }
                                if (key_pressed == 'a') {
                                    body_set_rotation(missile,
                                        body_get_orientation(missile)
                                        - ANGLE_OFFSET / (2*i + 1));
                                }
                            }
                            body_set_velocity(missile, speed);
                        }
                    }
                }

                if (!missile_exists) {
                    tank_shoot(scene, tank1);
                }
            }
            else {
                list_clear(keys_pressed);
                if (type == KEY_RELEASED) {
                    body_set_velocity(tank1_body, VEC_ZERO);
                    list_clear(keys_pressed);
                } else if (type == KEY_PRESSED) {
                    vector_t speed1 = VEC_ZERO;
                    char *key_tap = malloc(sizeof(char));
                    *key_tap = key;
                    list_add(keys_pressed, key_tap);
                    for (size_t i = 0; i < list_size(keys_pressed); i++) {
                        char key_pressed = *(char *) list_get(keys_pressed, i);
                        if (key_pressed == 's') {
                            speed1.x = -INIT_VEL * cos(body_get_orientation(tank1_body));
                            speed1.y = -INIT_VEL * sin(body_get_orientation(tank1_body));
                        }
                        if (key_pressed == 'w') {
                            speed1.x = INIT_VEL * cos(body_get_orientation(tank1_body));
                            speed1.y = INIT_VEL * sin(body_get_orientation(tank1_body));
                        }
                        if (key_pressed == 'd') {
                            body_set_rotation(tank1_body, body_get_orientation(tank1_body)
                                                         + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == 'a') {
                            body_set_rotation(tank1_body, body_get_orientation(tank1_body)
                                                          - ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == ' ') {
                            if (body_get_time(tank1_body) > tank_get_curr_reload(tank1)) {
                                tank_shoot(scene, tank1);
                                tank_set_body_time(tank1, 0);
                            }
                        }
                    }
                    body_set_velocity(tank1_body, speed1);
                }
        }
        } else {
            body_t *tank1_body = tank_get_body(tank1);
            body_t *tank2_body = tank_get_body(tank2);

            if ((tank_get_weapon(tank1) == (shooting_handler_t) remote_missile_shoot) && 
                (tank_get_weapon(tank2) != (shooting_handler_t) remote_missile_shoot)) {
                body_t *missile1 = NULL;
                body_set_velocity(tank1_body, VEC_ZERO);
                for (size_t i=0; i < scene_bodies(scene); i++) {
                        if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                            == TANK_REMOTE_MISSILE_1) {
                            missile1 = scene_get_body(scene, i);
                        }
                }
                if (missile1 == NULL) {
                    tank_shoot(scene, tank1);
                    for (size_t i=0; i < scene_bodies(scene); i++) {
                        if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                            == TANK_REMOTE_MISSILE_1) {
                            missile1 = scene_get_body(scene, i);
                        }
                    }
                }
                wall_boundary(scene, missile1);
                if (type == KEY_RELEASED) {
                    body_set_velocity(missile1, VEC_ZERO);
                    body_set_velocity(tank2_body, VEC_ZERO);
                    list_clear(keys_pressed);
                } else if (type == KEY_PRESSED) {
                    vector_t speed1 = VEC_ZERO;
                    vector_t speed2 = VEC_ZERO;
                    char *key_tap = malloc(sizeof(char));
                    *key_tap = key;
                    list_add(keys_pressed, key_tap);
                    for (size_t i = 0; i < list_size(keys_pressed); i++) {
                        char key_pressed = *(char *) list_get(keys_pressed, i);
                        if (key_pressed == 's') {
                            speed1.x = -INIT_VEL * cos(body_get_orientation(missile1));
                            speed1.y = -INIT_VEL * sin(body_get_orientation(missile1));
                        }
                        if (key_pressed == 'w') {
                            speed1.x = INIT_VEL * cos(body_get_orientation(missile1));
                            speed1.y = INIT_VEL * sin(body_get_orientation(missile1));
                        }
                        if (key_pressed == 'd') {
                            body_set_rotation(missile1, body_get_orientation(missile1)
                                              + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == 'a') {
                            body_set_rotation(missile1, body_get_orientation(missile1)
                                              - ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == DOWN_ARROW) {
                            speed2.x = -INIT_VEL * cos(body_get_orientation(tank2_body));
                            speed2.y = -INIT_VEL * sin(body_get_orientation(tank2_body));
                        }
                        if (key_pressed == UP_ARROW) {
                            speed2.x = INIT_VEL * cos(body_get_orientation(tank2_body));
                            speed2.y = INIT_VEL * sin(body_get_orientation(tank2_body));
                        }
                        if (key == RIGHT_ARROW) {
                            body_set_rotation(tank2_body, body_get_orientation(tank2_body)
                                                          + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == LEFT_ARROW) {
                            body_set_rotation(tank2_body, body_get_orientation(tank2_body)
                                                          - ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == '.') {
                            if (body_get_time(tank2_body) > tank_get_curr_reload(tank2)) {
                                tank_shoot(scene, tank2);
                                tank_set_body_time(tank2, 0);
                            }
                        }
                    }
                    body_set_velocity(missile1, speed1);
                    body_set_velocity(tank2_body, speed2);
                }
            }

            else if ((tank_get_weapon(tank1) != (shooting_handler_t) remote_missile_shoot)
                 && (tank_get_weapon(tank2) == (shooting_handler_t) remote_missile_shoot))
                {
                body_t *missile2 = NULL;
                body_set_velocity(tank2_body, VEC_ZERO);
                for (size_t i=0; i < scene_bodies(scene); i++) {
                        if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                            == TANK_REMOTE_MISSILE_2) {
                            missile2 = scene_get_body(scene, i);
                        }
                }
                if (missile2 == NULL) {
                    tank_shoot(scene, tank2);
                    for (size_t i=0; i < scene_bodies(scene); i++) {
                        if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                            == TANK_REMOTE_MISSILE_2) {
                            missile2 = scene_get_body(scene, i);
                        }
                    }
                }
                wall_boundary(scene, missile2);
                if (type == KEY_RELEASED) {
                    body_set_velocity(missile2, VEC_ZERO);
                    body_set_velocity(tank1_body, VEC_ZERO);
                    list_clear(keys_pressed);
                } else if (type == KEY_PRESSED) {
                    vector_t speed1 = VEC_ZERO;
                    vector_t speed2 = VEC_ZERO;
                    char *key_tap = malloc(sizeof(char));
                    *key_tap = key;
                    list_add(keys_pressed, key_tap);
                    for (size_t i = 0; i < list_size(keys_pressed); i++) {
                        char key_pressed = *(char *) list_get(keys_pressed, i);
                        if (key_pressed == 's') {
                            speed1.x = -INIT_VEL * cos(body_get_orientation(tank1_body));
                            speed1.y = -INIT_VEL * sin(body_get_orientation(tank1_body));
                        }
                        if (key_pressed == 'w') {
                            speed1.x = INIT_VEL * cos(body_get_orientation(tank1_body));
                            speed1.y = INIT_VEL * sin(body_get_orientation(tank1_body));
                        }
                        if (key_pressed == 'd') {
                            body_set_rotation(tank1_body, body_get_orientation(tank1_body)
                                                          + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == 'a') {
                            body_set_rotation(tank1_body, body_get_orientation(tank1_body)
                                                          - ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == ' ') {
                            if (body_get_time(tank1_body) > tank_get_curr_reload(tank1)) {
                                tank_shoot(scene, tank1);
                                tank_set_body_time(tank1, 0);
                            }
                        }
                        if (key_pressed == DOWN_ARROW) {
                            speed2.x = -INIT_VEL * cos(body_get_orientation(missile2));
                            speed2.y = -INIT_VEL * sin(body_get_orientation(missile2));
                        }
                        if (key_pressed == UP_ARROW) {
                            speed2.x = INIT_VEL * cos(body_get_orientation(missile2));
                            speed2.y = INIT_VEL * sin(body_get_orientation(missile2));
                        }
                        if (key == RIGHT_ARROW) {
                            body_set_rotation(missile2, body_get_orientation(missile2)
                                                        + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == LEFT_ARROW) {
                            body_set_rotation(missile2, body_get_orientation(missile2)
                                                        - ANGLE_OFFSET / (2*i + 1));
                        }
                    }
                    body_set_velocity(tank1_body, speed1);
                    body_set_velocity(missile2, speed2);
                }
            }

            else if ((tank_get_weapon(tank1) == (shooting_handler_t) remote_missile_shoot)
                 && (tank_get_weapon(tank2) == (shooting_handler_t) remote_missile_shoot))
                {
                body_t *missile1 = NULL;
                body_t *missile2 = NULL;
                body_set_velocity(tank1_body, VEC_ZERO);
                body_set_velocity(tank2_body, VEC_ZERO);
                for (size_t i=0; i < scene_bodies(scene); i++) {
                        if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                            == TANK_REMOTE_MISSILE_1) {
                            missile1 = scene_get_body(scene, i);
                        }
                        if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                            == TANK_REMOTE_MISSILE_2) {
                            missile2 = scene_get_body(scene, i);
                        }
                }
                if (missile1 == NULL || missile2 == NULL) {
                    if (missile1 == NULL) {
                        tank_shoot(scene, tank1);
                    }
                    if (missile2 == NULL) {
                        tank_shoot(scene, tank2);
                    }
                    for (size_t i=0; i < scene_bodies(scene); i++) {
                        if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                            == TANK_REMOTE_MISSILE_1) {
                            missile1 = scene_get_body(scene, i);
                        }
                        if (*(body_types_t *) body_get_info(scene_get_body(scene, i))
                            == TANK_REMOTE_MISSILE_2) {
                            missile2 = scene_get_body(scene, i);
                        }
                    }
                }
                wall_boundary(scene, missile1);
                wall_boundary(scene, missile2);
                if (type == KEY_RELEASED) {
                    body_set_velocity(missile1, VEC_ZERO);
                    body_set_velocity(missile2, VEC_ZERO);
                    list_clear(keys_pressed);
                } else if (type == KEY_PRESSED) {
                    vector_t speed1 = VEC_ZERO;
                    vector_t speed2 = VEC_ZERO;
                    char *key_tap = malloc(sizeof(char));
                    *key_tap = key;
                    list_add(keys_pressed, key_tap);
                    for (size_t i = 0; i < list_size(keys_pressed); i++) {
                        char key_pressed = *(char *) list_get(keys_pressed, i);
                        if (key_pressed == 's') {
                            speed1.x = -INIT_VEL * cos(body_get_orientation(missile1));
                            speed1.y = -INIT_VEL * sin(body_get_orientation(missile1));
                        }
                        if (key_pressed == 'w') {
                            speed1.x = INIT_VEL * cos(body_get_orientation(missile1));
                            speed1.y = INIT_VEL * sin(body_get_orientation(missile1));
                        }
                        if (key_pressed == 'd') {
                            body_set_rotation(missile1, body_get_orientation(missile1)
                                                        + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == 'a') {
                            body_set_rotation(missile1, body_get_orientation(missile1)
                                                        - ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == DOWN_ARROW) {
                            speed2.x = -INIT_VEL * cos(body_get_orientation(missile2));
                            speed2.y = -INIT_VEL * sin(body_get_orientation(missile2));
                        }
                        if (key_pressed == UP_ARROW) {
                            speed2.x = INIT_VEL * cos(body_get_orientation(missile2));
                            speed2.y = INIT_VEL * sin(body_get_orientation(missile2));
                        }
                        if (key == RIGHT_ARROW) {
                            body_set_rotation(missile2, body_get_orientation(missile2)
                                                        + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == LEFT_ARROW) {
                            body_set_rotation(missile2, body_get_orientation(missile2)
                                                        - ANGLE_OFFSET / (2*i + 1));
                        }
                    }
                    body_set_velocity(missile1, speed1);
                    body_set_velocity(missile2, speed2);
                }
            }
            else {
                if (type == KEY_RELEASED) {
                    body_set_velocity(tank1_body, VEC_ZERO);
                    body_set_velocity(tank2_body, VEC_ZERO);
                    list_clear(keys_pressed);
                } else if (type == KEY_PRESSED) {
                    vector_t speed1 = VEC_ZERO;
                    vector_t speed2 = VEC_ZERO;
                    char *key_tap = malloc(sizeof(char));
                    *key_tap = key;
                    list_add(keys_pressed, key_tap);
                    for (size_t i = 0; i < list_size(keys_pressed); i++) {
                        char key_pressed = *(char *) list_get(keys_pressed, i);
                        if (key_pressed == 's') {
                            speed1.x = -INIT_VEL * cos(body_get_orientation(tank1_body));
                            speed1.y = -INIT_VEL * sin(body_get_orientation(tank1_body));
                        }
                        if (key_pressed == 'w') {
                            speed1.x = INIT_VEL * cos(body_get_orientation(tank1_body));
                            speed1.y = INIT_VEL * sin(body_get_orientation(tank1_body));
                        }
                        if (key_pressed == 'd') {
                            body_set_rotation(tank1_body, body_get_orientation(tank1_body)
                                                          + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == 'a') {
                            body_set_rotation(tank1_body, body_get_orientation(tank1_body)
                                                          - ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == ' ') {
                            if (body_get_time(tank1_body) > tank_get_curr_reload(tank1)) {
                                tank_shoot(scene, tank1);
                                tank_set_body_time(tank1, 0);
                            }
                        }
                        if (key_pressed == DOWN_ARROW) {
                            speed2.x = -INIT_VEL * cos(body_get_orientation(tank2_body));
                            speed2.y = -INIT_VEL * sin(body_get_orientation(tank2_body));
                        }
                        if (key_pressed == UP_ARROW) {
                            speed2.x = INIT_VEL * cos(body_get_orientation(tank2_body));
                            speed2.y = INIT_VEL * sin(body_get_orientation(tank2_body));
                        }
                        if (key == RIGHT_ARROW) {
                            body_set_rotation(tank2_body, body_get_orientation(tank2_body)
                                                          + ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == LEFT_ARROW) {
                            body_set_rotation(tank2_body, body_get_orientation(tank2_body)
                                                          - ANGLE_OFFSET / (2*i + 1));
                        }
                        if (key_pressed == '.') {
                            if (body_get_time(tank2_body) > tank_get_curr_reload(tank2)) {
                                tank_shoot(scene, tank2);
                                tank_set_body_time(tank2, 0);
                            }
                        }
                    }
                    body_set_velocity(tank1_body, speed1);
                    body_set_velocity(tank2_body, speed2);
                }
            }
        }
    }
}

bool within_rect(body_t *body, vector_t point) {
    list_t *list = body_get_shape(body);
    assert(list_size(list) == get_num_rect_pts());
    bool within = (point.x >= ((vector_t *)list_get(list, BOTTOM_LEFT_CORNER))->x
                  && point.x <= ((vector_t *)list_get(list, TOP_RIGHT_CORNER))->x
                  && point.y <= ((vector_t *)list_get(list, TOP_LEFT_CORNER))->y
                  && point.y >= ((vector_t *)list_get(list, BOTTOM_LEFT_CORNER))->y);

    list_free(list);
    return within;
}

void on_mouse(scene_t *scene, vector_t point, bool *play, scene_t **scenes, int *level,
              bool *multi, bool *choosing_level, bool *game_over, bool *game_started,
              int *unlocked_level, tank_t *tank1, tank_t* tank2) {
    if (*play) {
        if (within_rect(scene_get_body(scenes[PLAY], PAUSE_BUTTON), point)){
            *play = false;
        }
    } else {
        if (!(*game_over || *game_started) && !(*choosing_level)
              && within_rect(scene_get_body(scenes[PAUSE], RESUME_BUT), point)) {
            *play = true;
        } else if (!(*game_over || *game_started) && !(*choosing_level)
                   && within_rect(scene_get_body(scenes[PAUSE], RESTART_BUT), point)) {
            printf("clicked restart\n");
            scene_erase_some(scenes[PLAY], FIRST_REMOVABLE_INDEX);
            screen_set_tanks(scenes[PLAY], multi, tank1, tank2);
            if (*level == 1) {
                level_1(get_top_right(), LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT,
                        scenes[PLAY], multi);
            } else if (*level == 2) {
                level_2(get_top_right(), LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT,
                        scenes[PLAY], multi);
            } else if (*level == 3) {
                level_3(get_top_right(), LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT,
                        scenes[PLAY], multi);
            }
            *play = true;
        } else if (within_rect(scene_get_body(scenes[PAUSE], EASY_BUT), point)
                   && *unlocked_level >= get_first_level()) {
            printf("clicked easy\n");
            if (*choosing_level) {
                scene_remove_body(scenes[PAUSE], WHITE_SCREEN);
                *choosing_level = false;
            } else if (*game_over) {
                scene_remove_body(scenes[PAUSE], WHITE_SCREEN);
                *game_over = false;
            } else if (*game_started) {
                scene_remove_body(scenes[PAUSE], WHITE_SCREEN);
                *game_started = false;
            }
            scene_erase_some(scenes[PLAY], FIRST_REMOVABLE_INDEX);
            screen_set_tanks(scenes[PLAY], multi, tank1, tank2);
            level_1(get_top_right(), LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT,
                    scenes[PLAY], multi);
            *level = get_first_level();
            *play = true;
        } else if (within_rect(scene_get_body(scenes[PAUSE], MEDIUM_BUT), point)
                   &&  *unlocked_level >= get_second_level()) {
            printf("clicked medium\n");
            if (*choosing_level) {
                scene_remove_body(scene, WHITE_SCREEN);
                *choosing_level = false;
            } else if (*game_over) {
                scene_remove_body(scene, WHITE_SCREEN);
                *game_over = false;
            } else if (*game_started) {
                scene_remove_body(scenes[PAUSE], WHITE_SCREEN);
                *game_started = false;
            }
            scene_erase_some(scenes[PLAY], FIRST_REMOVABLE_INDEX);
            screen_set_tanks(scenes[PLAY], multi, tank1, tank2);
            level_2(get_top_right(), LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT,
                    scenes[PLAY], multi);
            *level = get_second_level();
            *play = true;
        } else if (within_rect(scene_get_body(scenes[PAUSE], HARD_BUT), point)
                   &&  *unlocked_level >= get_third_level()) {
            printf("clicked hard\n");
            if (*choosing_level) {
                scene_remove_body(scene, WHITE_SCREEN);
                *choosing_level = false;
            } else if (*game_over) {
                scene_remove_body(scene, WHITE_SCREEN);
                *game_over = false;
            } else if (*game_started) {
                scene_remove_body(scenes[PAUSE], WHITE_SCREEN);
                *game_started = false;
            }
            scene_erase_some(scenes[PLAY], FIRST_REMOVABLE_INDEX);
            screen_set_tanks(scenes[PLAY], multi, tank1, tank2);
            level_3(get_top_right(), LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT,
                    scenes[PLAY], multi);
            *level = get_third_level();
            *play = true;
        } else if (!(*choosing_level) && within_rect(scene_get_body(scenes[PAUSE],
                   CHOOSE_PLAYER_BOX), point)) {
            *multi = !(*multi);
            *choosing_level = true;
            if (*game_over) {
                scene_remove_body(scene, WHITE_SCREEN);
                *game_over = false;
            } else if (*game_started) {
                scene_remove_body(scene, WHITE_SCREEN);
                *game_started = false;
            }
            vector_t center = {get_top_right().x/2.0, 3.0*get_top_right().y/4.0};
            screen_set_rect(scene, center, get_top_right().x,
                              get_top_right().y/2.0, WHITE_SCREEN, color_get_white());
        }
    }
}

int main(int argc, char *argv[]) {
    sdl_init(get_bottom_left(), get_top_right());
    scene_t *scene = scene_init();

    screen_set_pause_button(scene);
    tank_t *tank1 = screen_set_new_tank(scene, (body_types_t) TANK_1,
                                    get_tank1_init_pos());
    body_set_lives(tank_get_body(tank1), get_init_lives());
    tank_t *tank2 = screen_set_new_tank(scene, (body_types_t) TANK_2,
                                    get_tank2_off_screen());
    bool *multi = malloc(sizeof(bool));
    *multi = false;
    level_1(get_top_right(), LEVEL_1_WALL_LENGTH, LEVEL_1_WALL_HEIGHT, scene, multi);
    int *level = malloc(sizeof(int));
    *level = get_first_level();

    scene_t *pause_scene = scene_init();
    screen_set_pause_screen(pause_scene);
    bool *play = malloc(sizeof(bool));
    *play = true;
    bool *choosing_level = malloc(sizeof(bool));
    *choosing_level = false;
    bool *game_over = malloc(sizeof(bool));
    *game_over = false;
    bool *game_started = malloc(sizeof(bool));
    *game_started = true;
    int *unlocked_level = malloc(sizeof(int));
    *unlocked_level = get_third_level();
    text_open_font();
    image_load();
    
    sdl_on_key((key_handler_t)on_key_push);
    sdl_on_mouse((mouse_handler_t)on_mouse);

    double time_passed = START_TIME;
    double shoot_time = START_TIME;
    scene_t *temp_scene = malloc(sizeof(scene_t *));
    temp_scene = scene;
    scene_t **scenes = malloc(sizeof(scene_t *) * NUM_SCENES);
    scenes[PLAY] = scene;
    scenes[PAUSE] = pause_scene;

    vector_t *tank_center = malloc(sizeof(vector_t));
    vector_t center = {rand() % (int)get_top_right().x, rand() % (int)get_top_right().y};
    *tank_center = center;

    list_t *keys_pressed = list_init(0, free);
    game_outcomes_t win;

    load_sound();

    while (!sdl_is_done(temp_scene, scene_get_body(temp_scene, 0), play, scenes, level,
                        multi, choosing_level, tank1, tank2, game_over, keys_pressed,
                        game_started, unlocked_level)) {
        double dt = time_since_last_tick();
        if (*play) {
            win = gameplay_find_winner(scene, tank1, tank2, multi, game_over);
            if (win >=1 && win <= 3) {
                if (*level == get_second_level()) {
                    *unlocked_level = get_third_level();
                } else if (*level == get_first_level() && *unlocked_level
                           == get_first_level()) {  
                    *unlocked_level = get_second_level();
                }
            }

            if ((*game_over || *game_started)) {
                *play = false;
                vector_t center = {get_top_right().x/2.0, 3*get_top_right().y/4.0};
                screen_set_rect(pause_scene, center, get_top_right().x,
                                get_top_right().y/2 - get_choose_level_height(),
                                WHITE_SCREEN, color_get_white());
            }
        }


        if (*play) {
            temp_scene = scene;
            time_passed += dt;
            shoot_time += dt;
        
            // Have the enemy tank(s) shoot after a certain period of time
            if (shoot_time >= TANK_SHOOT_TIME) {
                vector_t player_loc;
                for(int i = 0; i < scene_bodies(scene); i++) {
                    if (*(body_types_t *)body_get_info(scene_get_body(scene, i))
                        == TANK_1) {
                        player_loc = body_get_centroid(scene_get_body(scene, i));
                    }
                }
                enemy_tank_shoot(scene, level, player_loc);
                shoot_time = START_TIME;
            }

            update_and_check_projectiles_and_tanks(scene, tank1, dt);

            if (*multi) {
                update_and_check_projectiles_and_tanks(scene, tank2, dt);
                handle_force_field(scene, tank2, dt);
            }

            handle_force_field(scene, tank1, dt);
            
            wall_boundary(scene, tank_get_body(tank1));
            wall_boundary(scene, tank_get_body(tank2));

            for(size_t i = 0; i < scene_bodies(scene); i++) { 
                if (*(body_types_t *)body_get_info(scene_get_body(scene, i)) == ENEMY_TANK
                    && !body_is_powerup(scene_get_body(scene, i))) {
                    body_hit_boundary_check(scene_get_body(scene, i), get_bottom_left(),
                    get_top_right(), dt);
                }
            }

            // Shoot a power-up at an interval of time.
            if (time_passed > TANK_POWER_UP_TIME) {
                make_tank_power_up(temp_scene, rand() % NUM_POWERUPS, tank1, tank2);
                time_passed = START_TIME;
            }
        } else {
            temp_scene = pause_scene;
            time_passed = START_TIME;
        }

        sdl_render_scene(temp_scene);
        if (*play) {
            text_for_play(temp_scene, multi, tank1, tank2);
        } else {
            text_for_pause(temp_scene, multi,  choosing_level, win, game_started);
            image_pause_screen(temp_scene, unlocked_level);
        }
        sdl_show();
        
        scene_tick(temp_scene, dt);

        // Keep playing music
        continue;
    }

    free_sound();
    image_free();
    text_free();
    free(play);
    free(multi);
    free(choosing_level);
    free(game_over);
    free(game_started);
    scene_free(pause_scene);
    scene_free(scene);
    free(scenes);
}
