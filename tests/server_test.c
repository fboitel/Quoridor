#include "tests.h"
#include "player.h"
#include "move.h"
#include "board.h"
#include "opt.h"
#include "math.h"
#include "ia.h"
#include <stdio.h>
#include <string.h>

bool is_valid_displacement(struct graph_t* board, size_t destination, enum color_t player);


struct game_state_t game_s;
extern char *name;

size_t board_size_s;
size_t edges_s;
struct graph_t *board_s;

size_t position_player_1 = -1;
size_t position_player_2 = -1;
enum color_t active_player_s = -1;

static void setup(void){
    board_size_s = 5;
    board_s = graph_init(board_size_s, SQUARE);
    edges_s = ceil(board_size_s * board_size_s * (board_size_s - 1) * (board_size_s - 1) * 2)/15;
    initialize(BLACK, board_s, edges_s);
    active_player_s = BLACK;
}
static void teardown(void){
    finalize();
    }

void test_is_valid_displacement(){
    printf("%s", __func__);
    position_player_1 = 0;
    position_player_2 = 24;
    //if (!is_valid_displacement(board, 1, BLACK))
        FAIL("A player should move himself in a neighbour vertex");
}


void test_server_main(void){
    TEST(test_is_valid_displacement);

    SUMMARY();
}