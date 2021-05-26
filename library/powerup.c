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
#include "tank.h"

const int CIRCLE_PTS = 16;
const double BULLET_RADIUS = 5;
const vector_t TANK_BULLET_INIT_VEL = {0,100}; // will need to change so it is pointing in the direction of tank
const double BULLET_ELASTICITY = 0.9;
const double MACHINE_GUN_RELOAD_TIME = 0.5;

typedef struct tank_powerup_aux {
    tank_t *tank;
    char type;
} tank_powerup_aux_t;

powerup_aux_t *powerup_aux_init(tank_t *tank, char type) {
    tank_powerup_aux_t *tank_pow_aux_obj = malloc(sizeof(tank_powerup_aux_t));
    tank_pow_aux_obj->tank = tank;
    tank_pow_aux_obj->type = type;
    return tank_pow_aux_obj;
}

// give tank object a handler for shooting that is the current weapon
// default handler must be saved in the object as well to make sure that once
// the power up's time limit is over, the tank returns to old shooting style
// tank should have a double field for reload speed

void default_gun_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of normal gun
    list_t *bullet = animate_circle(body_get_centroid(body), BULLET_RADIUS,
                                       CIRCLE_PTS);
    char *tank_bullet_info = malloc(sizeof(char *));
    *tank_bullet_info = TANK_BULLET;
    body_t *bullet_body = body_init_with_info(bullet, BULLET_MASS,
                                              GREEN, tank_bullet_info, free);
    body_set_velocity(bullet_body, TANK_BULLET_INIT_VEL);
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(char *) body_get_info(scene_get_body(scene, i)) == TANK_INFO_1 || *(char *) body_get_info(scene_get_body(scene, i)) == TANK_INFO_2) {
            create_destructive_collision(scene, bullet_body, scene_get_body(scene, i));
        }
        if (*(char *) body_get_info(scene_get_body(scene, i)) == WALL_INFO) {
            create_physics_collision(scene, BULLET_ELASTICITY, bullet_body, scene_get_body(scene, i));
        }
    }
    scene_add_body(scene, bullet_body);
}

void machine_gun_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of machine gun
}

void frag_bomb_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of frag bomb
}

void land_mine_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of machine gun
}

void force_field_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of machine gun
}

void laser_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of machine gun
}

void remote_missle_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of machine gun
}

void tank_powerup_fxn(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    body_remove(body2);

    if (((tank_powerup_aux_t *)aux)->type == (char) 0) {
        // machine gun power up
        tank_set_new_reload_time(((tank_powerup_aux_t *)aux)->tank, MACHINE_GUN_RELOAD_TIME);
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) machine_gun_shoot);
    } else if (((tank_powerup_aux_t *)aux)->type == (char) 1) {
        //frag bomb power up
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) frag_bomb_shoot);
    } else if (((tank_powerup_aux_t *)aux)->type == (char) 2) {
        //land mine power up
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) land_mine_shoot);
    } else if (((tank_powerup_aux_t *)aux)->type == (char) 3) {
        // force field power up
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) force_field_shoot);
    } else if (((tank_powerup_aux_t *)aux)->type == (char) 4) {
        // laser power up
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) laser_shoot);
    } else {
        // remote controlled missle power up
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) remote_missle_shoot);
    }
}

void create_tank_powerup_collision(scene_t *scene, tank_t *tank, body_t *powerup, char type) {
    tank_powerup_aux_t *tank_pow_aux = tank_powerup_aux_init(tank, type);
    body_t *body1 = tank_get_body(tank);
    create_collision(scene, body1, powerup, (collision_handler_t) tank_powerup_fxn,
                    tank_pow_aux, (free_func_t) free);
}
