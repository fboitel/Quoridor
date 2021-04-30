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

bool game_over = false;
size_t position_player_1 = -1;
size_t position_player_2 = -1;
enum color_t active_player = -1;
enum color_t winner = -1;
size_t turn = 0;


enum reasons_t { WIN = 0, INVALID_MOVE = 1 };

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

// Compute the next player
enum color_t get_next_player(enum color_t player) { return 1 - player; }

// Return if the displacement of the player is valid
bool is_valid_displacement(struct graph_t* board, size_t destination, enum color_t player) {

	size_t position_player = player == BLACK ? position_player_1 : position_player_2;

	size_t position_opposent = player == BLACK ? position_player_2 : position_player_1;

	// destination is in the board and destination is not on a player cell
	if (board->num_vertices <= destination || position_player == destination || position_opposent == destination) {
		return false;
	}

	// check first move
	if (position_player == -1) {
		if (player == BLACK) {
			return destination < board_size;
		}
		return board->num_vertices - board_size < destination&& destination < board->num_vertices;
	}

	// check destination is in the 4 near cells :
	if (is_linked(board, position_player, destination)) {
		return true;
	}

	// check others destination manually
	// TODO : try to find something more clean, but it's hard

	// move 2 on the left
	if (position_player % board_size > 1 &&
		position_player - destination == 2 &&
		is_linked(board, position_player, position_player - 1) &&
		is_linked(board, position_player - 1, destination) &&
		position_opposent == position_player - 1) {
		return true;
	}

	// move 2 on the right
	if (position_player % board_size < board_size - 2 &&
		destination - position_player == 2 &&
		is_linked(board, position_player, position_player + 1) &&
		is_linked(board, position_player + 1, destination) &&
		position_opposent == position_player + 1) {
		return true;
	}

	// move 2 on the top
	if (position_player / board_size > 1 &&
		destination + 2 * board_size == position_player &&
		is_linked(board, position_player, position_player - board_size) &&
		is_linked(board, position_player - board_size, destination) &&
		position_opposent == position_player - board_size) {
		return true;
	}

	// move 2 on the bottom
	if (position_player / board_size < board_size - 2 &&
		position_player + 2 * board_size == destination &&
		is_linked(board, position_player, position_player + board_size) &&
		is_linked(board, position_player + board_size, destination) &&
		position_opposent == position_player + board_size) {
		return true;
	}

	// move top + left
	if (position_player / board_size > 0 &&
		position_player % board_size > 0 &&
		position_player - board_size == destination + 1 &&
		(
			(is_linked(board, position_player, position_player - board_size) &&
				is_linked(board, position_player - board_size, destination) &&
				position_opposent == position_player - board_size
				)
			||
			(is_linked(board, position_player, position_player - 1) &&
				is_linked(board, position_player - 1, destination) &&
				position_opposent == position_player -1)
			)
		) {
		return true;
	}

	// move top + right
	if (position_player / board_size > 0 &&
		position_player % board_size < board_size - 1 &&
		position_player - board_size == destination - 1 &&
		(
			(is_linked(board, position_player, position_player - board_size) &&
				is_linked(board, position_player - board_size, destination) &&
				position_opposent == position_player - board_size
				)
			||
			(is_linked(board, position_player, position_player + 1) &&
				is_linked(board, position_player + 1, destination) &&
				position_opposent == position_player + 1)
			)
		) {
		return true;
	}

	// move bot + right
	if (position_player / board_size < board_size -1 &&
		position_player % board_size < board_size -1 &&
		position_player + board_size == destination - 1 &&
		(
			(is_linked(board, position_player, position_player + board_size) &&
				is_linked(board, position_player + board_size, destination) &&
				position_opposent == position_player + board_size
				)
			||
			(is_linked(board, position_player, position_player + 1) &&
				is_linked(board, position_player + 1, destination) &&
				position_opposent == position_player + 1)
			)
		) {
		return true;
	}

	// move bot + left
	if (position_player / board_size < board_size -1 &&
		position_player % board_size > 0 &&
		position_player + board_size == destination + 1 &&
		(
			(is_linked(board, position_player, position_player + board_size) &&
				is_linked(board, position_player + board_size, destination) &&
				position_opposent == position_player + board_size
				)
			||
			(is_linked(board, position_player, position_player - 1) &&
				is_linked(board, position_player - 1, destination) &&
				position_opposent == position_player - 1)
			)
		) {
		return true;
	}

	//gsl_spmatrix_uint_set(board->t, position_player, destination, 5);

	return true;
}

bool is_valid_wall(struct graph_t* board, struct edge_t e[]) {
	return
		(//FIXME : some wall will be refused unfairly
			// vertical wall
			vertex_from_direction(board, e[0].fr, EAST) == e[0].to &&
			vertex_from_direction(board, e[1].fr, EAST) == e[1].to &&
			(vertex_from_direction(board, e[0].fr, SOUTH) == e[1].fr ||
				vertex_from_direction(board, e[0].to, SOUTH) == e[1].to)
			) || (
				// horizontal wall
				vertex_from_direction(board, e[0].fr, SOUTH) == e[0].to &&
				vertex_from_direction(board, e[1].fr, SOUTH) == e[1].to &&
				(vertex_from_direction(board, e[0].fr, EAST) == e[1].fr ||
					vertex_from_direction(board, e[0].to, EAST) == e[1].to)
				);
}


void end_game(enum reasons_t reason) {
	winner = reason == WIN ? active_player : get_next_player(active_player);
	game_over = true;
}

// Check move validity
bool move_is_valid(struct move_t* mv, struct graph_t* board, enum color_t player) {

	// Check type
	if (!(mv->t == WALL || mv->t == MOVE)) {
		fprintf(stderr, "Error from %s: incorrect move type (%u)\n", player == BLACK ? P1_name() : P2_name(), mv->t);
		end_game(INVALID_MOVE);
		return false;
	}

	// Check color
	if (mv->c != player) {
		fprintf(stderr, "Error from %s: incorrect color (%u)\n", player == BLACK ? P1_name() : P2_name(), mv->c);
		end_game(INVALID_MOVE);
		return false;
	}

	// Check if move respect rules
	if (mv->t == MOVE) {
		if (!is_valid_displacement(board, mv->m, player)) {
			fprintf(stderr, "Error from %s: bad displacement %zu --> %zu\n", player == BLACK ? P1_name() : P2_name(), player == BLACK ? position_player_1 : position_player_2, mv->m);
			end_game(INVALID_MOVE);
			return false;
		}
	}


	// Check if edges create a wall
	if (mv->t == WALL) {
		if (!is_valid_wall(board, mv->e)) {
			fprintf(stderr, "Error from %s: bad wall (%zu, %zu), (%zu, %zu)\n", player == BLACK ? P1_name() : P2_name(), mv->e[0].fr, mv->e[0].to, mv->e[1].fr, mv->e[1].to);
			end_game(INVALID_MOVE);
			return false;
		}
	}

	// TODO : check valid moves
	return true;
}

void update_board(struct graph_t* board, struct move_t* last_move) {

	// Update players positions if they move
	if (last_move->t == MOVE) {
		if (active_player == BLACK) {
			position_player_1 = last_move->m;
		}
		else {
			position_player_2 = last_move->m;
		}
	}
	else {
		// Update the board if a player has put a wall
		placeWall(board, last_move->e);
	}
}

void close_server(struct graph_t* board) {
	P1_finalize();
	P2_finalize();
	graph_free(board);
	dlclose(P1_lib);
	dlclose(P2_lib);
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
	// TODO : init a new board depending on parameters + copy
	size_t m = board_size;
	struct graph_t* board = graph_init(m, SQUARE);
	struct graph_t* boardCopy1 = graph_init(m, SQUARE);
	struct graph_t* boardCopy2 = graph_init(m, SQUARE);
	printf("Board created\n");

	// TODO : compute num_walls depending on size and board shape
	int num_walls = 10;

	// Init random starting player
	active_player = rand() % 2;

	// init players
	P1_initialize(BLACK, boardCopy1, num_walls);
	P2_initialize(WHITE, boardCopy2, num_walls);
	printf("Players initialized\n");
	printf("\n");
	printf("%s vs %s\n", P1_name(), P2_name());
	printf("%s begins\n", active_player == BLACK ? P1_name() : P2_name());

	// Initialize the first move as a move to the intial place
	struct move_t last_move = (struct move_t){
			.m = SIZE_MAX,
			.c = active_player,
			.e = {no_edge(), no_edge()},
			.t = NO_TYPE
	};

	// Game loop
	while (!game_over) {
		turn++;

		// Plays the active player
		last_move = active_player == BLACK ? P1_play(last_move) : P2_play(last_move);

		// Check move validity
		if (!move_is_valid(&last_move, board, active_player)) {
			break;
		}

		update_board(board, &last_move);

		display_board(board, m, position_player_1, position_player_2);
		//	printf("wall : %ld:%ld - %ld:%ld \n", last_move.e[0].fr,  last_move.e[0].to,  last_move.e[1].fr,  last_move.e[1].to);

		// Check if a player has won
		if (is_winning(board, active_player, active_player == BLACK ? position_player_1 : position_player_2)) {
			end_game(WIN);
			break;
		}

		active_player = get_next_player(active_player);
	}

	printf("GAME OVER\n");
	printf("%s won\n", winner == BLACK ? P1_name() : P2_name());
	printf("Finish after %zu turns\n", turn);

	close_server(board);

	return EXIT_SUCCESS;
}
