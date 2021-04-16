#include "board.h"
#include "graph.h"
#include "move.h"
#include "opt.h"
#include <string.h>
#include <dlfcn.h>          // to use dynamic libs
#include <gsl/gsl_matrix.h> // for matrix
#include <gsl/gsl_matrix_double.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   // for random
#include <unistd.h> // to check file existence

extern char* player_1_path;
extern char* player_2_path;

void* P1_lib;
void (*P1_initialize)(enum color_t id, struct graph_t* graph, size_t num_walls);
char* (*P1_name)(void);
struct move_t(*P1_play)(struct move_t previous_move);
void (*P1_finalize)();

void* P2_lib;
void (*P2_initialize)(enum color_t id, struct graph_t* graph, size_t num_walls);
char* (*P2_name)(void);
struct move_t(*P2_play)(struct move_t previous_move);

void (*P2_finalize)();

// Open players libs
int load_libs(void) {
    // TODO : check fails
    P1_lib = dlopen(player_1_path, RTLD_NOW);
    char* error = dlerror();

    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    P1_initialize = dlsym(P1_lib, "initialize");
    P1_name = dlsym(P1_lib, "get_player_name");
    P1_play = dlsym(P1_lib, "play");
    P1_finalize = dlsym(P1_lib, "finalize");

    P2_lib = dlopen(player_2_path, RTLD_LAZY);
    error = dlerror();

    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }
    if (P1_lib == NULL) {
        printf("Path to player's 2 library is unreachable.\n");
        exit(EXIT_FAILURE);
    }

    P2_initialize = dlsym(P2_lib, "initialize");
    P2_name = dlsym(P2_lib, "get_player_name");
    P2_play = dlsym(P2_lib, "play");
    P2_finalize = dlsym(P2_lib, "finalize");

    return EXIT_SUCCESS;
}

// Check if the player is winning
// by checking if he is on a vertex owned by the other player
bool is_winning(struct graph_t* board, enum color_t active_player, struct move_t move) {
    return gsl_spmatrix_uint_get(board->o, 1 - active_player, move.m);
}

void update(struct graph_t* graph, struct move_t move) {
    if (move.t == WALL)
        add_edges(graph, move.e);
}

// Compute the next player
enum color_t get_next_player(enum color_t player) {
    return 1 - player;
}

// Check move validity TODO
bool check(struct move_t mv, struct graph_t* board, enum color_t player) {
    // Check type
    if (mv.t != WALL && mv.t < MOVE) {
        fprintf(stderr, "Error from %s: incorrect move type\n", player == BLACK ? P1_name() : P2_name());
        return false;
    }
    // Check color
    if (mv.c != player) {
        fprintf(stderr, "Error from %s: incorrect color\n", player == BLACK ? P1_name() : P2_name());
        return false;
    }
    // Check if edges form a wall
    if (mv.t == WALL) {
        // Check if edges are inside the board
        for (size_t i = 0; i < 2; i++) {
            if (mv.e[i].fr >= board->t->size1)
                return false;
            if (mv.e[i].to >= board->t->size1)
                return false;
        }
        // Check if edges are aligned
        if (!((mv.e[0].fr == mv.e[0].to && mv.e[0].to == mv.e[1].fr && mv.e[1].fr == mv.e[1].to) ||
            (mv.e[0].fr == mv.e[0].to && mv.e[1].fr == mv.e[1].to)))

            return 1; // TODO
    }
    else {
        return 1; // TODO
    }
    return true;
}

void display_board(struct graph_t* board, size_t board_size) {
    // display board

    // care about out of tab
    char next_line[100] = "";

    for (size_t i = 0; i < board_size * board_size; ++i) {

        if (i % board_size == 0) {
            printf("\n");
            printf("%s", next_line);
            printf("\n");
            strcpy(next_line, "");
        }

        printf("0");

        if (i + 1 < board_size * board_size) {
            int matrix_state_1 = gsl_spmatrix_uint_get(board->t, i, i + 1);
            if (matrix_state_1 == 4) {
                printf(" - ");
            }
            else {
                printf("   ");
            }
        }

        if (i + board_size < board_size * board_size) {
            int matrix_state_2 = gsl_spmatrix_uint_get(board->t, i, i + board_size);
            if (matrix_state_2 == 2) {
                strcat(next_line, "|   ");
            }
            else {
                printf("    ");
            }
        }
    }
    printf("\n");

    /*
   printf("0 - 0 - 0 - 1\n");
   printf("|   |   |   |\n");
   printf("0 - 0 - 0 - 0\n");
   printf("|   |       |\n");
   printf("0 - 0 - 0 - 0\n");
   printf("|   |   |   |\n");
   printf("0 - 0 - 0 - 0\n");
   */

   /*printf("0");
    int matrix_state_1 = 0;
    int matrix_state_2 = 0;
    if (i + 1 < board_size * board_size) {
      matrix_state_1 = gsl_spmatrix_uint_get(board->t, i, i + board_size);
    }

    if (i + board_size < board_size * board_size) {
      matrix_state_2 = gsl_spmatrix_uint_get(board->t, i, i + board_size);
    }

    if (i + board_size < board_size * board_size) {
      int matrix_state_2 = gsl_spmatrix_uint_get(board->t, i, i + board_size);
      if (matrix_state_2 == 2) {
        strcat(next_line, "| ");
      } else if (matrix_state_2 == 7) {
        strcat(next_line, "---");
      } else if (matrix_state_2 == 8) {
        strcat(next_line, "-");
      } else {
        strcat(next_line, "  ");
      }
    }

    if (i + 1 < board_size * board_size) {
      // get data from matrix
      int matrix_state_1 = gsl_spmatrix_uint_get(board->t, i, i + 1);

      // if connected to east node
      if (matrix_state_1 == 4) {
        printf(" - ");
        strcat(next_line, "  ");
      } else if (matrix_state_1 == 5) { // if there is a wall with east node
        printf(" | ");
        strcat(next_line, "| ");
      } else if (matrix_state_1 == 6) { // if there is a wall with east node
        printf(" | ");
        strcat(next_line, "  ");
      } else { // if there are no connection with east node
        printf("   ");
        strcat(next_line, "  ");
      }*/
}

int main(int argc, char* argv[]) {

    // Parse arguments
    parse_args(argc, argv);
    printf("Args parsed\n");

    // Init random generator
    srand(time(NULL));

    // Load players
    load_libs();
    printf("Libs loaded\n");

    // Initialize a new board of size m and shape t
// TODO : init a new board depending on parameters
    size_t m = 4;
    struct graph_t* board = graph_init(m, SQUARE);
    struct graph_t* boardCopy1 = graph_init(m, SQUARE);
    struct graph_t* boardCopy2 = graph_init(m, SQUARE);
    printf("Board created\n");

    // TODO : compute num_walls depending on size and board shape
    int num_walls = 10;

    // Init random starting player
    enum color_t active_player = rand() % 2;
    enum color_t winner;

    // init players
    // init players
    P1_initialize(BLACK, boardCopy1, num_walls);
    P2_initialize(WHITE, boardCopy2, num_walls);
    printf("Players initialized\n");
    printf("\n");
    printf("%s vs %s\n", P1_name(), P2_name());
    printf("%s begins\n", active_player == BLACK ? P1_name() : P2_name());
    // Initialize the first move as a move to the intial place
    struct move_t last_move = { .m = SIZE_MAX,
                               .c = active_player,
                               .e = {no_edge(), no_edge()},
                               .t = NO_TYPE };

    // display adj matrix
    for (size_t i = 0; i < m * m; ++i) {
        for (size_t j = 0; j < m * m; ++j) {
            printf("% d ", gsl_spmatrix_uint_get(board->t, i, j));
        }
        printf("\n");
    }

    display_board(board, m);

    // Game loop
    bool game_over = false;
    while (!game_over) {
        // Play
        last_move = active_player == BLACK ? P1_play(last_move) : P2_play(last_move);

        // Check move validity
        if (0) {// || !check(last_move, board, active_player)) {
            printf("Invalid move from %s\n", active_player == BLACK ? P1_name() : P2_name());
            winner = get_next_player(active_player);
            break;
        }

        // Update the graph if a player has put a wall
        update(board, last_move);

        // Check if a player has won
        if (is_winning(board, active_player, last_move)) {
            printf("Win\n");
            winner = active_player;
            game_over = true;
        }
        else {
            printf("Not win\n");
            active_player = get_next_player(active_player);
        }
    }
    printf("GAME OVER\n");
    printf("%s won\n", winner == BLACK ? P1_name() : P2_name());

    P1_finalize();
    P2_finalize();
    graph_free(board);

    return EXIT_SUCCESS;
}
