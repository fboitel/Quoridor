#include "tests.h"
#include "player.h"
#include "client.h"
#include "move.h"
#include "board.h"
#include "opt.h"
#include "math.h"
#include <stdio.h>
#include <assert.h>

extern struct player_t player;

void test_initialization(void) {
    printf("%s", __func__);

    size_t n = 3;
    struct graph_t* board = graph_init(n, SQUARE);
    size_t edges = n * n * (n - 1) * (n - 1) * 2;
    initialize(BLACK, board, ceil(edges / 15));

    assert(player.id == BLACK);
    assert(player.num_walls == ceil(edges / 15));
    // TODO Check player.graph

    finalize();
    // Free the board
    printf("\tOK\n");
}

void test_player_main(void) {
    test_initialization();
}
