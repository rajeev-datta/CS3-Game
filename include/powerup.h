#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "scene.h"
#include "tank.h"

typedef enum powerups{
    MACHINE_GUN,
    FRAG_BOMB,
    LAND_MINE,
    FORCE_FIELD,
    REMOTE_MISSILE,
} powerups_t;

void default_gun_shoot(scene_t *scene, body_t *body);

void machine_gun_shoot(scene_t *scene, body_t *body);

void frag_bomb_shoot(scene_t *scene, body_t *body);

void land_mine_shoot(scene_t *scene, body_t *body);

void force_field_shoot(scene_t *scene, body_t *body);

void remote_missile_shoot(scene_t *scene, body_t *body);

void create_tank_powerup_collision(scene_t *scene, tank_t *tank, body_t *powerup, powerups_t type);

#endif // #ifndef __POWERUP_H__