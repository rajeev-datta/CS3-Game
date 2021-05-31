#ifndef __TANK_H__
#define __TANK_H__

#include "body.h"
#include "scene.h"

const double FORCE_FIELD_TIME_LIMIT = 5;
const rgb_color_t RED = {1, 0, 0};
const rgb_color_t BLACK = {0, 0, 0};

typedef struct tank tank_t;

typedef void (*shooting_handler_t)
    (scene_t *scene, body_t *body);

tank_t *tank_init(list_t *points);

tank_t *enemy_tank_init(list_t *points, vector_t speed);

body_t *tank_get_body(tank_t *tank);

void tank_set_shooting_handler(tank_t *tank, shooting_handler_t new_weapon);

void tank_set_new_reload_time(tank_t *tank, double new_reload_time);

void tank_set_new_range(tank_t *tank, double new_bullet_range);

shooting_handler_t tank_get_weapon(tank_t *tank);

shooting_handler_t tank_get_default_weapon(tank_t *tank);

double tank_get_default_range(tank_t *tank);

void tank_shoot(scene_t *scene, tank_t *tank);

double tank_get_curr_reload(tank_t *tank);

double tank_get_curr_range(tank_t *tank);

void tank_increase_powerup_time(tank_t *tank, double time_increment);

double tank_get_total_powerup_time(tank_t *tank);

double tank_get_curr_powerup_time(tank_t *tank);

void tank_set_powerup_time(tank_t *tank, double time);

#endif // #ifndef __TANK_H__