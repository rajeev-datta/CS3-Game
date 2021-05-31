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

const rgb_color_t RED = {1, 0, 0};
const rgb_color_t BLACK = {0, 0, 0};
const rgb_color_t GREEN = {0, 1, 0};
const rgb_color_t PURPLE = {0.5, 0, 0.5};
const rgb_color_t MAROON = {128.0/255, 0, 0};
const rgb_color_t BLUE = {0, 0, 1};

#endif // #ifndef __COLOR_H__
