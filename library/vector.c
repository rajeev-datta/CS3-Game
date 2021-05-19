#include <stdio.h>
#include <math.h>
#include "vector.h"

const vector_t VEC_ZERO = {.x = 0, .y = 0};

vector_t vec_add(vector_t v1, vector_t v2) {
    vector_t sum;

    sum.x = v1.x + v2.x;
    sum.y = v1.y + v2.y;

    return sum;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
   return vec_add(v1, vec_negate(v2));
}

vector_t vec_negate(vector_t v) {
    return vec_multiply(-1, v);
}

vector_t vec_multiply(double scalar, vector_t v) {
    vector_t product;

    product.x = v.x * scalar;
    product.y = v.y * scalar;

    return product;
}

double vec_dot(vector_t v1, vector_t v2) {
    double dot_prod = (v1.x * v2.x) + (v1.y * v2.y);

    return dot_prod;
}

double vec_cross(vector_t v1, vector_t v2) {
    double cross_prod = (v1.x * v2.y) - (v2.x * v1.y);

    return cross_prod;
}

vector_t vec_rotate(vector_t v, double angle) {
    vector_t rotate;

    rotate.x = (v.x * cos(angle)) - (v.y * sin(angle));
    rotate.y = (v.x * sin(angle)) + (v.y * cos(angle));

    return rotate;
}

double vec_magnitude(vector_t v) {
    return sqrt(v.x*v.x + v.y*v.y);
}
