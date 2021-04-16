#include "board.h"
#include "graph.h"
#include "move.h"
#include "opt.h"
#include <dlfcn.h>          // to use dynamic libs
#include <gsl/gsl_matrix.h> // for matrix
#include <gsl/gsl_matrix_double.h>
#include <stdio.h>

#include <stdlib.h>
#include <time.h>   // for random
#include <unistd.h> // to check file existence

extern char *player_1_path;
extern char *player_2_path;

void *P1_lib;
void (*P1_initialize)(enum color_t id, struct graph_t *graph, size_t num_walls);
char *(*P1_name)(void);
struct move_t (*P1_play)(struct move_t previous_move);
void (*P1_finalize)();

void *P2_lib;
void (*P2_initialize)(enum color_t id, struct graph_t *graph, size_t num_walls);
char *(*P2_name)(void);
struct move_t (*P2_play)(struct move_t previous_move);

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

int is_winning(enum color_t active_player) {
  // TODO : check with board
  (void)active_player;
  return 1;
}

void update(struct graph_t *graph, struct move_t move) {
  if (move.t == WALL)
    add_edges(graph, move.e);
}

// Compute the next player
enum color_t get_next_player(enum color_t player) { return 1 - player; }

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  printf("Args parsed\n");

  srand(time(NULL));

  // Load players
  load_libs();
  printf("Libs loaded\n");

  // Initialize a new board of size m and shape t
  // TODO : init a new board depending on parameters
  size_t m = 2;
  struct graph_t *board = graph_init(m, SQUARE);
  printf("Board created\n");

  // TODO : compute num_walls depending on size and board shape
  int num_walls = 10;

  // Init random starting player
  enum color_t active_player = rand() % 2;

  // init players
  P1_initialize(BLACK, board, num_walls);
  P2_initialize(WHITE, board, num_walls);
  printf("Players initialized\n");
  printf("\n");
  printf("%s vs %s\n", P1_name(), P2_name());
  printf("%s begins\n", active_player == BLACK ? P1_name() : P2_name());
  // Initialize the first move as a move to the intial place
  struct move_t last_move = {.m = SIZE_MAX,
                             .c = active_player,
                             .e = {no_edge(), no_edge()},
                             .t = NO_TYPE};

  // display adj matrix
  for (size_t i = 0; i < m * m; ++i) {
    for (size_t j = 0; j < m * m; ++j) {
      printf("% d ", gsl_spmatrix_uint_get(board->t, i, j));
    }
    printf("\n");
  }

  // display board

  char *next_line = "";

  for (size_t i = 0; i < m * m; ++i) {

    if (i % m == 0) {
      printf("\n");
    }
    int matrix_state_1 = 0;
    if (i + 1 < m * m) {
      matrix_state_1 = gsl_spmatrix_uint_get(board->t, i, i + 1);
    }
    int matrix_state_2 = 0;

    if (i + m < m * m) {
      matrix_state_2 = gsl_spmatrix_uint_get(board->t, i, i + m);
    }

    printf("0");

    if (matrix_state_1 == 4) {
      printf(" - ");
    }
    if (matrix_state_2 == 2) {
      printf("|");
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

  // Game loop
  bool game_over = false;
  while (!game_over) {
    // Play
    last_move =
        active_player == BLACK ? P1_play(last_move) : P2_play(last_move);
    // Update the graph if a player has put a wall
    update(board, last_move);
    // Check if a player has won
    if (is_winning(active_player))
      game_over = true;
    else
      active_player = get_next_player(active_player);
  }
  printf("GAME OVER\n");
  printf("%s won\n", active_player == BLACK ? P1_name() : P2_name());

  P1_finalize();
  P2_finalize();
  graph_free(board);

  return EXIT_SUCCESS;
}
