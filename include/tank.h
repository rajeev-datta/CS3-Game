#ifndef __TANK_H__
#define __TANK_H__

#include "body.h"
#include "scene.h"

// The tank structure
typedef struct tank tank_t;

// A handler that helps in shooting
typedef void (*shooting_handler_t)
    (scene_t *scene, body_t *body);

/**
 * Initializing the player tanks
 * 
 * @param points the list_t representing the points of the tank's body
 * @param info the information of the tank's body
 * 
 * @return the tank_t of the tank
 **/
tank_t *tank_init(list_t *points, void* info);

/**
 * Initializing the enemy tanks
 * 
 * @param points the list_t representing the points of the tank's body
 * @param speed a vector_t representing the speed of the enemy tank
 * @param info the information of the tank's body
 * 
 * @return the tank_t of the tank
 **/
tank_t *enemy_tank_init(list_t *points, vector_t speed, void* info);

/**
 * Returns the body of a tank
 * 
 * @param tank the tank
 * 
 * @return a body_t
 */
body_t *tank_get_body(tank_t *tank);

/**
 * The shooting handler of the tank
 * 
 * @param tank the tank that will shoot
 * @param new_weapon the shooting_handler_t that defines what shooting happens
 */
void tank_set_shooting_handler(tank_t *tank, shooting_handler_t new_weapon);

/**
 * Sets the time it takes a tank to reload
 * 
 * @param tank the tank that will shoot
 * @param new_reload_time the new reload time
 */
void tank_set_new_reload_time(tank_t *tank, double new_reload_time);

/**
 * Sets the range that a tank shoots
 * 
 * @param tank the tank that will shoot
 * @param new_bullet_range the new range
 */
void tank_set_new_range(tank_t *tank, double new_bullet_range);

// Returns the shooting_handler_t of a tank, which defines the type of weapon
shooting_handler_t tank_get_weapon(tank_t *tank);

// Returns the default shooting weapon of a tank
shooting_handler_t tank_get_default_weapon(tank_t *tank);

// Returns the default shooting range of a tank
double tank_get_default_range(tank_t *tank);

// The method that makes the tank shoot
void tank_shoot(scene_t *scene, tank_t *tank);

// Returns the current reload time of the tank
double tank_get_curr_reload(tank_t *tank);

// Returns the current shooting range of the tank
double tank_get_curr_range(tank_t *tank);

// Increases the powerup time of a tank by the specified amount
void tank_increase_powerup_time(tank_t *tank, double time_increment);

// Returns the total powerup time of a tank
double tank_get_total_powerup_time(tank_t *tank);

// Returns the current powerup time of a tank
double tank_get_curr_powerup_time(tank_t *tank);

// Sets the powerup time of a tank
void tank_set_powerup_time(tank_t *tank, double time);

// Increases the body time by the specified amount
void tank_increase_body_time(tank_t *tank, double time_increment);

// Sets the body time to the specified amount
void tank_set_body_time(tank_t *tank, double time);

// Sets the timer of the tank to the specified amount
void tank_set_timer(tank_t *tank, double time);

// Increases the timer of the tank by the specified amount
void tank_increase_timer(tank_t *tank, double dt);

// Gets the timer of the tank
double tank_get_timer(tank_t *tank);

// The method that lets the enemy tank shoot at a player
void enemy_tank_shoot(scene_t *scene, int *level, vector_t player);

// The method that helps in the enemy tank shooting
void enemy_tank_helper(scene_t *scene, body_t *enemy, double max_y, double min_y,
                       vector_t player);

#endif // #ifndef __TANK_H__