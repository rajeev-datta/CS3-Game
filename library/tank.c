#include "tank.h"
#include "powerup.h"

const int MASS = 100;
const rgb_color_t BLACK = {0, 0, 0}; 
const rgb_color_t RED = {1, 0, 0};
const double default_reload_time = 2;

typedef struct tank {
    body_t *shape;
    shooting_handler_t default_weapon;
    shooting_handler_t weapon;
    double default_reload;
    double new_reload_time;
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
    tank_obj->default_weapon = (shooting_handler_t) default_gun_shoot;
    tank_obj->default_reload = default_reload_time;
    tank_obj->new_reload_time = 0;
    return tank_obj;
}

body_t *tank_get_body(tank_t *tank) {
    return tank->shape;
}

void tank_set_shooting_handler(tank_t *tank, shooting_handler_t new_weapon) {
    tank->weapon = new_weapon;
}

void tank_set_new_reload_time(tank_t *tank, double new_reload_time) {
    tank->new_reload_time = new_reload_time;
}
