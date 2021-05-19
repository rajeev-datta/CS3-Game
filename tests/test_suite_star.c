#include "star.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "list.h"

void test_star_size0() {
    star_t *star = star_init(0, 0.0, 0.0, 0.0);
    assert(list_size(star_get_vertices(star)) == 0);
    star_free(star);
}

void test_star_size6() {
    star_t *star = star_init(6, 0.0, 0.0, 0.0);
    assert(list_size(star_get_vertices(star)) == 0);
    star_free(star);
}

void test_star_colors() {
    star_t *star = star_init(6, 0.0, 0.5, 0.25);
    //test initialized colors
    assert(star_get_color(star)->r == 0.0);
    assert(star_get_color(star)->g == 0.5);
    assert(star_get_color(star)->b == 0.25);
    
    star_free(star);
}

void test_star_velocity() {
    star_t *star = star_init(6, 0.0, 0.0, 0.0);
    //test initialized velocities
    assert(star_get_velocity(star)->x == 0);
    assert(star_get_velocity(star)->y == 0);

    //set velocities and test
    star_get_velocity(star)->x = 100;
    star_get_velocity(star)->y = 200;
    assert(star_get_velocity(star)->x == 100);
    assert(star_get_velocity(star)->y == 200);

    star_free(star);
}

void test_star_vertices() {
    star_t *star = star_init(2, 0.0, 0.0, 0.0);
    //create list of vertices
    list_t *vertices = list_init(4, free);
    vector_t *vec1 = malloc(sizeof(vector_t));
    vec1->x = 100; 
    vec1->y = 100;
    list_add(vertices, vec1);
    vector_t *vec2 = malloc(sizeof(vector_t));
    vec2->x = 300; 
    vec2->y = 300;
    list_add(vertices, vec2);

    //set star vertices and test
    star_set_vertices(star, vertices);
    assert(vec_equal(*(vector_t *)list_get(star_get_vertices(star), 0), *vec1));
    assert(vec_equal(*(vector_t *)list_get(star_get_vertices(star), 1), *vec2));

    //test size
    assert(list_size(star_get_vertices(star)) == 2);
    star_free(star);
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_star_size0)
    DO_TEST(test_star_size6)
    DO_TEST(test_star_colors)
    DO_TEST(test_star_velocity)
    DO_TEST(test_star_vertices)

    puts("star_test PASS");
}
