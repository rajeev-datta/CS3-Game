#ifndef __ANIMATE_H__
#define __ANIMATE_H__

#include "vector.h"
#include "list.h"
#include "sdl_wrapper.h"
#include "body.h"
#include "tank.h"

/**
 *  @brief Animates a tank
 *
 *  @param coord a vector_t representing the position of the tank
 * 
 *  @return a list_t representing the points of the tank
 */
list_t *animate_tank(vector_t *coord);

/**
 *  @brief Makes a ellipse
 *
 *  @param center a vector_t representing the center of the ellipse
 *  @param major a double representing the major axis
 *  @param minor a double representing the minor axis
 *  @param points an int representing the number of points making up the ellipse
 * 
 *  @return a list_t representing the points of the ellipse
 */
list_t *animate_ellipse(vector_t *center, double major, double minor, int points);

/**
 *  @brief Makes a circle
 *
 *  @param center a vector_t representing the center of the circle
 *  @param radius a double representing the radius
 *  @param points an int representing the number of points making up the circle
 * 
 *  @return a list_t representing the points of the circle
 */
list_t *animate_circle(vector_t center, double radius, int points);

list_t *animate_ring(vector_t center, double inner_radius, double outer_radius,
                     int points);

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
    TOP_LEFT_CORNER,
    TOP_RIGHT_CORNER,
    BOTTOM_RIGHT_CORNER
} rect_corners_t;

int get_num_rect_pts();

//checks boundaries for enemy tanks, y velocity only
void body_hit_boundary_check(body_t *body, vector_t min, vector_t max, double dt);

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

/**
 *  @brief Creates a boundary for the invaders that drops them down vertically if
 *  a side boundary is hit
 *
 *  @param scene a scene_t representing the scene
 *  @param top_right a vector_t representing the top right coordinate of the screen
 *  @param spacing a double representing the spacing between two invaders
 *  @param radius a double representing the radius of an invader
 */
void wrapping_boundary(scene_t *scene, vector_t top_right, 
                       double spacing, double radius);

/**
 *  @brief Creates a boundary for the spaceship that prevents it from going past
 *  a boundary
 *
 *  @param scene a scene_t representing the scene
 *  @param top_right a vector_t representing the top right coordinate of the screen
 *  @param bottom_left a vector_t representing the bottom left coordinate of the screen
 *  @param radius a double representing the radius of an invader
 */
void stop_boundary(scene_t *scene, vector_t top_right, vector_t bottom_left,
                   double radius);

/**
 *  @brief Creates a boundary for the tank that prevents it from going through a
 *  wall
 *
 *  @param scene a scene_t representing the scene
 *  @param tank_body a body_t representing the body of the input tank
 */
void wall_boundary(scene_t *scene, body_t *tank_body);

#endif // #ifndef __ANIMATE_H__
