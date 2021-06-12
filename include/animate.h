#ifndef __ANIMATE_H__
#define __ANIMATE_H__

#include "vector.h"
#include "star.h"
#include "list.h"
#include "sdl_wrapper.h"
#include "body.h"
#include "tank.h"

///Animates a tank
list_t *animate_tank(vector_t *coord);

// Animates an ellipse
list_t *animate_ellipse(vector_t *center, double major, double minor, int points);

/**
 *  @brief Makes a circle
 *
 *  @param center a vector_t representing the center of the star
 *  @param radius a double representing the radius
 *  @param points an int representing the number of points making up the circle
 * 
 *  @return a vec_list_t representing the points of the circle
 */
list_t *animate_circle(vector_t center, double radius, int points);

list_t *animate_ring(vector_t center, double inner_radius, double outer_radius, int points);

/**
 *  @brief Makes Pacman
 *
 *  @param center a vector_t representing the center of Pacman
 *  @param radius a double representing the radius
 *  @param points an int representing the number of points making up Pacman
 *  if Pacman was a full circle
 * 
 *  @return a vec_list_t representing the points of Pacman
 */
list_t *animate_pacman(vector_t *center, double radius, int points);

//Animates the invader
list_t *animate_invader(vector_t *center, double radius, int points);

//Animates a rectangle, used as a bullet
list_t *animate_rectangle(vector_t center, double length, double height);

const typedef enum rect_corners{
    BOTTOM_LEFT_CORNER,
    TOP_LEFT_CORNER
} rect_corners_t;

/**
 *  @brief Makes a star
 *
 *  @param center a vector_t representing the center of the star
 *  @param r1 a double representing the inner radius
 *  @param r2 a double representing the outer radius
 *  @param points an int representing two times the number of outer points in a star
 * 
 *  @return a vec_list_t representing the points of the star
 */
list_t *make_star(vector_t center, double r1, double r2, int points);

/**
 *  @brief Makes the star bounce around the screen
 *
 *  @param star a star_t representing the points on the star
 *  @param min a vector_t representing the minimum x and y coordinates of the screen
 *  @param max a vector_t representing the maximum x and y coordinates of the screen
 *  @param dt a double representing the change in time since last tick
 */
void hit_boundary_check(star_t *star, vector_t min, vector_t max, double dt);

//checks boundaries for enemy tanks, y velocity only
void body_hit_boundary_check(body_t *body, vector_t min, vector_t max, double dt);

/**
 *  @brief Makes an object fall due to gravity
 *
 *  @param star a star_t representing the points on the star
 *  @param g a double representing the acceleration due to gravity
 *  @param min a vector_t representing the minimum x and y coordinates of the screen
 *  @param max a vector_t representing the maximum x and y coordinates of the screen
 *  @param dt a double representing the change in time since last tick
 *  @param radius a double representing the radius of the star
 *  @param elasticity a double representing the elasticity of a bounce
 */
void bounce(star_t *star, double g, vector_t min, vector_t max, double dt, 
            double radius, double elasticity);

/**
 *  @brief Does an action if specific keys are pressed
 *
 *  @param pacman a list_t representing pacman's body
 *  @param pac_rad double representing pacman's radius
 *  @param pel_rad double representing pellet's radius
 *  @param scene a scene_t
 */
void remove_pellets(body_t *pacman, double pac_rad, double pel_rad, scene_t *scene);

/**
 *  @brief Lets pacman come through the other end
 *
 *  @param pacman a body_t representing pacman's body
 *  @param top_right_coord the top right coordinate of the screen
 */
void continuous_boundary(body_t *pacman, vector_t top_right_coord);

void wrapping_boundary(scene_t *scene, vector_t top_right, 
                       double spacing, double radius);

void stop_boundary(scene_t *scene, vector_t top_right, vector_t bottom_left,
                   double radius);

void wall_boundary(scene_t *scene, body_t *tank_body);

#endif // #ifndef __ANIMATE_H__
