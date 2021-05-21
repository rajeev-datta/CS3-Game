#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "scene.h"

void machine_gun_shoot(scene_t *scene, void *body);

void frag_bomb_shoot(scene_t *scene, void *body);

void land_mine_shoot(scene_t *scene, void *body);

void force_field_shoot(scene_t *scene, void *body);

void laser_shoot(scene_t *scene, void *body);

void remote_missle_shoot(scene_t *scene, void *body);

#endif // #ifndef __POWERUP_H__