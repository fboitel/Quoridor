#include <gsl/gsl_matrix.h> // for matrix
#include <stdlib.h>
#include <string.h>         // for strcmp
#include <unistd.h>         // to check file existence
#include "player.h"         // temporary


enum board_shape_t {
	SQUARE, TORIC, H, SNAKE, INVALID_SHAPE = -1
};

int board_size = 10;
enum board_shape_t boardShape = SQUARE;
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

void usage(char* exec_path, char *message) {
	if (message != NULL) {
		fprintf(stderr, "%s\n\n", message);
	}
	fprintf(stderr, "Usage: %s [-m SIZE] [-t SHAPE] <PLAYER_1_PATH> <PLAYER_2_PATH>\n", exec_path);
}

void parse_args(int argc, char **argv) {
	for (int i = 1; i < argc; ++i) {
		char *arg = argv[i];

		if (!strcmp(arg, "-m")) {
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
			++i;

			if (i == argc) {
				usage(argv[0], "\"-t\" option must be followed by the board shape.");
				exit(EXIT_FAILURE);
			}

			if ((boardShape = parse_board_shape(argv[i])) == INVALID_SHAPE) {
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
				char message[strlen(message_start) + strlen(argv[i]) + strlen(message_end) + 1];
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
}

int main(int argc, char *argv[]) {

	parse_args(argc, argv);

	// init a new matrix size of 3x3
	gsl_matrix *board = gsl_matrix_alloc(3, 3);

	// free the matrix
	gsl_matrix_free(board);

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
