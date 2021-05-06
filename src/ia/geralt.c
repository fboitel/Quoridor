#include <ia_utils.h>
#include "ia.h"
#include "move.h"


#define EDGE(graph, n, i, j) ((graph)[(i) * (n) + (j)])
#define DISPLACEMENT_MOVE(dest) ((SimpleMove) {MOVE, {(dest), 0, 0, 0}})


// define simplified structures to gain efficiency

typedef struct {
	enum movetype_t type;
	int action[4];
} SimpleMove;

typedef struct {
	char *graph;
	int pos;
	int num_walls;
	int opponent_pos;
	int opponent_num_walls;
} SimpleGameState;


char *name = "Geralt";


void add_displacement_moves(const char *graph, int n, SimpleMove *moves, int *nb_of_moves, int player_pos, int opponent_pos, char main_direction, char secondary_direction) {
	int step_1 = player_pos + main_direction;
	if (step_1 < 0 || step_1 >= n || !EDGE(graph, n, player_pos, step_1)) {
		return;
	}

	if (step_1 != opponent_pos) {
		moves[*nb_of_moves] = DISPLACEMENT_MOVE(step_1);
		*nb_of_moves = *nb_of_moves + 1;
		return;
	}

	int step_2 = step_1 + main_direction;
	if (step_2 >= 0 && step_2 < n && EDGE(graph, n, step_1, step_2)) {
		moves[*nb_of_moves] = DISPLACEMENT_MOVE(step_2);
		*nb_of_moves = *nb_of_moves + 1;
	}

	step_2 = step_1 + secondary_direction;
	if (step_2 >= 0 && step_2 < n && EDGE(graph, n, step_1, step_2)) {
		moves[*nb_of_moves] = DISPLACEMENT_MOVE(step_2);
		*nb_of_moves = *nb_of_moves + 1;
	}

	step_2 = step_1 - secondary_direction;
	if (step_2 >= 0 && step_2 < n && EDGE(graph, n, step_1, step_2)) {
		moves[*nb_of_moves] = DISPLACEMENT_MOVE(step_2);
		*nb_of_moves = *nb_of_moves + 1;
	}
}


SimpleMove *get_possible_moves(SimpleGameState *game, int n, int *nb_of_moves) {
	SimpleMove *moves = malloc((2 * (n - 1) * (n - 1) + 5) * sizeof(SimpleMove));
	*nb_of_moves = 0;

	add_displacement_moves(game->graph, n, moves, nb_of_moves, game->pos, game->opponent_pos, 1, 1);
	add_displacement_moves(game->graph, n, moves, nb_of_moves, game->pos, game->opponent_pos, 1, -1);
	add_displacement_moves(game->graph, n, moves, nb_of_moves, game->pos, game->opponent_pos, -1, 1);
	add_displacement_moves(game->graph, n, moves, nb_of_moves, game->pos, game->opponent_pos, -1, -1);

	moves = realloc(moves, *nb_of_moves * sizeof(SimpleMove));
	return moves;
}


SimpleMove get_rand_move(SimpleGameState *game, int n) {
	int nb_of_moves;
	SimpleMove *moves = get_possible_moves(game, n, &nb_of_moves);
	SimpleMove move = moves[rand() % nb_of_moves];
	free(moves);
	return move;
}


SimpleGameState compress_game(struct game_state_t game, int *n) {
	SimpleGameState compressed = {
		.pos = (int) game.self.pos,
		.num_walls = (int) game.self.num_walls,
		.opponent_pos = (int) game.opponent.pos,
		.opponent_num_walls = (int) game.opponent.num_walls
	};

	*n = (int) game.graph->t->size1;
	compressed.graph = malloc(*n * *n);

	for (int i = 0; i < *n; ++i) {
		for (int j = 0; j < *n; ++j) {
			EDGE(compressed.graph, *n, i, j) = (char) gsl_spmatrix_uint_get(game.graph->t, i, j);
		}
	}

	return compressed;
}


struct move_t expand_move(struct game_state_t game, SimpleMove move) {
	struct move_t expanded = {
			.c = game.self.color,
			.t = move.type,
			.m = move.type == MOVE ? move.action[0] : game.self.pos
	};

	switch (move.type) {
		case WALL:
			expanded.e[0] = (struct edge_t) {move.action[0], move.action[1]};
			expanded.e[1] = (struct edge_t) {move.action[2], move.action[3]};
			break;

		default:
			expanded.e[0] = no_edge();
			expanded.e[1] = no_edge();
			break;
	}

	return expanded;
}


struct move_t make_first_move(struct game_state_t game) {
	return make_default_first_move(game);
}


struct move_t make_move(struct game_state_t game) {
	int n;
	SimpleGameState compressed_game = compress_game(game, &n);
	struct move_t move = expand_move(game, get_rand_move(&compressed_game, n));
	free(compressed_game.graph);
	return move;
}
