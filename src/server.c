#include "board.h"
#include "graph.h"
#include "move.h"
#include "opt.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern char* player_1_path;
extern char* player_2_path;
extern int board_size;

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
bool is_winning(struct graph_t* board, enum color_t active_player, size_t position) {
	return gsl_spmatrix_uint_get(board->o, 1 - active_player, position);
}

void update(struct graph_t* graph, struct move_t move) {
	if (move.t == WALL)
		add_edges(graph, move.e);
}

// Compute the next player
enum color_t get_next_player(enum color_t player) { return 1 - player; }

bool is_valid_wall(struct graph_t* board, struct edge_t e[]) {
	return
		(
			vertex_from_direction(board, e[0].fr, EAST) == e[0].to &&
			vertex_from_direction(board, e[1].fr, EAST) == e[1].to &&
			vertex_from_direction(board, e[0].fr, SOUTH) == e[1].fr &&
			vertex_from_direction(board, e[0].to, SOUTH) == e[1].to) ? true :
		(
			vertex_from_direction(board, e[0].fr, SOUTH) == e[0].to &&
			vertex_from_direction(board, e[1].fr, SOUTH) == e[1].to &&
			vertex_from_direction(board, e[0].fr, EAST) == e[1].fr &&
			vertex_from_direction(board, e[0].to, EAST) == e[1].to) ? true :
		false;
}

// Check move validity
bool check(struct move_t mv, struct graph_t* board, enum color_t player) {
	// Check type
	if (mv.t < 0 || mv.t >= NO_TYPE) {
		fprintf(stderr, "Error from %s: incorrect move type (%u)\n",
			player == BLACK ? P1_name() : P2_name(), mv.t);
		return false;
	}

	// Check color
	if (mv.c != player) {
		fprintf(stderr, "Error from %s: incorrect color (%u)\n",
			player == BLACK ? P1_name() : P2_name(), mv.c);
		return false;
	}

	// Check if edges create a wall
	if (mv.t == WALL) {
		if (!is_valid_wall(board, mv.e)) {
			fprintf(stderr, "Error from %s: bad wall (%zu, %zu), (%zu, %zu)\n",
				player == BLACK ? P1_name() : P2_name(), mv.e[0].fr, mv.e[0].to, mv.e[1].fr, mv.e[1].to);
			return false;
		}
	}
	return true;
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
	size_t m = board_size;
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

	/*
	// display adj matrix
	for (size_t i = 0; i < m * m; ++i) {
		for (size_t j = 0; j < m * m; ++j) {
			printf("% d ", gsl_spmatrix_uint_get(board->t, i, j));
		}
		printf("\n");
	}*/

	size_t position_player_1 = -1;
	size_t position_player_2 = -1;

	// Game loop
	bool game_over = false;
	size_t turn = 0;

	while (!game_over) {
		turn++;
		// Play
		last_move =
			active_player == BLACK ? P1_play(last_move) : P2_play(last_move);

		// Check move validity
		if (!check(last_move, board, active_player)) {
			printf("Invalid move from %s\n",
				active_player == BLACK ? P1_name() : P2_name());
			winner = get_next_player(active_player);
			break;
		}

		// Update the graph if a player has put a wall
		update(board, last_move);

		// Update display
		if (last_move.t == MOVE) {
			if (active_player == BLACK) {
				position_player_1 = last_move.m;
			}
			else {
				position_player_2 = last_move.m;
			}
		}
		/* TODO: update display
		if (last_move.t == WALL) {
			for (int i = 0; i < 2; i++) {
				if (!is_no_edge(last_move.e[i])) {
					size_t first_node = last_move.e[i].fr;
					size_t second_node = last_move.e[i].to;
					gsl_spmatrix_uint_set(board->t, i, j, )
				}
			}
		}*/
		display_board(board, m, position_player_1, position_player_2);

		// Check if a player has won
		if (is_winning(board, active_player, 
		active_player == BLACK ? position_player_1 : position_player_2)) {
			winner = active_player;
			game_over = true;
		}
		else {
			active_player = get_next_player(active_player);
		}
	}
	printf("GAME OVER\n");
	printf("%s won\n", winner == BLACK ? P1_name() : P2_name());
	printf("Finish after %zu turns\n", turn);
	P1_finalize();
	P2_finalize();
	graph_free(board);
	dlclose(P1_lib);
	dlclose(P2_lib);

	return EXIT_SUCCESS;
}
