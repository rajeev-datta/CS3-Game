#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "scene.h"

void machine_gun_shoot(scene_t *scene, void *body);

void frag_bomb_shoot(scene_t *scene, void *body);

void land_mine_shoot(scene_t *scene, void *body);

void force_field_shoot(scene_t *scene, void *body);

void laser_shoot(scene_t *scene, void *body);

void remote_missle_shoot(scene_t *scene, void *body);

void create_tank_powerup_collision(scene_t *scene, body_t *body1, body_t *powerup, char type);

#endif // #ifndef __POWERUP_H__