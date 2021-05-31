#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "color.h"
#include "list.h"
#include "scene.h"
#include "vector.h"
#include "body.h"

// Values passed to a key handler when the given arrow key is pressed
typedef enum {
    LEFT_ARROW = 1,
    UP_ARROW = 2,
    RIGHT_ARROW = 3,
    DOWN_ARROW = 4,
} arrow_key_t;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum {
    KEY_PRESSED,
    KEY_RELEASED
} key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 * @param body a void * that could be a body
 * @param scene a scene_t representing the scene
 * @param play a bool representing if the game is playing or not
 * @param multi a pointer to a boolean representing if the game is multiplayer
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time,
                              void *body, scene_t *scene, bool *play, bool * multi);

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param scene a scene_t representing the scene
 * @param point a vector_t representing the coordinates of the mouse
 * @param play a bool representing if the game is playing or not
 * @param scenes an array of scene_t's representing the scenes in the game
 * @param level a pointer to an int representing the level of the game
 * @param multi a pointer to a boolean representing if the game is multiplayer
 * @param choosing_level a pointer to a boolean indicating if the level is being chosen
 */
typedef void (*mouse_handler_t)(scene_t *scene, vector_t point, bool *play,
                                scene_t **scenes, int *level, bool * multi,
                                bool *choosing_level);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle keypresses.
 *
 * @param scene a scene_t representing the scene
 * @param body a void * that could be a body
 * @param play a boolean saying if we are playing the game
 * @param scenes an array of scene_t's representing the scenes in the game
 * @param level a pointer to an integer representing the level of the game
 * @param multi a pointer to a boolean indicating multiplayer
 * @param choosing_level a pointer to a boolean indicating if the level is being chosen
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(scene_t *scene, void *body, bool *play, scene_t **scenes, int *level,
                 bool *multi, bool *choosing_level);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param points the list of vertices of the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(list_t *points, rgb_color_t color);

/**
 * Writes text to the screen
 * @param x an integer representing the x-coordinate of the upper-left corner of the text box
 * @param y an integer representing the y-coordinate of the upper-left corner of the text box
 * @param width an integer representing the width of the text box
 * @param height an integer representing the height of the text box
 * @param font the font
 * @param color the color of the text
 * @param text the text that will be written on the screen
 */
void sdl_write(double x, double y, int width, int height, TTF_Font *font, SDL_Color color,
               char *text);

/**
 * Puts an image on the screen
 * @param image the SDL_Surface of the image
 * @param x an integer representing the x-coordinate of the upper-left corner of the image
 * @param y an integer representing the y-coordinate of the upper-left corner of the image
 * @param width an integer representing the width of the image
 * @param height an integer representing the height of the image
 */
void sdl_image(SDL_Surface *image, int x, int y, int width, int height);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 */
void sdl_render_scene(scene_t *scene);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 * 
 * @param handler the function to call with each mouse press
 */
void sdl_on_mouse(mouse_handler_t handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

#endif // #ifndef __SDL_WRAPPER_H__

