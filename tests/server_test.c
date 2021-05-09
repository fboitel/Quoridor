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
bool is_valid_wall(struct graph_t* board, struct edge_t e[]);

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



void fill_wall(size_t fr_1, size_t to_1, size_t fr_2, size_t to_2, struct edge_t e[]){
    e[0].fr = fr_1;
    e[0].to = to_1;
    e[1].fr = fr_2;
    e[1].to = to_2;
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

    for (int iter = 0; iter < 10; iter++){ 
        position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7); 
        position_player_2 = 99;
        if (!is_valid_displacement(my_board, position_player_1 - 1, BLACK) || !is_valid_displacement(my_board, position_player_1 + 1, BLACK) || !is_valid_displacement(my_board, position_player_1 - 10, BLACK) || !is_valid_displacement(my_board, position_player_1 + 10, BLACK))
            FAIL("A player should move himself in a neighbour vertex");


        position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);    
        for (size_t i = 0; i < my_board->num_vertices; i++)
            if ((i != position_player_1 + 1 && i != position_player_1 - 1 && i != position_player_1 + 10 && i != position_player_1 - 10) && is_valid_displacement(my_board, i, BLACK)){
                FAIL("A player can't teleport himself");
            }
        
        position_player_1 = 10*(1 + rand()%7);
        if (is_valid_displacement(my_board, position_player_1 - 1, BLACK))
            FAIL("A player can't cross the board");

        position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);
        struct edge_t e[2];
        fill_wall(position_player_1, position_player_1 + 10, position_player_1 + 1, position_player_1 + 11, e);
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
        fill_wall(position_player_2, position_player_2 + 10, position_player_2 + 1, position_player_2 + 11, e);
        place_wall(my_board, e);
        if (!is_valid_displacement(my_board, position_player_1 + 11, BLACK) || !is_valid_displacement(my_board, position_player_1 + 9, BLACK))
            FAIL("A player should have the possibility to jump on the side of a player placed behind him if there is a wall in front of him");
        remove_wall(my_board, e);
        }
}



void test_is_valid_wall(){
    printf("%s", __func__);

    struct edge_t e[2];
    size_t fr_1 = rand()%8 + 10*rand()%8;
    fill_wall(fr_1, fr_1 + 10, fr_1 + 1, fr_1 + 11, e);
    if (!is_valid_wall(my_board, e))
        FAIL("An horizontal wall can be placed anywhere in an empty board");
    
    fill_wall(fr_1, fr_1 + 1, fr_1 + 10, fr_1 + 11, e);
    if (!is_valid_wall(my_board, e))
        FAIL("An vertical wall can be placed anywhere in an empty board");

    fr_1 = 1 + rand()%8 + 10*(1 + rand()%8);
    fill_wall(fr_1, fr_1 - 10, fr_1 - 1, fr_1 - 11, e);
    if (!is_valid_wall(my_board, e))
        FAIL("The order of the vertex given to put a wall shouldn't matter");

    fr_1 = rand()%7 + 10*(rand()%7);
    fill_wall(fr_1, fr_1 + 20, fr_1 + 1, fr_1 + 21, e);
    if (is_valid_wall(my_board, e))
        FAIL("The vertex separated by a wall should be connexe");

    fr_1 = rand()%7 + 10*(rand()%7);
    fill_wall(fr_1, fr_1 + 10, fr_1 + 2, fr_1 + 12, e);
    if (is_valid_wall(my_board, e))
        FAIL("The vertex separated by a wall should be connexe");
    
    fr_1 = rand()%8 + 10*(rand()%8);
    fill_wall(fr_1, fr_1 + 10, fr_1 + 1, fr_1 + 11, e);
    place_wall(my_board, e);
    if (is_valid_wall(my_board, e))
        FAIL("A wall cannot be placed if another one is there");
    remove_wall(my_board, e);
}


void test_server_main(void){
    TEST(test_is_valid_displacement);
    TEST(test_is_valid_wall);
    SUMMARY();
}