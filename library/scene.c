 #include <assert.h>
#include <stdlib.h>
#include "list.h"
#include "vector.h"
#include "color.h"
#include "body.h"
#include "scene.h"

const size_t INIT_NUM_BODIES = 15;

typedef struct force_data {
    force_creator_t force_fxn;
    free_func_t free_fxn;
    aux_t *aux_data;
    list_t *bodies;
} force_data_t;

typedef struct scene {
    list_t *bodies;
    list_t *force_data_lst;
} scene_t;

void force_data_free(force_data_t *force_data) {
    if (force_data->free_fxn != NULL) {
        force_data->free_fxn(force_data->aux_data);
    }
    free(force_data);
}

force_data_t *force_data_init(force_creator_t force_funct, free_func_t free_funct, aux_t *aux_params) {
    force_data_t *force_data = malloc(sizeof(force_data_t));
    force_data->aux_data = aux_params;
    force_data->force_fxn = force_funct;
    force_data->free_fxn = free_funct;
    force_data->bodies = list_init(1, NULL);
    return force_data;
}

void force_data_add_body(force_data_t *force_data, body_t *body) {
    list_add(force_data->bodies, body);
}

scene_t *scene_init(void) {
    scene_t *scene_obj = malloc(sizeof(scene_t));
    scene_obj->bodies = list_init(INIT_NUM_BODIES, (free_func_t)body_free);
    scene_obj->force_data_lst = list_init(INIT_NUM_BODIES, (free_func_t)force_data_free);
    return scene_obj;
}

void scene_free(scene_t *scene) {
    list_free(scene->bodies);
    list_free(scene->force_data_lst);
    free(scene);
}

size_t scene_bodies(scene_t *scene) {
    return list_size(scene->bodies);
}

body_t *scene_get_body(scene_t *scene, size_t index) {
    assert(index < list_size(scene->bodies));
    return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
    list_add(scene->bodies, body);
}

void scene_remove_body(scene_t *scene, size_t index) {
    body_remove(list_get(scene->bodies, index));
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
    scene_add_bodies_force_creator(scene, forcer, aux, scene->bodies, freer);
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                                    list_t *bodies, free_func_t freer) {
    force_data_t *force_data = force_data_init(forcer, freer, aux);
    list_free(force_data->bodies);
    force_data->bodies = bodies;
    list_add(scene->force_data_lst, force_data);
}

void scene_tick(scene_t *scene, double dt) {
    for (size_t i = 0; i < list_size(scene->force_data_lst); i++) {
        force_data_t *force_data = list_get(scene->force_data_lst, i);
        force_data->force_fxn(force_data->aux_data);
    }

    for (size_t j=0; j < list_size(scene->force_data_lst); j++) {
        force_data_t *curr_force_data = list_get(scene->force_data_lst, j);
        for (size_t k=0; k < list_size(curr_force_data->bodies); k++) {
            body_t *curr_body = list_get(curr_force_data->bodies, k);
            if (body_is_removed(curr_body)) {
                force_data_free(list_remove(scene->force_data_lst, j));
                j--;
                break;
            }
        }
    }

    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *body = list_get(scene->bodies, i);
        if(body_is_removed(body)) {
            body_t *removed = list_remove(scene->bodies, i);
            body_free(removed);
            i--;
        }
        else {
            body_tick(body, dt);
        }
    }
}

void erase_scene(scene_t *scene) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        scene_remove_body(scene, i);
    }
}

