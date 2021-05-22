#include "body.h"
#include "scene.h"

typedef struct tank tank_t;

typedef void (*shooting_handler_t)
    (scene_t *scene, tank_t *tank);

tank_t *tank_init(body_t *shape, void *info, shooting_handler_t *weapon);
