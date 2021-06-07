#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

const double FACTOR = 2;

typedef struct list {
    size_t size;
    size_t capacity;
    void **data;
    free_func_t free_fxn;
} list_t;


list_t *list_init(size_t initial_size, free_func_t free_method) {
    list_t *list = malloc(sizeof(list_t));
    assert(list != NULL);
    list->size = 0;
    list->capacity = initial_size;
    void **temp_data = malloc(initial_size * sizeof(void *));
    assert(temp_data != NULL);
    list->data = temp_data;
    list->free_fxn = free_method;
    return list;
}

void list_free(list_t *list) {
    if (list->free_fxn != NULL) {
        for (size_t i = 0; i < list->size; i++) {
            list->free_fxn(list->data[i]);
        }
    }
    free(list->data);
    free(list);
}

size_t list_size(list_t *list) {
    return list->size;
}

size_t list_capacity(list_t *list) {
    return list->capacity;
}

void *list_get(list_t *list, size_t index) {
    assert(index < list->size);
    return list->data[index];
}

void *list_remove(list_t *list, size_t index) {
    assert(list->size != 0);
    void *removed = list->data[index];
    list->data[index] = NULL;
    for (size_t i=index+1; i < list->size; i++) {
        list_set(list, list_get(list, i), i-1);
    }
    list->size--;
    return removed;
}

void list_add(list_t *list, void *value) {
    list->size++;
    list_set(list, value, list->size - 1);
}

void resize(list_t *list) {
    while (list->size >= list->capacity) {
        list->capacity += 1;
        list->capacity *= FACTOR;
        list->data = realloc(list->data, list->capacity * sizeof(void *));
    }
}

void list_set(list_t *list, void *value, size_t idx) {
    resize(list);
    assert(value != NULL && list->size > idx);
    list->data[idx] = value;
}

void list_clear(list_t *list) {
    while (list_size(list) != 0.0) {
        list_remove(list, 0);
    }
}
