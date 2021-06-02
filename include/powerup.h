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
} powerups_t;

// bullets might be destroying powerups because the corresponding values for the powerups and body types are equal and thus
// the bullets think it is a tank

// machine gun powerup works
// land mine powerup works
// force field powerup creates a force field and then is removed alongside the tank itself... so doesn't work
// remote missile causes an immediate assertion fail
// frag bomb check seems to be the reason behind the freezing problem

tank_powerup_aux_t *tank_powerup_aux_init(tank_t *tank, powerups_t type);

void default_gun_shoot(scene_t *scene, body_t *body);

void machine_gun_shoot(scene_t *scene, body_t *body);

void frag_bomb_shoot(scene_t *scene, body_t *body);

void land_mine_shoot(scene_t *scene, body_t *body);

void force_field_shoot(scene_t *scene, body_t *body);

void remote_missile_shoot(scene_t *scene, body_t *body);

void create_tank_powerup_collision(scene_t *scene, tank_t *tank, body_t *powerup, powerups_t type);

#endif // #ifndef __POWERUP_H__