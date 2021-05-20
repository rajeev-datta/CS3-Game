#include "animate.h"
#include <math.h>
#include "polygon.h"
#include <stdlib.h>
#include "star.h"
#include "list.h"
#include <assert.h>
#include "collision.h"
#include <float.h>

const double ELLIPSE_SCALE = 1.0/3;
const char INVADER_INFORMATION = 'i';
const char SPACESHIP_INFORMATION = 's';
const char PADDLE_INFORMATION = 'p';
const int RECT_PTS = 4;

list_t *animate_ellipse(vector_t *center, double major, double minor, int points) {
    list_t *ellipse = animate_circle(*center, major, points);
    double init_y = ((vector_t *)list_get(ellipse, 0))->y;
    for(int i = 0; i < points; i++) {
        vector_t *point = list_get(ellipse, i);
        point->y = (point->y - init_y) * ELLIPSE_SCALE + init_y;
    }
    return ellipse;
}

list_t *animate_circle(vector_t center, double radius, int points) {
    list_t *circle = list_init(points, free);
    double angle = 0;
    for (int j = 0; j < points; j++) {
        vector_t *point = malloc(sizeof(vector_t));
        point->x = center.x + radius * cos(angle);
        point->y = center.y + radius * sin(angle);
        list_add(circle, point);
        angle += M_PI * 2 / points;
    }
    return circle;
}

list_t *animate_pacman(vector_t *center, double radius, int points) {
    list_t *circle = animate_circle(*center, radius, points);
    list_t *pacman = list_init(points - points/4 + 1, free);
    list_add(pacman, center);
    for (int i = points/8; i <= points - points/8; i++) {
        vector_t *temp = malloc(sizeof(vector_t));
        *temp = *(vector_t *)list_get(circle, i);
        list_add(pacman, temp);
    }
    list_free(circle);
    return pacman;
}

list_t *animate_invader(vector_t *center, double radius, int points) {
    list_t *circle = animate_circle(*center, radius, points);
    list_t *invader = list_init(points - points/3 + 1, free);
    list_add(invader, center);
    for (int i = points/12; i <= 1.4*points/3; i++) {
        vector_t *temp = malloc(sizeof(vector_t));
        *temp = *(vector_t *)list_get(circle, i);
        list_add(invader, temp);
    }
    list_free(circle);
    return invader;
}

list_t *animate_rectangle(vector_t center, double length, double height) {
    list_t *rectangle = list_init(4, free);
    vector_t *vertex1 = malloc(sizeof(vector_t));
    vector_t *vertex2 = malloc(sizeof(vector_t));
    vector_t *vertex3 = malloc(sizeof(vector_t));
    vector_t *vertex4 = malloc(sizeof(vector_t));
    *vertex1 = (vector_t) {center.x - length/2.0, center.y - height/2.0};
    *vertex2 = (vector_t) {center.x - length/2.0, center.y + height/2.0};
    *vertex3 = (vector_t) {center.x + length/2.0, center.y + height/2.0};
    *vertex4 = (vector_t) {center.x + length/2.0, center.y - height/2.0};
    list_add(rectangle, vertex1);
    list_add(rectangle, vertex2);
    list_add(rectangle, vertex3);
    list_add(rectangle, vertex4);
    return rectangle;
}

list_t *animate_level_1(vector_t top_right, double wall_length, double wall_height) {
    list_t *walls = list_init((size_t) 5, free);
    list_t *center_wall = animate_rectangle((vector_t) {top_right.x/2, top_right.y/2}, wall_length, wall_height);
    list_t *left_top_wall = animate_rectangle((vector_t) {top_right.x/4, top_right.y - (wall_length/2)}, wall_length, wall_height);
    list_t *left_bottom_wall = animate_rectangle((vector_t) {top_right.x/4, wall_length/2}, wall_length, wall_height);
    list_t *right_top_wall = animate_rectangle((vector_t) {(top_right.x*3)/4, top_right.y - (wall_length/2)}, wall_length, wall_height);
    list_t *right_bottom_wall = animate_rectangle((vector_t) {(top_right.x*3)/4, wall_length/2}, wall_length, wall_height);
    list_add(walls, center_wall);
    list_add(walls, left_top_wall);
    list_add(walls, left_bottom_wall);
    list_add(walls, right_top_wall);
    list_add(walls, right_bottom_wall);
    return walls;
}

list_t *make_star(vector_t center, double r1, double r2, int points) {
    list_t *star = list_init(points, free);
    double angle = M_PI / points;

    for (int i = 1; i <= points; i++) {
        vector_t *v = malloc(sizeof(*v));
        if (i % 2) {
            *v = (vector_t) {center.x + r2*cos(angle), center.y + r2*sin(angle)};
        }
        else {
            *v = (vector_t) {center.x + r1*cos(angle), center.y + r1*sin(angle)};
        }
        list_add(star, v);
        angle += M_PI / points * 2;
    }

    return star;
}

void hit_boundary_check(star_t *star, vector_t min, vector_t max, double dt) {
    list_t *points = star_get_vertices(star);
    for (size_t i = 0; i < list_size(points); i++) {
        vector_t *temp = list_get(star_get_vertices(star), i);

        //cases for boundaries and velocities (each case)
        if ((temp->x > max.x && star_get_velocity(star)->x > 0) ||
            (temp->x < min.x && star_get_velocity(star)->x < 0)) {
            star_get_velocity(star)->x *= -1;
            break;
        }
    }
    for (size_t i = 0; i < list_size(points); i++) {
        vector_t *temp = list_get(star_get_vertices(star), i);

        //cases for boundaries and velocities (each case)
        if ((temp->y > max.y && star_get_velocity(star)->y > 0) ||
            (temp->y < min.y && star_get_velocity(star)->y < 0)) {
            star_get_velocity(star)->y *= -1;
            break;
        }
    }
}

void bounce(star_t *star, double g, vector_t min, vector_t max, double dt, double radius,
            double elasticity) {
    star_get_velocity(star)->y -= g * dt;
    list_t *points = star_get_vertices(star);
    for (size_t i = 0; i < list_size(points); i++) {
        vector_t *temp = list_get(points, i);
        if (temp->y < min.y && star_get_velocity(star)->y < 0) {
            star_get_velocity(star)->y *= (-1) * elasticity;
            break;
        }
    }
}

void remove_pellets(body_t *pacman, double pac_rad, double pel_rad, scene_t *scene) {
    int num_removed = 0;
    for (size_t i = 1; i < scene_bodies(scene); i++) {
         if (find_collision_old(body_get_shape(pacman), body_get_shape(scene_get_body(scene, i)))) {
             scene_remove_body(scene, i - num_removed);
             num_removed++;
         }
    }
}

void continuous_boundary(body_t *pacman, vector_t top_right_coord) {
    list_t *vectors = body_get_real_shape(pacman);
    bool crossed_right = true;
    bool crossed_left = true;
    bool crossed_up = true;
    bool crossed_down = true;
    for (size_t i = 0; i < list_size(vectors); i++) {
        vector_t *point = list_get(vectors, i);
        if (point->x < top_right_coord.x) {
            crossed_right = false;
        }
        if (point->x > 0) {
            crossed_left = false;
        }
        if (point->y < top_right_coord.y) {
            crossed_up = false;
        }
        if (point->y > 0) {
            crossed_down = false;
        }
    }
    vector_t new_centroid = body_get_centroid(pacman);
    if (crossed_right) {
        new_centroid.x -= top_right_coord.x;
        body_set_centroid(pacman, new_centroid);
    } else if (crossed_left) {
        new_centroid.x += top_right_coord.x;
        body_set_centroid(pacman, new_centroid);
    }

    if (crossed_up) {
        new_centroid.y -= top_right_coord.y;
        body_set_centroid(pacman, new_centroid);
    } else if (crossed_down) {
        new_centroid.y += top_right_coord.y;
        body_set_centroid(pacman, new_centroid);
    }
}

void wrapping_boundary(scene_t *scene, vector_t top_right, 
                       double spacing, double radius) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *curr_body = scene_get_body(scene, i);
        if (*(char *) body_get_info(curr_body) == INVADER_INFORMATION) {
            if (!body_is_removed(curr_body)) {
                list_t *vectors = body_get_real_shape(curr_body);
                bool hit_right = false;
                bool hit_left = false;
                for (size_t j = 0; j < list_size(vectors); j++) {
                    vector_t *point = list_get(vectors, j);
                    if (point->x > top_right.x - spacing && body_get_velocity(curr_body).x > 0) {
                        hit_right = true;
                    }
                    if (point->x < spacing && body_get_velocity(curr_body).x < 0) {
                        hit_left = true;
                    }
                }
                vector_t new_centroid = body_get_centroid(curr_body);
                if (hit_right || hit_left) {
                    body_set_velocity(curr_body, 
                                    vec_negate(body_get_velocity(curr_body)));
                    new_centroid.y -= 8 * radius + spacing;
                    body_set_centroid(curr_body, new_centroid);
                }
            }
        }
    }
}

void stop_boundary(scene_t *scene, vector_t top_right, vector_t bottom_left, double radius) {
    if (scene != NULL && (*(char *) body_get_info(scene_get_body(scene, 0)) == SPACESHIP_INFORMATION
        || *(char *) body_get_info(scene_get_body(scene, 0)) == PADDLE_INFORMATION)) {
        vector_t space_ship_centroid = body_get_centroid(scene_get_body(scene, 0));
        if (space_ship_centroid.x + radius >= top_right.x) {
            body_set_centroid(scene_get_body(scene, 0),
            (vector_t) {top_right.x - radius, space_ship_centroid.y});
        }
        else if (space_ship_centroid.x - radius <= bottom_left.x) {
            body_set_centroid(scene_get_body(scene, 0),
            (vector_t) {radius, space_ship_centroid.y});
        }
    }
}
