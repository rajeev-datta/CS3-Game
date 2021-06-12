#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "color.h"
#include "polygon.h"
#include <math.h>
#include "list.h"
#include "vector.h"
#include "collision.h"
#include <stdio.h>
#include <float.h>

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
    collision_info_t collision;
    collision.collided = false;
    collision_overlap_t collision1 = is_overlap(shape1, shape2);
    collision_overlap_t collision2 = is_overlap(shape2, shape1);
    if (collision1.collided && collision2.collided) {
        if (collision1.overlap < collision2.overlap) {
            collision.collided = true;
            collision.axis = collision1.axis;
        }
        else {
            collision.collided = true;
            collision.axis = collision2.axis;
        }
    }
    return collision;
}

collision_overlap_t is_overlap(list_t *shape1, list_t *shape2) {
    double min_overlap = DBL_MAX;
    vector_t collision_axis = *(vector_t *)list_get(shape1, 0);
    collision_overlap_t collision;
    collision.collided = true;
    for (int i = 0; i < list_size(shape1); i++) {
        vector_t vec_perp = find_perp_edge(*(vector_t *) list_get(shape1, i), 
                                           *(vector_t *) list_get(shape1, 
                                           (i+1)%list_size(shape1)));
        vector_t proj1 = find_projection(shape1, vec_perp);
        vector_t proj2 = find_projection(shape2, vec_perp);
        //x is minimum, y is maximum
        if (proj1.x > proj2.y) {
            collision.collided = false;
        }
        if (proj1.y < proj2.x) {
            collision.collided = false;
        }
        if ((fabs(proj2.x - proj1.y) < min_overlap)
            || (fabs(proj1.x - proj2.y) < min_overlap)) {
            min_overlap = fabs(proj2.x - proj1.y);
            collision_axis = vec_perp;
            collision.overlap = min_overlap;
        } 
        // if (fabs(proj1.x - proj2.y) < min_overlap) {
        //     min_overlap = fabs(proj1.x - proj2.y);
        //     collision_axis = vec_perp;
        //     collision.overlap = min_overlap;
        // }
        if (proj1.x > proj2.x && proj1.y < proj2.y) {
            if (fabs(proj1.y - proj1.x) < min_overlap) {
                min_overlap = fabs(proj1.y - proj1.x);
                collision_axis = vec_perp;
                collision.overlap = min_overlap;
            }
        }
        if (proj2.x > proj1.x && proj2.y < proj1.y) {
            if (fabs(proj2.y - proj2.x) < min_overlap) {
                min_overlap = fabs(proj2.y - proj2.x);
                collision_axis = vec_perp;
                collision.overlap = min_overlap;
            }
        }
    }
    collision.axis = collision_axis;
    return collision;
}

bool is_overlap_old(list_t *shape1, list_t *shape2) {
    for (int i = 0; i < list_size(shape1); i++) {
        vector_t vec_perp = find_perp_edge(*(vector_t *) list_get(shape1, i), 
                                           *(vector_t *) list_get(shape1, 
                                           (i+1)%list_size(shape1)));
        vector_t proj1 = find_projection(shape1, vec_perp);
        vector_t proj2 = find_projection(shape2, vec_perp);
        //x is minimum, y is maximum
        if (proj1.x > proj2.y) {
            return false;
        }
        if (proj1.y < proj2.x) {
            return false;
        }
    }
    return true;
}

bool find_collision_old(list_t *shape1, list_t *shape2) {
    return is_overlap_old(shape1, shape2) && is_overlap_old(shape2, shape1);
}

vector_t find_perp_edge(vector_t vertex1, vector_t vertex2) {
    vector_t edge = vec_subtract(vertex2, vertex1);
    vector_t vec_perp = (vector_t) {-1*edge.y, edge.x};
    vector_t unit_vec_perp = vec_multiply(1/vec_magnitude(vec_perp), vec_perp);
    return unit_vec_perp;
}

vector_t find_projection(list_t *shape, vector_t proj_dest) {
    vector_t unit_proj_dest = vec_multiply(1 / vec_magnitude(proj_dest), 
                                           proj_dest);
    double min = vec_dot(*(vector_t *) list_get(shape, 0), unit_proj_dest);
    double max = vec_dot(*(vector_t *) list_get(shape, 0), unit_proj_dest);
    for (int i = 0; i < list_size(shape); i++) {
        double curr_dot = vec_dot(*(vector_t *) list_get(shape, i), 
                                  unit_proj_dest);
        if (curr_dot < min) {
            min = curr_dot;
        }
        if (curr_dot > max) {
            max = curr_dot;
        }
    }
    vector_t projection = {min, max};
    return projection;
}