#include "tests.h"
#include "player.h"
#include "move.h"
#include "board.h"
#include "opt.h"
#include "math.h"
#include "ia.h"
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

enum reasons_t { WIN = 0, INVALID_MOVE = 1 };
bool is_valid_displacement(struct graph_t* board, size_t destination, enum color_t player);
bool is_valid_wall(struct graph_t* board, struct edge_t e[]);
void update_board(struct graph_t* board, struct move_t* last_move);
int is_winning(struct graph_t* board, enum color_t active_player, size_t position);
bool move_is_valid(struct move_t* mv, struct graph_t* board, enum color_t player);
void end_game(enum reasons_t reason);


struct game_state_t game_s;
extern char *name;


extern size_t board_size;
extern size_t edges;
struct graph_t *my_board;
struct graph_t *my_smaller_board;

extern size_t position_player_1;
extern size_t position_player_2;
extern enum color_t active_player;
extern bool game_over;

void* P1_lib;
void* P2_lib;
extern char* (*P1_name)(void);
extern char* (*P2_name)(void);





static void setup(void){
    board_size = 10;
    my_board = graph_init(board_size, SQUARE);
    my_smaller_board = graph_init(6, SQUARE);
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
    board_size = 10;
    for (size_t i = 0; i < my_board->num_vertices - 1; i++){
        position_player_1 = i;
        if (is_valid_displacement(my_board, -1, BLACK) || is_valid_displacement(my_board, 100, BLACK))
            FAIL("A player cannot run away from the board");
        if (is_valid_displacement(my_board, i, BLACK)){
            FAIL("A player cannot move on himself");
        }
    }  
    bool error = false;
    for (int iter = 0; iter < 10; iter++)
            if (!error){
            position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7); 
            position_player_2 = 99;
            if (!is_valid_displacement(my_board, position_player_1 - 1, BLACK) || !is_valid_displacement(my_board, position_player_1 + 1, BLACK) || !is_valid_displacement(my_board, position_player_1 - 10, BLACK) || !is_valid_displacement(my_board, position_player_1 + 10, BLACK)){
                FAIL("A player should move himself in a neighbour vertex");
                error = true;
            }

            position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);    
            for (size_t i = 0; i < my_board->num_vertices; i++)
                if ((i != position_player_1 + 1 && i != position_player_1 - 1 && i != position_player_1 + 10 && i != position_player_1 - 10) && is_valid_displacement(my_board, i, BLACK)){
                    FAIL("A player can't teleport himself");
                    error = true;
                }
                
            position_player_1 = 10*(1 + rand()%7);
            if (is_valid_displacement(my_board, position_player_1 - 1, BLACK)){
                FAIL("A player can't cross the board");
                error = true;
            }

            position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);
            struct edge_t e[2];
            fill_wall(position_player_1, position_player_1 + 10, position_player_1 + 1, position_player_1 + 11, e);
            place_wall(my_board, e);
            if (is_valid_displacement(my_board, position_player_1 + 10, BLACK)){
                FAIL("A player can't jump over a wall");
                error = true;
            }
            remove_wall(my_board, e);

            position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);
            position_player_2 = position_player_1 - 1;
            if (is_valid_displacement(my_board, position_player_2, BLACK)){
                FAIL("A player cannot move where another player is already");
                error = true;
            }
                
            position_player_1 = (1 + rand()%7) + 10*(1 + rand()%7);
            position_player_2 = position_player_1 + 10;
            if (is_valid_displacement(my_board, position_player_2, BLACK)){
                FAIL("A player cannot move where another player is already");
                error = true;
            }

            position_player_1 = (2 + rand()%6) + 10*(2 + rand()%6);
            position_player_2 = position_player_1 + 1;
            if (!is_valid_displacement(my_board, position_player_2 + 1, BLACK)){
                FAIL("A player should have the possibility to jump over another player");
                error = true;
            }
                
            position_player_1 = (2 + rand()%6) + 10*(2 + rand()%6);
            position_player_2 = position_player_1 - 10;
            if (!is_valid_displacement(my_board, position_player_2 - 10, BLACK)){
                FAIL("A player should have the possibility to jump over another player");
                error = true;
            }
            
            position_player_1 = (2 + rand()%6) + 10*(2 + rand()%6);
            position_player_2 = position_player_1 + 10;
            fill_wall(position_player_2, position_player_2 + 10, position_player_2 + 1, position_player_2 + 11, e);
            place_wall(my_board, e);
            if (!is_valid_displacement(my_board, position_player_1 + 11, BLACK) || !is_valid_displacement(my_board, position_player_1 + 9, BLACK)){
                FAIL("A player should have the possibility to jump on the side of a player placed behind him if there is a wall in front of him");
                error = true;
            }
            remove_wall(my_board, e);
        }
}



void test_is_valid_wall(){
    printf("%s", __func__);
    board_size = 10;
    bool error = false;
    for (int iter = 0; iter < 10; iter++)
            if(!error){
        struct edge_t e[2];
        size_t fr_1 = rand()%8 + 10*(rand()%8);
        fill_wall(fr_1, fr_1 + 10, fr_1 + 1, fr_1 + 11, e);
        if (!is_valid_wall(my_board, e)) {
            FAIL("An horizontal wall can be placed anywhere in an empty board");
            error = true;
        }

        fill_wall(fr_1, fr_1 + 1, fr_1 + 10, fr_1 + 11, e);
        if (!is_valid_wall(my_board, e)){
            FAIL("An vertical wall can be placed anywhere in an empty board");
            error = true;
        }

        fr_1 = 1 + rand()%8 + 10*(1 + rand()%8);
        fill_wall(fr_1, fr_1 - 10, fr_1 - 1, fr_1 - 11, e);
        if (!is_valid_wall(my_board, e)){
            FAIL("The order of the vertex given to put a wall shouldn't matter");
            error = true;
        }

        fr_1 = rand()%7 + 10*(rand()%7);
        fill_wall(fr_1, fr_1 + 20, fr_1 + 1, fr_1 + 21, e);
        if (is_valid_wall(my_board, e)){
            FAIL("The vertex separated by a wall should be connexe");
            error = true;
        }


        fr_1 = rand()%7 + 10*(rand()%7);
        fill_wall(fr_1, fr_1 + 10, fr_1 + 2, fr_1 + 12, e);
        if (is_valid_wall(my_board, e)){
            FAIL("The vertex separated by a wall should be connexe");
            error = true;
        }
            
        fr_1 = rand()%8 + 10*(rand()%8);
        fill_wall(fr_1, fr_1 + 10, fr_1 + 1, fr_1 + 11, e);
        place_wall(my_board, e);
        if (is_valid_wall(my_board, e)){
            FAIL("A wall cannot be placed if another one is there");
            error = true;
        }
        remove_wall(my_board, e);

        fr_1 = 1 + rand()%7 + 10*(1 + rand()%7);
        struct edge_t et1[2];
        fill_wall(fr_1, fr_1 + 10, fr_1 + 1, fr_1 + 11, et1);
        place_wall(my_board, et1);
        fill_wall(fr_1, fr_1 + 10, fr_1 + 1, fr_1 + 11, e);
        if (is_valid_wall(my_board, e)){
            FAIL("A wall cannot intercept another wall");
            error = true;
        }
        remove_wall(my_board, et1);

        fr_1 = 9 + 10*(rand()%7);
        fill_wall(fr_1, fr_1 + 10, fr_1 + 1, fr_1 + 11, e);
        if (is_valid_wall(my_board, e)){
            FAIL("A wall cannot begin in one side of the baord and finish in the other");
            error = true;
        }
    }
    position_player_1 = 56;
    struct edge_t w1[2] = {{46, 56}, {47, 57}};
    struct edge_t w2[2] = {{57, 58}, {67, 68}};
    struct edge_t w3[2] = {{66, 76}, {67, 77}};
    struct edge_t w4[2] = {{55, 56}, {65, 66}};
    place_wall(my_board, w1);
    place_wall(my_board, w2);
    place_wall(my_board, w3);
    if (is_valid_wall(my_board, w4))
        FAIL("A player can't be locked in by walls");
    else {
        position_player_1 = 6;
        position_player_2 = 56;

        if (is_valid_wall(my_board, w3))
            FAIL("A player can't be locked in by walls");
    }
    remove_wall(my_board, w1);
    remove_wall(my_board, w2);
    remove_wall(my_board, w3);

    struct edge_t bw1[2] = {{30, 40}, {31, 41}};
    struct edge_t bw2[2] = {{32, 42}, {33, 43}};
    struct edge_t bw3[2] = {{34, 44}, {35, 45}};
    struct edge_t bw4[2] = {{36, 46}, {37, 47}};
    struct edge_t bw5[2] = {{38, 48}, {39, 49}};
    place_wall(my_board, bw1);
    place_wall(my_board, bw2);
    place_wall(my_board, bw3);
    place_wall(my_board, bw4);
    position_player_1 = 10;
    position_player_2 = 15;
    if (is_valid_wall(my_board,bw5))
        FAIL("A wall cannot completely block a player from the finish line");
    else {
        position_player_1 = 80;
    position_player_2 = 90;
        if (is_valid_wall(my_board,bw5))
            FAIL("A wall cannot completely block a player from the finish line");
    }
    remove_wall(my_board, bw1);
    remove_wall(my_board, bw2);
    remove_wall(my_board, bw3);
    remove_wall(my_board, bw4);
    
}

void test_update_board(){
    printf("%s", __func__);
    board_size = 10;
    struct move_t move1 = {
        .m = 5, 
        .e = {{-1, -1}, {-1, -1}},
        .t = MOVE,
        .c = BLACK
                            };
    position_player_1 = 4;
    position_player_2 = 3;
    active_player = BLACK;
    update_board(my_board, &move1);
    if (position_player_1 != 5)
        FAIL("Update_board should modify the position of the players if it is needed");

    struct edge_t e[2];
    struct move_t move2 = {
        .m = 5, 
        .e = {{5, 15}, {6, 16}},
        .t = WALL,
        .c = BLACK
                            };
    update_board(my_board, &move2);
    fill_wall(5, 15, 6, 16, e);
    if (is_valid_wall(my_board, e))
        FAIL("Update_board should put a wall if it is needed");
}

void test_is_winning(){
    printf("%s", __func__);
    board_size = 10;
    //player_1 = WHITE, his vertices are at the top
    //player_2 = BLACK, his vertices are at the bottom
   
    for (size_t i = 10; i < 100; i++)
        if (is_winning(my_board, WHITE, i)){
            FAIL("player_1 should be on player_2's vertex to win");
            break;
        }
    for (size_t i = 0; i < 90; i++)
        if (is_winning(my_board, BLACK, i)){
            FAIL("player_2 should be on player_1's vertex to win");
            break;
        }
    for (size_t i = 0; i < 10; i++)
        if (!is_winning(my_board, WHITE, i)){
            FAIL("player_1 should win if he is on opponent's vertex");
            break;
        }
    for (size_t i = 90; i < 100; i++)
        if (!is_winning(my_board, BLACK, i)){
            FAIL("player_2 should win if he is on opponent's vertex");
            break;
        }
    
}
void test_move_is_valid(){
    printf("%s", __func__);
    board_size = 6;
    P1_lib = dlopen("build/crashboy.so", RTLD_NOW);
    P2_lib = dlopen("build/crashboy.so", RTLD_LAZY);
    P1_name = dlsym(P1_lib, "get_player_name");
    P2_name = dlsym(P2_lib, "get_player_name");
    freopen("/dev/null", "r", stderr);
    struct edge_t bw1[2] = {{6, 12}, {7, 13}};
    struct edge_t bw2[2] = {{13, 14}, {19, 20}};
    struct edge_t bw3[2] = {{20, 26}, {21, 27}};
    struct edge_t bw4[2] = {{10, 16}, {11, 17}};
    struct edge_t bw5[2] = {{16, 17}, {22, 23}};
    place_wall(my_smaller_board, bw1);
    place_wall(my_smaller_board, bw2);
    place_wall(my_smaller_board, bw3);
    place_wall(my_smaller_board, bw4);
    place_wall(my_smaller_board, bw5);
    bool error = false;
    for (size_t pos = 0; pos < 10; pos++) {
        position_player_1 = rand()%36;
        position_player_2 = rand()%36;
        while (position_player_2 == position_player_1)
            position_player_2 = rand()%36;
        for (size_t i = 0; i < 50; i++)
            if (!error){
                size_t base_wall = rand()%28;
                struct move_t move;
                move.m = rand()%36;  
                move.e[0].fr = base_wall;
                move.e[0].to = base_wall + 6;
                move.e[1].fr = base_wall + 1;
                move.e[1].to = base_wall + 7;
                move.t = MOVE;
                move.c = rand()%2;
                bool val_disp = is_valid_displacement(my_smaller_board, move.m, move.c);
                bool mov_val = move_is_valid(&move, my_smaller_board, move.c);
                if (val_disp && !mov_val){
                    FAIL("!move_is_valid but found is_valid_displacement");
                    error = true;
                }
                if (!val_disp && mov_val){
                    FAIL("move_is_valid but found !is_valid_displacement");
                    error = true;
                }

                move.t = WALL;
                bool val_wall = is_valid_wall(my_smaller_board, move.e);
                mov_val = move_is_valid(&move, my_smaller_board, move.c);
                if (val_wall && !mov_val){
                    FAIL("!move_is_valid but found is_valid_wall");
                    error = true;
                }
                if (!val_wall && mov_val){
                    FAIL("move_is_valid but found !is_valid_wall");
                    error = true;
                }
            }
    }
    remove_wall(my_smaller_board, bw1);
    remove_wall(my_smaller_board, bw2);
    remove_wall(my_smaller_board, bw3);
    remove_wall(my_smaller_board, bw4);
    remove_wall(my_smaller_board, bw5);
    dlclose(P1_lib);
    dlclose(P2_lib);
    freopen("/dev/tty","r",stderr);
}
void test_server_main(void){
    TEST(test_is_winning);
    TEST(test_is_valid_displacement);
    TEST(test_is_valid_wall);
    TEST(test_move_is_valid);
    TEST(test_update_board);
    SUMMARY();
}