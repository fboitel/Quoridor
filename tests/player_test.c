#include "tests.h"
#include "player.h"
#include "client.h"
#include "move.h"
#include "board.h"
#include "opt.h"
#include "math.h"
#include <stdio.h>
#include <string.h>

extern struct player_t player;
extern char *name;

size_t n = 3;
size_t edges = 0;
struct graph_t *board = NULL;

static void setup(void) {
	n = 3;
	board = graph_init(n, SQUARE);
	edges = n * n * (n - 1) * (n - 1) * 2;
	initialize(BLACK, board, ceil(edges / 15.0));
}

static void teardown(void) {
	finalize();
}

void test_initialization(void) {
	printf("%s", __func__);

	if (player.id != BLACK) {
		FAIL("Id initialization failed");
	}

	if (player.num_walls != ceil(edges / 15.0)) {
		FAIL("Wrong number of walls initialized");
	}

	// TODO Check player.graph
}

void test_get_player_name(void) {
	printf("%s", __func__);

	if (strcmp(get_player_name(), name) != 0) {
		FAIL("Can not get player name");
	}
}

void test_play_random(void) {
	printf("%s", __func__);
}

void test_player_main(void) {
	TEST(test_initialization);
	TEST(test_get_player_name);
	// TEST(test_play_random);

	SUMMARY();
}
