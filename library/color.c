#include "color.h"

const rgb_color_t RED = {1, 0, 0};
const rgb_color_t BLACK = {0, 0, 0};
const rgb_color_t GREEN = {0, 1, 0};
const rgb_color_t PURPLE = {0.5, 0, 0.5};
const rgb_color_t MAROON = {128.0/255, 0, 0};
const rgb_color_t BLUE = {0, 0, 1};
const rgb_color_t WHITE = {1, 1, 1};

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