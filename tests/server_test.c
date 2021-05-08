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

extern size_t board_size;
extern size_t edges;
struct graph_t *my_board;

extern size_t position_player_1;
extern size_t position_player_2;
extern enum color_t active_player_s;

static void setup(void){
    board_size = 10;
    my_board = graph_init(board_size, SQUARE);
    edges = ceil(board_size * board_size * (board_size - 1) * (board_size - 1) * 2)/15;
    initialize(BLACK, my_board, edges);
  //  active_player_s = BLACK;
}
static void teardown(void){
    finalize();
    }

void test_is_valid_displacement(){
    printf("%s", __func__);
    
    for (size_t i = 0; i < my_board->num_vertices - 1; i++){
        position_player_1 = i;
        if (is_valid_displacement(my_board, -1, BLACK) || is_valid_displacement(my_board, 100, BLACK))
            FAIL("A player cannot run away from the board");
        if (is_valid_displacement(my_board, i, BLACK)){
            FAIL("A player cannot move on himself");
        }
    }  

    for (int iter = 10; iter < 20; iter++){ 
        position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7); 
        position_player_2 = 99;
        if (!is_valid_displacement(my_board, position_player_1 - 1, BLACK) || !is_valid_displacement(my_board, position_player_1 + 1, BLACK) || !is_valid_displacement(my_board, position_player_1 - 10, BLACK) || !is_valid_displacement(my_board, position_player_1 + 10, BLACK))
            FAIL("A player should move himself in a neighbour vertex");


        position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);    
        for (size_t i = 0; i < my_board->num_vertices; i++)
            if ((i != position_player_1 + 1 && i != position_player_1 - 1 && i != position_player_1 + 10 && i != position_player_1 - 10) && is_valid_displacement(my_board, i, BLACK)){
                FAIL("A player can't teleport himself");
            }
        
        position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);
        struct edge_t e[2];
        e[0].fr = position_player_1;
        e[0].to = position_player_1 + 10;
        e[1].fr = position_player_1 + 1;
        e[1].to = position_player_1 + 11;
        place_wall(my_board, e);
        if (is_valid_displacement(my_board, position_player_1 + 10, BLACK))
            FAIL("A player can't jump over a wall");

        remove_wall(my_board, e);
        position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);
        position_player_2 = position_player_1 - 1;
        if (is_valid_displacement(my_board, position_player_2, BLACK))
            FAIL("A player cannot move where another player is already");
        
        position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);
        position_player_2 = position_player_1 + 10;
        if (is_valid_displacement(my_board, position_player_2, BLACK))
            FAIL("A player cannot move where another player is already");

        position_player_1 = (2 + rand()%6) + 10*(2 + rand()%6);
        position_player_2 = position_player_1 + 1;
        if (!is_valid_displacement(my_board, position_player_2 + 1, BLACK))
            FAIL("A player should have the possibility to jump over another player");
        
        position_player_1 = (2 + rand()%6) + 10*(2 + rand()%6);
        position_player_2 = position_player_1 - 10;
        if (!is_valid_displacement(my_board, position_player_2 - 10, BLACK))
            FAIL("A player should have the possibility to jump over another player");

        position_player_1 = (2 + rand()%6) + 10*(2 + rand()%6);
        position_player_2 = position_player_1 + 10;
        e[0].fr = position_player_2;
        e[0].to = position_player_2 + 10;
        e[1].fr = position_player_2 + 1;
        e[1].to = position_player_2 + 11;
        place_wall(my_board, e);
        if (!is_valid_displacement(my_board, position_player_1 + 11, BLACK) || !is_valid_displacement(my_board, position_player_1 + 9, BLACK))
            FAIL("A player should have the possibility to jump on the side of a player placed behind him if there is a wall in front of him");
        remove_wall(my_board, e);
        //display_board(my_board, board_size, position_player_1, position_player_2);
        }
}


void test_server_main(void){
    TEST(test_is_valid_displacement);

    SUMMARY();
}