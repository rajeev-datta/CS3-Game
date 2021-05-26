#include "body.h"
#include "scene.h"

typedef struct tank tank_t;

typedef void (*shooting_handler_t)
    (scene_t *scene, tank_t *tank);

tank_t *tank_init(list_t *points, void* info, shooting_handler_t weapon);

body_t *tank_get_body(tank_t *tank);

void tank_set_shooting_handler(tank_t *tank, shooting_handler_t new_weapon);

void tank_set_new_reload_time(tank_t *tank, double new_reload_time);
