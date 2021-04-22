#include "opt.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> // strcmp


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
	fprintf(stderr,
	        "Usage: %s [-m SIZE] [-t SHAPE] <PLAYER_1_PATH> <PLAYER_2_PATH>\n",
	        exec_path);
}

void parse_args(int argc, char **argv) {
	for (int i = 1; i < argc; ++i) {
		char *arg = argv[i];

		if (!strcmp(arg, "-m")) {
			if (board_size != -1) {
				usage(argv[0], "\"-m\" option can not be used multiple times.");
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
