#include <assert.h>
#include <stdlib.h>
#include "list.h"
#include "vector.h"
#include "scene.h"
#include "body.h"
#include <math.h>
#include "test_util.h"
#include "collision.h"
#include "forces.h"
#include <float.h>
#include <stdbool.h>
#include "animate.h"

// give tank object a handler for shooting that is the current weapon
// default handler must be saved in the object as well to make sure that once
// the power up's time limit is over, the tank returns to old shooting style

void machine_gun_shoot(scene_t *scene, void *body) {
    // method to handle the shooting of machine gun
}

void frag_bomb_shoot(scene_t *scene, void *body) {
    // method to handle the shooting of frag bomb
}

void land_mine_shoot(scene_t *scene, void *body) {
    // method to handle the shooting of machine gun
}

void force_field_shoot(scene_t *scene, void *body) {
    // method to handle the shooting of machine gun
}

void laser_shoot(scene_t *scene, void *body) {
    // method to handle the shooting of machine gun
}

void remote_missle_shoot(scene_t *scene, void *body) {
    // method to handle the shooting of machine gun
}

void tank_powerup_fxn(body_t *body1, body_t *body2, vector_t axis, void *aux) {

}

void create_tank_powerup_collision(scene_t *scene, body_t *body1, body_t *powerup, char type) {
    powerup_aux_t *pow_aux = powerup_aux_init(powerup, type);
    create_collision(scene, body1, powerup, (collision_handler_t) tank_powerup_fxn,
                    pow_aux, (free_func_t) free);
}
