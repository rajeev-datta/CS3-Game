#include "color.h"
#include <stdlib.h>

const rgb_color_t RED = {1, 0, 0};
const rgb_color_t BLACK = {0, 0, 0};
const rgb_color_t GREEN = {0, 1, 0};
const rgb_color_t PURPLE = {0.5, 0, 0.5};
const rgb_color_t MAROON = {128.0/255, 0, 0};
const rgb_color_t BLUE = {0, 0, 1};
const rgb_color_t WHITE = {1, 1, 1};
const rgb_color_t LIME = {156/255.0, 212/256.0, 15/255.0};
const rgb_color_t TEAL = {15/255.0, 212/255.0, 166/255.0};
const rgb_color_t PINK = {255/255.0, 15/255.0, 219/255.0};

rgb_color_t color_get_red() {
    return RED;
}

rgb_color_t color_get_black() {
    return BLACK;
}

rgb_color_t color_get_green() {
    return GREEN;
}

rgb_color_t color_get_purple() {
    return PURPLE;
}

rgb_color_t color_get_maroon() {
    return MAROON;
}

rgb_color_t color_get_blue() {
    return BLUE;
}

rgb_color_t color_get_white() {
    return WHITE;
}

rgb_color_t color_get_lime() {
    return LIME;
}

rgb_color_t color_get_teal() {
    return TEAL;
}

rgb_color_t color_get_pink() {
    return PINK;
}

rgb_color_t color_random() {
    float red = (float)rand()/(float)RAND_MAX;
    float green = (float)rand()/(float)RAND_MAX;
    float blue = (float)rand()/(float)RAND_MAX;
    rgb_color_t color = {red, green, blue};
    return color;
}