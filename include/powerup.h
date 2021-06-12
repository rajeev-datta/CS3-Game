#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "scene.h"
#include "tank.h"

typedef struct tank_powerup_aux tank_powerup_aux_t;

typedef enum powerups{
    MACHINE_GUN,
    FRAG_BOMB,
    LAND_MINE,
    FORCE_FIELD,
    REMOTE_MISSILE,
    INC_LIVES,
} powerups_t;

tank_powerup_aux_t *tank_powerup_aux_init(tank_t *tank, powerups_t type);

void default_gun_shoot(scene_t *scene, body_t *body);

void machine_gun_shoot(scene_t *scene, body_t *body);

void frag_bomb_shoot(scene_t *scene, body_t *body);

void land_mine_shoot(scene_t *scene, body_t *body);

void force_field_shoot(scene_t *scene, body_t *body);

void remote_missile_shoot(scene_t *scene, body_t *body);

void increase_lives(body_t *body);

void create_tank_powerup_collision(scene_t *scene, tank_t *tank, body_t *powerup, powerups_t type);

void make_tank_power_up(scene_t *scene, int type, tank_t *tank, tank_t *tank_2);

void update_and_check_projectiles_and_tanks(scene_t *scene, tank_t *tank, double dt);

body_t *create_new_force_field(scene_t *scene, tank_t *tank);

void handle_force_field(scene_t *scene, tank_t *tank, double dt);

#endif // #ifndef __POWERUP_H__