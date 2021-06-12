#ifndef __COLOR_H__
#define __COLOR_H__

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
    float r;
    float g;
    float b;
} rgb_color_t;

/**
 * Returns an rgb_color_t for the color red
 */
rgb_color_t color_get_red();

/**
 * Returns an rgb_color_t for the color black
 */
rgb_color_t color_get_black();

/**
 * Returns an rgb_color_t for the color green
 */
rgb_color_t color_get_green();

/**
 * Returns an rgb_color_t for the color purple
 */
rgb_color_t color_get_purple();

/**
 * Returns an rgb_color_t for the color maroon
 */
rgb_color_t color_get_maroon();

/**
 * Returns an rgb_color_t for the color blue
 */
rgb_color_t color_get_blue();

/**
 * Returns an rgb_color_t for the color white
 */
rgb_color_t color_get_white();

/**
 * Returns an rgb_color_t for the color lime
 */
rgb_color_t color_get_lime();

/**
 * Returns an rgb_color_t for the color teal
 */
rgb_color_t color_get_teal();

/**
 * Returns an rgb_color_t for the color pink
 */
rgb_color_t color_get_pink();

/**
 * Returns an rgb_color_t for a randomly generated color
 */
rgb_color_t color_random();

#endif // #ifndef __COLOR_H__
