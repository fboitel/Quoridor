#include "tests.h"
#include "player.h"
#include "client.h"
#include "move.h"
#include "board.h"
#include "opt.h"
#include "math.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int passed = 0;
int total = 0;
int pass;

#define TEST(f) setup(); f(); teardown(); printf("\t%s\n", pass ? "OK" : "FAIL"); passed += pass; total++;

extern struct player_t player;
extern char *name;

size_t n = 3;
size_t edges = 0;
struct graph_t *board = NULL;


void setup(void) {
    pass = 1;

    n = 3;
    board = graph_init(n, SQUARE);
    edges = n * n * (n - 1) * (n - 1) * 2;
    initialize(BLACK, board, ceil(edges / 15));
}

void teardown(void) {
    graph_free(player.graph);
    finalize();
}

void fail(char* msg) {
    fprintf(stderr, "ERROR: %s\n", msg);
    pass = 0;
}

void test_initialization(void) {
    printf("%s", __func__);

    if (player.id != BLACK) {
        fail("Id initialization failed");
    }
    if (player.num_walls != ceil(edges / 15)) {
        fail("Wrong number of walls initialized");
    };
    // TODO Check player.graph
}

void test_get_player_name(void) {
    printf("%s", __func__);

    if(strcmp(get_player_name(), name)) {
        fail("Can not get player name");
    };
}


void test_play_random(void) {
    printf("%s", __func__);
}

void test_player_main(void) {
    TEST(test_initialization);
    TEST(test_get_player_name);
    // TEST(test_play_random);
    
    printf("%s (%d/%d) --> %s\n", __func__, passed, total, passed == total ? "PASSED" : "FAILED");
}
