#include "../headers/graph.h"
#include "../headers/move.h"
#include <dlfcn.h>          // to use dynamic libs
#include <gsl_matrix.h> // for matrix
#include <gsl_matrix_double.h>
#include <stdlib.h>
#include <string.h> // for strcmp
#include <unistd.h> // to check file existence

enum board_shape_t { SQUARE, TORIC, H, SNAKE, INVALID_SHAPE = -1 };

int board_size = -1;
enum board_shape_t board_shape = INVALID_SHAPE;
char *player_1_path = NULL;
char *player_2_path = NULL;

enum board_shape_t parse_board_shape(char *t) {
  if (strlen(t) != 1) {
    return INVALID_SHAPE;
  }

  switch (*t) {
  case 'c':
    return SQUARE;

  case 't':
    return TORIC;

  case 'h':
    return H;

  case 's':
    return SNAKE;

  default:
    return INVALID_SHAPE;
  }
}

void usage(char *exec_path, char *message) {
  if (message != NULL) {
    fprintf(stderr, "%s\n\n", message);
  }
  fprintf(stderr,
          "Usage: %s [-m SIZE] [-t SHAPE] <PLAYER_1_PATH> <PLAYER_2_PATH>\n",
          exec_path);
}

void parse_args(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    char *arg = argv[i];

    if (!strcmp(arg, "-m")) {
      if (board_size != -1) {
        usage(argv[0], "\"-m\" option can't be used multiple times.");
        exit(EXIT_FAILURE);
      }

      ++i;

      if (i == argc) {
        usage(argv[0], "\"-m\" option must be followed by the board size.");
        exit(EXIT_FAILURE);
      }

      if ((board_size = atoi(argv[i])) <= 0) {
        usage(argv[0], "Board size must be a strictly positive number.");
        exit(EXIT_FAILURE);
      }

    } else if (!strcmp(arg, "-t")) {
      if (board_shape != INVALID_SHAPE) {
        usage(argv[0], "\"-t\" option can't be used multiple times.");
        exit(EXIT_FAILURE);
      }

      ++i;

      if (i == argc) {
        usage(argv[0], "\"-t\" option must be followed by the board shape.");
        exit(EXIT_FAILURE);
      }

      if ((board_shape = parse_board_shape(argv[i])) == INVALID_SHAPE) {
        usage(argv[0], "Board shape must be \"c\", \"t\", \"h\" or \"s\".");
        exit(EXIT_FAILURE);
      }

    } else {
      if (player_2_path != NULL) {
        printf(argv[0], "There is too much players.");
        exit(EXIT_FAILURE);
      }

      if (access(argv[i], F_OK)) {
        char *message_start = "File \"";
        char *message_end = "\" doesn't exists.";
        char message[strlen(message_start) + strlen(argv[i]) +
                     strlen(message_end) + 1];
        sprintf(message, "%s%s%s", message_start, argv[i], message_end);
        usage(argv[0], message);
        exit(EXIT_FAILURE);
      }

      if (player_1_path == NULL) {
        player_1_path = argv[i];
      } else {
        player_2_path = argv[i];
      }
    }
  }

  if (player_2_path == NULL) {
    usage(argv[0], "Not enough players.");
    exit(EXIT_FAILURE);
  }

  if (board_shape == INVALID_SHAPE) {
    board_shape = SQUARE;
  }

  if (board_size == -1) {
    board_size = 15;
  } else {
    switch (board_shape) {
    case TORIC:
    case H:
      if (board_shape % 3 != 0) {
        usage(argv[0], "Using this shape, board size must be a multiple of 3.");
        exit(EXIT_FAILURE);
      }
      break;
    case SNAKE:
      if (board_shape % 5 != 0) {
        usage(argv[0], "Using this shape, board size must be a multiple of 5.");
        exit(EXIT_FAILURE);
      }
      break;
    default:
      break;
    }
  }
}

int gameOver = 0;

void (*initializePlayer1)(enum color_t id, struct graph_t *graph,
                          size_t num_walls);
struct move_t (*playPlayer1)(struct move_t previous_move);
void (*finalizePlayer1)();

void (*initializePlayer2)(enum color_t id, struct graph_t *graph,
                          size_t num_walls);
struct move_t (*playPlayer2)(struct move_t previous_move);
void (*finalizePlayer2)();

int loadLibs() {
  // open players libs
  // TODO : check fails
  void *libPlayer1 = dlopen(player_1_path, RTLD_LAZY);
  *(void **)(initializePlayer1) = dlsym(libPlayer1, "initialise");
  *(struct move_t **)(playPlayer1) = dlsym(libPlayer1, "play");
  *(void **)(finalizePlayer1) = dlsym(libPlayer1, "finalize");

  void *libPlayer2 = dlopen(player_1_path, RTLD_LAZY);
  *(void **)(initializePlayer2) = dlsym(libPlayer2, "initialise");
  *(struct move_t **)(playPlayer2) = dlsym(libPlayer2, "play");
  *(void **)(finalizePlayer2) = dlsym(libPlayer2, "finalize");
}

int isWinning(int activePlayer) {
  // TODO : check with board
  return 1;
}

int main(int argc, char *argv[]) {
  parse_args(argc, argv);

  // TODO : check fails
  loadLibs();

  // init a new matrix size of 3x3
  // gsl_matrix *board = gsl_matrix_alloc(3, 3);
  // TODO : ini a new board depending on parameters
  struct graph_t board = {0};

  // free the matrix
  // gsl_matrix_free(board);

  // TODO : calcul numWall depending on size and board shape
  int numWall = 10;

  // TODO : init random start player
  int activePlayer = 1;

  // init players
  initializePlayer1(BLACK, &board, numWall);
  initializePlayer2(WHITE, &board, numWall);

  // todo init move
  struct move_t last_move;
  while (!gameOver) {
    if (activePlayer == 1) {
      last_move = playPlayer1(last_move);
    } else {
      last_move = playPlayer2(last_move);
    }
    if (isWinning(activePlayer)) {
      gameOver = 1;
    }
  }
  finalizePlayer1();
  finalizePlayer2();

  /* pseudo code for game loop
  start_player = random()
  for each player p
          p->initialize(p == start_player ? BLACK : WHITE, graph, num_walls)
  while true
     p = compute_next_player()
     move = p->play(move)
     if is_winning()
            break
  for each player p
     p->finalize()
     */
}
