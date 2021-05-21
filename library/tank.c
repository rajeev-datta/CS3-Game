#include "tank.h"

const int MASS = 100;
const rgb_color_t BLACK = {0, 0, 0}; 
const rgb_color_t RED = {1, 0, 0};

typedef struct tank {
    body_t *shape;
    free_func_t *weapon;
} tank_t;

tank_t *tank_init(list_t *points, void* info, free_func_t *weapon) {
    tank_t *tank_obj = malloc(sizeof(tank_t));
    body_t *shape;
    
    //enemy tank
    if (info == 'e') { 
        shape = body_init(points, MASS, BLACK);
    }
    //player tank, info == 'p'
    else {
        shape = body_init(points, MASS, RED);
    }
    
    tank_obj->shape = shape;
    tank_obj->weapon = weapon;

    return tank_obj;
}
