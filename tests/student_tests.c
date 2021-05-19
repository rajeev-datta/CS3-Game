#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "test_util.h"
#include "forces.h"
#include "scene.h"
#include "color.h"

list_t *make_shape() {
    list_t *shape = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){(-1) * rand() % 100, (-1) * rand() % 100};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){rand() % 100, (-1) * rand() % 100};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){rand() % 100, rand() % 100};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){(-1) * rand() % 100, rand() % 100};
    list_add(shape, v);
    return shape;
}

// Test to make sure that force is not applied when the bodies are very close.
void test_create_newtonian_gravity() {
    const double DT = 1e-6;
    const int STEPS = 1000000;
    const double G = 1e3; 
    const double MASS = 10;
    const int UPPER_BOUND = 1e6;
    scene_t *scene = scene_init();
    body_t *body1 = body_init(make_shape(), MASS, (rgb_color_t){0, 0, 0});
    body_set_centroid(body1, (vector_t){-200, 0});
    body_set_velocity(body1, (vector_t){10, 0});
    scene_add_body(scene, body1);
    body_t *body2 = body_init(make_shape(), MASS, (rgb_color_t){0, 0, 0});
    body_set_centroid(body2, (vector_t){200, 0});
    body_set_velocity(body2, (vector_t){-10, 0});
    scene_add_body(scene, body2);
    create_newtonian_gravity(scene, G, body1, body2);
    for (int i = 0; i < STEPS; i++) {
        assert(vec_magnitude(body_get_velocity(body1)) < UPPER_BOUND);
        assert(vec_magnitude(body_get_velocity(body2)) < UPPER_BOUND);
        scene_tick(scene, DT);
    }
    scene_free(scene);
}

// Test that the object that is being affected by the force of the spring never moves
// farther than the amplitude.
void test_create_spring() {
    const double DT = 1e-6;
    const int STEPS = 1000000;
    const double MASS = 50;
    scene_t *scene = scene_init();
    body_t *body1 = body_init(make_shape(), MASS, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, body1);
    body_t *anchor = body_init(make_shape(), INFINITY, (rgb_color_t){0,0,0});
    scene_add_body(scene, anchor);
    vector_t difference = vec_subtract(body_get_centroid(body1), body_get_centroid(anchor));
    double amplitude = vec_magnitude(difference);
    vector_t direction = vec_multiply(1.0/amplitude, difference);

    for (int i = 0; i < STEPS; i++) {
        vector_t updated_diff = vec_subtract(body_get_centroid(body1), body_get_centroid(anchor));
        double x = vec_magnitude(updated_diff);
        vector_t curr_direction = vec_multiply(1.0/x, updated_diff);
        assert(vec_isclose(direction, curr_direction));
        assert(x < amplitude || within(1e-4, x, amplitude));
        scene_tick(scene, DT);
    }
    scene_free(scene);
}

// Tests that the drag force works
void test_create_drag() {
    const double DT = 1e-6;
    const int STEPS = 1000000;
    const double GAMMA = 50;
    const double MASS = 50;
    scene_t *scene = scene_init();
    body_t *body1 = body_init(make_shape(), MASS, (rgb_color_t){0, 0, 0});
    vector_t init_velocity = {100, 100};
    body_set_velocity(body1, init_velocity);
    scene_add_body(scene, body1);
    create_drag(scene, GAMMA, body1);
    vector_t prev_velocity = init_velocity;
    for (int i = 0; i < STEPS; i++) {
        vector_t predicted_vel = 
            {init_velocity.x * exp((-1) * GAMMA / body_get_mass(body1) * i * DT),
             init_velocity.y * exp((-1) * GAMMA / body_get_mass(body1) * i * DT)};
        assert(within(1e-4, predicted_vel.x, body_get_velocity(body1).x));
        assert(within(1e-4, predicted_vel.y, body_get_velocity(body1).y));
        assert(body_get_velocity(body1).x >= 0 && body_get_velocity(body1).y >= 0);
        prev_velocity = body_get_velocity(body1);
        scene_tick(scene, DT);
    }
    scene_free(scene);
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_create_drag)
    DO_TEST(test_create_spring)
    DO_TEST(test_create_newtonian_gravity)

    puts("student_tests PASS");
}
