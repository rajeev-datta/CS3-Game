#include "tank.h"

const int MASS = 100;
const rgb_color_t BLACK = {0, 0, 0}; 
const rgb_color_t RED = {1, 0, 0};

typedef struct tank {
    body_t *shape;
    shooting_handler_t weapon;
} tank_t;

tank_t *tank_init(list_t *points, void* info, shooting_handler_t weapon) {
    tank_t *tank_obj = malloc(sizeof(tank_t));
    
    //enemy tank
    if (info == 'e') { 
        body_t *shape = body_init(points, MASS, BLACK);
        tank_obj->shape = shape;
    }
    //player tank, info == 'p'
    else {
        body_t *shape = body_init(points, MASS, RED);
        tank_obj->shape = shape;
    }
    
    tank_obj->weapon = weapon;
    return tank_obj;
}

body_t *tank_get_body(tank_t *tank) {
    return tank->shape;
}

void tank_set_shooting_handler(tank_t *tank, shooting_handler_t new_weapon) {
    tank->weapon = new_weapon;
}
