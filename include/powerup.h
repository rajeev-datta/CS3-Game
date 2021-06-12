#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "scene.h"
#include "tank.h"

// An aux that helps in powerups
typedef struct tank_powerup_aux tank_powerup_aux_t;

// The enum struct that specifies the info of the powerups
typedef enum powerups{
    MACHINE_GUN,
    FRAG_BOMB,
    LAND_MINE,
    FORCE_FIELD,
    REMOTE_MISSILE,
    INC_LIVES,
} powerups_t;

/**
 * Method that creates a tank_powerup_aux_t with the given
 * tank and type
 *
 * @param tank tank to be placed in tank_powerup_aux object
 * @param type powerups_t to be placed in tank_powerup_aux object
 */
tank_powerup_aux_t *tank_powerup_aux_init(tank_t *tank, powerups_t type);

/**
 * Method that handles regular tank shooting.
 * Method will add bullets as bodies in the scene.
 * It will also add the relevant forces between the bullet
 * and other bodies in the scene.
 *
 * @param scene the scene containing the bodies
 * @param body the body shooting
 */
void default_gun_shoot(scene_t *scene, body_t *body);

/**
 * Method that handles machine gun shooting
 * Method will add machine gun bullets to the scene and add the
 * relevant forces between bodies.
 *
 * @param scene the scene containing the bodies
 * @param body the body shooting
 */
void machine_gun_shoot(scene_t *scene, body_t *body);

/**
 * Method that handles frag bomb shooting
 * Method will add a frag bomb to the scene.
 * It will use scene_body_detonate to explode.
 *
 * @param scene the scene containing the bodies
 * @param body the body shooting
 */
void frag_bomb_shoot(scene_t *scene, body_t *body);

/**
 * Method that handles land mine placing.
 * Method will place a land mine at a point
 * in front of the tank's barrel.
 *
 * @param scene the scene containing the bodies
 * @param body the body shooting
 */
void land_mine_shoot(scene_t *scene, body_t *body);

/**
 * A place holder method that shows tank has force field.
 *
 * @param scene the scene containing the bodies
 * @param body the body shooting
 */
void force_field_shoot(scene_t *scene, body_t *body);

/**
 * Method that creates a remote missile in the scene and
 * adds the relevant forces between bodies.
 *
 * @param scene the scene containing the bodies
 * @param body the body shooting
 */
void remote_missile_shoot(scene_t *scene, body_t *body);

/**
 * Method that handles the tank and powerup collision
 *
 * @param scene the scene containing the bodies
 * @param tank the tank being collided with
 * @param powerup body of powerup
 * @param type the type of powerup being collided with
 */
void create_tank_powerup_collision(scene_t *scene, tank_t *tank, body_t *powerup, powerups_t type);

/**
 * Method that handles creating powerups in the scene
 * and adding the relevant forces between the powerup and other bodies
 *
 * @param scene the scene containing the bodies
 * @param type integer associated with the power up to be made
 * @param tank player 1's tank
 * @param tank_2 player 2's tank
 */
void make_tank_power_up(scene_t *scene, int type, tank_t *tank, tank_t *tank_2);

/**
 * Method that handles removing objects after their designated
 * time in the scene. Includes checking the bullets, powerup projectiles,
 * and tank's powerups
 *
 * @param scene the scene containing the bodies
 * @param tank tank currently being updated
 * @param dt change in time
 */
void update_and_check_projectiles_and_tanks(scene_t *scene, tank_t *tank, double dt);

/**
 * Method that handles creating a force field at the tank's
 * current position and adding the necessary forces between
 * bodies in the scene and the newly created force field
 *
 * @param scene the scene containing the bodies
 * @param tank tank for force field to be created around
 * @return a pointer to the body of newly created force field
 */
body_t *create_new_force_field(scene_t *scene, tank_t *tank);

/**
 * Method that handles moving the force field to tank's current
 * position and getting rid of the force field when its time is up
 *
 * @param scene the scene containing the bodies
 * @param tank tank currently being updated
 * @param dt change in time
 */
void handle_force_field(scene_t *scene, tank_t *tank, double dt);

#endif // #ifndef __POWERUP_H__