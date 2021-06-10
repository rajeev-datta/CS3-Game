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
#include "powerup.h"
#include <stdio.h>

static const int CIRCLE_PTS = 16;
static const double BULLET_RADIUS = 5;
static const double MACHINE_GUN_BULLET_RADIUS = 2.5;
static const double TANK_BULLET_INIT_VEL = 300;
static const double BULLET_ELASTICITY = 0.9;
static const double MACHINE_GUN_RELOAD_TIME = 0.2;
static const double MACHINE_GUN_RANGE = 3;
static const double FRAG_BOMB_RELOAD_TIME = 1;
static const double FRAG_BOMB_RADIUS = 7;
static const double BULLET_MASS = 100;
static const double FRAG_BOMB_RANGE = 1;
static const double LAND_MINE_RELOAD_TIME = 1;
static const double LAND_MINE_TIME_LIMIT = 10;
static const double LAND_MINE_SIDE_LENGTH = 7;
static const double FORCE_FIELD_TIME_LIMIT = 3;
static const double FACTOR = 42.0;
static const double MACHINE_GUN_FACTOR = 40;
static const double FRAG_BOMB_FACTOR = 45;
static const double MINE_FACTOR = 45;
static const double MISSILE_FACTOR = 60;
static const double MISSILE_LENGTH = 20;
static const double MISSILE_HEIGHT = 10;
static const double MISSILE_RELOAD_TIME = 10;
static const double MISSILE_RANGE = 10;
static const double FORCE_FIELD_FACTOR = 80;

typedef struct tank_powerup_aux {
    tank_t *tank;
    powerups_t type;
} tank_powerup_aux_t;

tank_powerup_aux_t *tank_powerup_aux_init(tank_t *tank, powerups_t type) {
    tank_powerup_aux_t *tank_pow_aux_obj = malloc(sizeof(tank_powerup_aux_t));
    tank_pow_aux_obj->tank = tank;
    tank_pow_aux_obj->type = type;
    return tank_pow_aux_obj;
}

void default_gun_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of normal gun
    vector_t off_center;
    if (!body_has_force_field(body)) {
        off_center.x = FACTOR * cos(body_get_orientation(body));
        off_center.y = FACTOR * sin(body_get_orientation(body));
    } else {
        off_center.x = FORCE_FIELD_FACTOR * cos(body_get_orientation(body));
    off_center.y = FORCE_FIELD_FACTOR * sin(body_get_orientation(body));
    }
    vector_t bullet_center = vec_add(body_get_centroid(body), off_center);
    list_t *bullet = animate_circle(bullet_center, BULLET_RADIUS,
                                       CIRCLE_PTS);
    body_types_t *tank_bullet_info = malloc(sizeof(body_types_t *));
    *tank_bullet_info = BULLET;
    body_t *bullet_body = body_init_with_info(bullet, BULLET_MASS,
                                              color_get_green(), tank_bullet_info, free);
    vector_t tank_bullet_init_velocity;
    tank_bullet_init_velocity.x = TANK_BULLET_INIT_VEL * cos(body_get_orientation(body));
    tank_bullet_init_velocity.y = TANK_BULLET_INIT_VEL * sin(body_get_orientation(body));
    body_set_velocity(bullet_body, tank_bullet_init_velocity);

    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (!body_is_powerup(scene_get_body(scene, i))) {
            if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_1 || *(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_2) {
                create_destructive_collision(scene, bullet_body, scene_get_body(scene, i));
            }
            if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == ENEMY_TANK) {
                create_destructive_collision(scene, bullet_body, scene_get_body(scene, i));
            }
            if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == WALL) {
                create_physics_collision(scene, BULLET_ELASTICITY, bullet_body, scene_get_body(scene, i));
            }
            if(*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_FORCE_FIELD) {
                create_partial_destructive_collision(scene, scene_get_body(scene, i), bullet_body);
            }
        }
    }
    scene_add_body(scene, bullet_body);
}

void machine_gun_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of machine gun
    vector_t off_center;
    off_center.x = MACHINE_GUN_FACTOR * cos(body_get_orientation(body));
    off_center.y = MACHINE_GUN_FACTOR * sin(body_get_orientation(body));
    vector_t bullet_center = vec_add(body_get_centroid(body), off_center);
    list_t *bullet = animate_circle(bullet_center, MACHINE_GUN_BULLET_RADIUS,
                                       CIRCLE_PTS);
    body_types_t *tank_bullet_info = malloc(sizeof(body_types_t *));
    *tank_bullet_info = BULLET;
    body_t *bullet_body = body_init_with_info(bullet, BULLET_MASS,
                                              color_get_red(), tank_bullet_info, free);

    vector_t tank_bullet_init_velocity;
    tank_bullet_init_velocity.x = TANK_BULLET_INIT_VEL * cos(body_get_orientation(body));
    tank_bullet_init_velocity.y = TANK_BULLET_INIT_VEL * sin(body_get_orientation(body));
    body_set_velocity(bullet_body, tank_bullet_init_velocity);

    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_1 || *(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_2) {
            create_destructive_collision(scene, bullet_body, scene_get_body(scene, i));
        }
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == ENEMY_TANK) {
            create_destructive_collision(scene, bullet_body, scene_get_body(scene, i));
        }
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == WALL) {
            create_physics_collision(scene, BULLET_ELASTICITY, bullet_body, scene_get_body(scene, i));
        }
    }
    scene_add_body(scene, bullet_body);
}

void frag_bomb_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of frag bomb
    vector_t off_center;
    off_center.x = FRAG_BOMB_FACTOR * cos(body_get_orientation(body));
    off_center.y = FRAG_BOMB_FACTOR * sin(body_get_orientation(body));
    vector_t bullet_center = vec_add(body_get_centroid(body), off_center);
    list_t *bullet = animate_circle(bullet_center, FRAG_BOMB_RADIUS,
                                       CIRCLE_PTS);
    body_types_t *tank_frag_bomb_info = malloc(sizeof(body_types_t *));
    *tank_frag_bomb_info = TANK_FRAG_BOMB;
    body_t *frag_bomb_body = body_init_with_info(bullet, BULLET_MASS,
                                              color_get_black(), tank_frag_bomb_info, free);
    vector_t tank_bullet_init_velocity;
    tank_bullet_init_velocity.x = TANK_BULLET_INIT_VEL * cos(body_get_orientation(body));
    tank_bullet_init_velocity.y = TANK_BULLET_INIT_VEL * sin(body_get_orientation(body));

    body_set_velocity(frag_bomb_body, tank_bullet_init_velocity);
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_1 || *(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_2) {
            create_destructive_collision(scene, frag_bomb_body, scene_get_body(scene, i));
        }
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == ENEMY_TANK) {
            create_destructive_collision(scene, frag_bomb_body, scene_get_body(scene, i));
        }
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == WALL) {
            create_physics_collision(scene, BULLET_ELASTICITY, frag_bomb_body, scene_get_body(scene, i));
        }
    }
    scene_add_body(scene, frag_bomb_body);
}

void land_mine_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of land mine
    vector_t off_center;
    off_center.x = MINE_FACTOR * cos(body_get_orientation(body));
    off_center.y = MINE_FACTOR * sin(body_get_orientation(body));
    vector_t bullet_center = vec_add(body_get_centroid(body), off_center);
    list_t *mine = animate_rectangle(bullet_center, LAND_MINE_SIDE_LENGTH,
                                       LAND_MINE_SIDE_LENGTH);
    body_types_t *tank_land_mine_info = malloc(sizeof(body_types_t *));
    *tank_land_mine_info = TANK_LAND_MINE;
    body_t *land_mine_body = body_init_with_info(mine, BULLET_MASS,
                                              color_get_purple(), tank_land_mine_info, free);

    body_set_velocity(land_mine_body, (vector_t) {0, 0});
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_1 || *(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_2) {
            create_destructive_collision(scene, land_mine_body, scene_get_body(scene, i));
        }
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == ENEMY_TANK) {
            create_destructive_collision(scene, land_mine_body, scene_get_body(scene, i));
        }
    }
    scene_add_body(scene, land_mine_body);
}

void force_field_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of force field
    // placeholder to show the tank is currently using a force field
}

void remote_missile_shoot(scene_t *scene, body_t *body) {
    // method to handle the shooting of remote missile

    vector_t off_center;
    off_center.x = MISSILE_FACTOR * cos(body_get_orientation(body));
    off_center.y = MISSILE_FACTOR * sin(body_get_orientation(body));
    vector_t bullet_center = vec_add(body_get_centroid(body), off_center);
    list_t *missile = animate_rectangle(bullet_center, MISSILE_LENGTH,
                                       MISSILE_HEIGHT);
    body_types_t *tank_missile_info = malloc(sizeof(body_types_t *));
    *tank_missile_info = TANK_REMOTE_MISSILE_1;
    body_t *missile_body = body_init_with_info(missile, BULLET_MASS,
                                              color_get_purple(), tank_missile_info, free);

    // vector_t tank_bullet_init_velocity;
    // tank_bullet_init_velocity.x = TANK_BULLET_INIT_VEL * cos(body_get_orientation(body));
    // tank_bullet_init_velocity.y = TANK_BULLET_INIT_VEL * sin(body_get_orientation(body));
    body_set_velocity(missile_body, (vector_t) {0, 0});
    body_set_rotation(missile_body, body_get_orientation(body));


    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_1 || *(body_types_t *) body_get_info(scene_get_body(scene, i)) == TANK_2) {
            create_destructive_collision(scene, missile_body, scene_get_body(scene, i));
        }
        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == ENEMY_TANK) {
            create_destructive_collision(scene, missile_body, scene_get_body(scene, i));
        }

        if (*(body_types_t *) body_get_info(scene_get_body(scene, i)) == WALL) {
            create_physics_collision(scene, BULLET_ELASTICITY, missile_body, scene_get_body(scene, i));
        }
    }
    scene_add_body(scene, missile_body);
}

void tank_powerup_fxn(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    body_set_time(body1, 0); // or maybe should set to reload amount?

    if (((tank_powerup_aux_t *)aux)->type == MACHINE_GUN) {
        // machine gun power up
        tank_set_new_reload_time(((tank_powerup_aux_t *)aux)->tank, MACHINE_GUN_RELOAD_TIME);
        tank_set_new_range(((tank_powerup_aux_t *)aux)->tank, MACHINE_GUN_RANGE);
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) machine_gun_shoot);
    } else if (((tank_powerup_aux_t *)aux)->type == FRAG_BOMB) {
        //frag bomb power up
        tank_set_new_reload_time(((tank_powerup_aux_t *)aux)->tank, FRAG_BOMB_RELOAD_TIME);
        tank_set_new_range(((tank_powerup_aux_t *)aux)->tank, FRAG_BOMB_RANGE);
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) frag_bomb_shoot);
    } else if (((tank_powerup_aux_t *)aux)->type == LAND_MINE) {
        //land mine power up
        tank_set_new_reload_time(((tank_powerup_aux_t *)aux)->tank, LAND_MINE_RELOAD_TIME);
        tank_set_new_range(((tank_powerup_aux_t *)aux)->tank, LAND_MINE_TIME_LIMIT);
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) land_mine_shoot);
    } else if (((tank_powerup_aux_t *)aux)->type == FORCE_FIELD) {
        // force field power up
        tank_set_new_range(((tank_powerup_aux_t *)aux)->tank, FORCE_FIELD_TIME_LIMIT);
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) force_field_shoot);
    } else {
        // remote controlled missile power up
        tank_set_new_reload_time(((tank_powerup_aux_t *)aux)->tank, MISSILE_RELOAD_TIME);
        tank_set_new_range(((tank_powerup_aux_t *)aux)->tank, MISSILE_RANGE);
        tank_set_shooting_handler(((tank_powerup_aux_t *)aux)->tank, (shooting_handler_t) remote_missile_shoot);
    }

    body_remove(body2);
}

void create_tank_powerup_collision(scene_t *scene, tank_t *tank, body_t *powerup, powerups_t type) {
    tank_powerup_aux_t *tank_pow_aux = tank_powerup_aux_init(tank, type);
    body_t *body1 = tank_get_body(tank);
    create_collision(scene, body1, powerup, (collision_handler_t) tank_powerup_fxn,
                    tank_pow_aux, (free_func_t) free);
}
