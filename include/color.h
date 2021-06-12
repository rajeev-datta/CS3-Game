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

rgb_color_t color_get_red();

rgb_color_t color_get_black();

rgb_color_t color_get_green();

rgb_color_t color_get_purple();

rgb_color_t color_get_maroon();

rgb_color_t color_get_blue();

rgb_color_t color_get_white();

rgb_color_t color_random();

#endif // #ifndef __COLOR_H__
