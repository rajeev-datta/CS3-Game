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

// Returns the red color
rgb_color_t color_get_red();

// Returns the black color
rgb_color_t color_get_black();

// Returns the green color
rgb_color_t color_get_green();

// Returns the purple color
rgb_color_t color_get_purple();

// Returns the maroon color
rgb_color_t color_get_maroon();

// Returns the blue color
rgb_color_t color_get_blue();

// Returns the white color
rgb_color_t color_get_white();

// Returns the lime color
rgb_color_t color_get_lime();

// Returns the teal color
rgb_color_t color_get_teal();

// Returns the pink color
rgb_color_t color_get_pink();

// Returns a random color
rgb_color_t color_random();

#endif // #ifndef __COLOR_H__
