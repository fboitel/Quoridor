#include "opt.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>


int board_size = -1;
enum shape_t board_shape = INVALID_SHAPE;
char *player_1_path = NULL;
char *player_2_path = NULL;


enum shape_t parse_board_shape(char *t) {
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
	fprintf(stderr, "Usage: %s [-m SIZE] [-t SHAPE] <PLAYER_1_PATH> <PLAYER_2_PATH>\n", exec_path);
}

void assert(bool condition, char *exec_path, char *message) {
	if (!condition) {
		usage(exec_path, message);
		exit(EXIT_FAILURE);
	}
}

void parse_args(int argc, char **argv) {
	for (int i = 1; i < argc; ++i) {
		char *arg = argv[i];

		if (strcmp(arg, "-m") == 0) {
			assert(board_size == -1, argv[0], "\"-m\" option can not be used multiple times.");
			assert(i + 1 < argc, argv[0], "\"-m\" option must be followed by the board size.");

			board_size = atoi(argv[++i]);
			assert(board_size > 0, argv[0], "Board size must be a strictly positive number.");

		} else if (strcmp(arg, "-t") == 0) {
			assert(board_shape == INVALID_SHAPE, argv[0], "\"-t\" option can't be used multiple times.");
			assert(i + 1 < argc, argv[0], "\"-t\" option must be followed by the board shape.");

			board_shape = parse_board_shape(argv[++i]);
			assert(board_shape != INVALID_SHAPE, argv[0], "Board shape must be \"c\", \"t\", \"h\" or \"s\".");

		} else {
			assert(player_2_path == NULL, argv[0], "There is too much players.");

			if (access(argv[i], F_OK)) {
				char *message_start = "File \"";
				char *message_end = "\" doesn't exists.";
				char message[strlen(message_start) + strlen(argv[i]) + strlen(message_end) + 1];
				sprintf(message, "%s%s%s", message_start, argv[i], message_end);
				assert(false, argv[0], message);
			}

			if (player_1_path == NULL) {
				player_1_path = argv[i];
			} else {
				player_2_path = argv[i];
			}
		}
	}

	assert(player_2_path != NULL, argv[0], "Not enough players.");

	if (board_shape == INVALID_SHAPE) {
		board_shape = SQUARE;
	}

	if (board_size == -1) {
		board_size = 15;
	}

	switch (board_shape) {
		case TORIC:
		case H:
			assert(board_shape % 3 == 0, argv[0], "Using this shape, board size must be a multiple of 3.");
			break;
		case SNAKE:
			assert(board_shape % 5 == 0, argv[0], "Using this shape, board size must be a multiple of 5.");
			break;
		default:
			break;
	}
}
