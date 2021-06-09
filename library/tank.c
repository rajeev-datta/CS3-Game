#include "tank.h"
#include "powerup.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sound.h"

static const int MASS = 100;
static const double default_reload_time = 1;
static const double default_bullet_range = 7;
static const double POWERUP_TIME = 10;

typedef struct tank {
    body_t *shape;
    shooting_handler_t default_weapon;
    shooting_handler_t weapon;
    double default_reload;
    double new_reload_time;
    double default_range;
    double new_range;
    double curr_powerup_time;
    double total_powerup_time;
} tank_t;

tank_t *tank_init(list_t *points, void* info) {
    tank_t *tank_obj = malloc(sizeof(tank_t));
    
    body_t *shape = body_init_with_info(points, MASS, color_get_red(), info, free);
    if (*(body_types_t *) info == TANK_2) {
        body_set_rotation(shape, M_PI);
    }
    tank_obj->shape = shape;
    
    tank_obj->weapon = NULL;
    tank_obj->default_weapon = (shooting_handler_t) default_gun_shoot;
    tank_obj->default_reload = default_reload_time;
    tank_obj->new_reload_time = 0;
    tank_obj->default_range = default_bullet_range;
    tank_obj->new_range = 0;
    tank_obj->total_powerup_time = POWERUP_TIME;
    tank_obj->curr_powerup_time = 0;
    return tank_obj;
}

tank_t *enemy_tank_init(list_t *points, vector_t speed, void* info) {
    tank_t *tank_obj = malloc(sizeof(tank_t));
    
    body_t *shape = body_init_with_info(points, MASS, color_get_black(), info, free);
    body_set_velocity(shape, speed);
    body_set_rotation(shape, M_PI);
    tank_obj->shape = shape;

    tank_obj->weapon = NULL;
    tank_obj->default_weapon = (shooting_handler_t) default_gun_shoot;
    tank_obj->default_reload = default_reload_time;
    tank_obj->new_reload_time = 0;
    tank_obj->default_range = default_bullet_range;
    tank_obj->new_range = 0;
    tank_obj->total_powerup_time = POWERUP_TIME;
    tank_obj->curr_powerup_time = 0;
    return tank_obj;
}

body_t *tank_get_body(tank_t *tank) {
    return tank->shape;
}

void tank_set_shooting_handler(tank_t *tank, shooting_handler_t new_weapon) {
    tank->weapon = new_weapon;
}

void tank_set_new_reload_time(tank_t *tank, double new_reload_time) {
    tank->new_reload_time = new_reload_time;
}

void tank_set_new_range(tank_t *tank, double new_bullet_range) {
    tank->new_range = new_bullet_range;
}

shooting_handler_t tank_get_weapon(tank_t *tank) {
    return tank->weapon;
}

shooting_handler_t tank_get_default_weapon(tank_t *tank) {
    return tank->default_weapon;
}

double tank_get_default_range(tank_t *tank) {
    return tank->default_range;
}

void tank_shoot(scene_t *scene, tank_t *tank) {
    play_shoot_sound();
    body_t *tank_body = tank_get_body(tank);
    if (tank_get_weapon(tank) != NULL && tank_get_weapon(tank) != (shooting_handler_t) force_field_shoot) {
        shooting_handler_t weapon = tank_get_weapon(tank);
        weapon(scene, tank_body);
    }

    else {
        shooting_handler_t weapon = tank_get_default_weapon(tank);
        weapon(scene, tank_body);
    }
}

double tank_get_curr_reload(tank_t *tank) {
    if (tank->new_reload_time == 0) {
        return tank->default_reload;
    }
    else {
        return tank->new_reload_time;
    }
}

double tank_get_curr_range(tank_t *tank) {
    if (tank->new_range == 0 || tank_get_weapon(tank) == (shooting_handler_t) force_field_shoot) {
        return tank->default_range;
    }
    else {
        return tank->new_range;
    }
}

void tank_increase_powerup_time(tank_t *tank, double time_increment) {
    double curr_time = tank->curr_powerup_time;
    curr_time += time_increment;
    tank->curr_powerup_time = curr_time;
}

double tank_get_total_powerup_time(tank_t *tank) {
    return tank->total_powerup_time;
}

double tank_get_curr_powerup_time(tank_t *tank) {
    return tank->curr_powerup_time;
}

void tank_set_powerup_time(tank_t *tank, double time) {
    tank->curr_powerup_time = time;
}

double tank_get_new_weapon_range(tank_t *tank) {
    return tank->new_range;
}

void tank_increase_body_time(tank_t *tank, double time_increment) {
    body_increase_time(tank_get_body(tank), time_increment);
}

void tank_set_body_time(tank_t *tank, double time) {
    body_set_time(tank_get_body(tank), time);
}
