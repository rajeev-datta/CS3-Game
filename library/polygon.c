#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "list.h"
#include "polygon.h"
#include "vector.h"

double polygon_area(list_t *polygon) {
    double sum = 0;
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v1 = list_get(polygon, i);
        vector_t *v2 = list_get(polygon, (i+1) % list_size(polygon));
        sum += vec_cross(*v1, *v2);
    }
    double area = (sum)/2.0;
    return area;
}

vector_t polygon_centroid(list_t *polygon) {
    vector_t centroid;
    double sum_x = 0;
    double sum_y = 0;
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v1 = list_get(polygon, i);
        vector_t *v2 = list_get(polygon, (i+1) % list_size(polygon));
        vector_t add_vector = vec_add(*v1, *v2);
        sum_x += (add_vector.x) * vec_cross(*v1, *v2);
        sum_y += (add_vector.y) * vec_cross(*v1, *v2);
    }
    sum_x /= (6*polygon_area(polygon));
    sum_y /= (6*polygon_area(polygon));
    centroid.x = sum_x;
    centroid.y= sum_y;
    return centroid;
}

void polygon_translate(list_t *polygon, vector_t translation) {
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v1 = list_get(polygon, i);
        v1->x += translation.x;
        v1->y += translation.y;
    }
}

void polygon_rotate_helper(vector_t *polygon_vertex, double angle) {
    double temp_x = (polygon_vertex->x * cos(angle)) - (polygon_vertex->y * sin(angle));
    double temp_y = (polygon_vertex->x * sin(angle)) + (polygon_vertex->y * cos(angle));
    polygon_vertex->x = temp_x;
    polygon_vertex->y = temp_y;
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
    polygon_translate(polygon, vec_negate(point));
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v1 = list_get(polygon, i);
        polygon_rotate_helper(v1, angle);
    }
    polygon_translate(polygon, point);
}
