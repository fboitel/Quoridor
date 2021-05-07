#include <ia_utils.h>
#include <stdbool.h>
#include <string.h>
#include "ia.h"
#include "move.h"

#define EDGE(graph, n2, i, j) ((graph)[(i) * (n2) + (j)])
#define DISPLACEMENT_MOVE(dest) ((SimpleMove) {MOVE, {(dest), 0, 0, 0}})
#define QUEUE_ADD(array, capacity, start, size, value) ((array)[((start) + (size)++) % (capacity)] = (value))
#define QUEUE_REMOVE(array, capacity, start, size) ((array)[(start)++ % (capacity)]); --(size)

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
	int n2 = n * n;
	char edge;

	int step_1 = player_pos + main_direction;
	if (step_1 < 0 || step_1 >= n2 || (edge = EDGE(graph, n2, player_pos, step_1)) == 0 || edge > 4) {
		return;
	}

	if (step_1 != opponent_pos) {
		moves[(*nb_of_moves)++] = DISPLACEMENT_MOVE(step_1);
		return;
	}

	int step_2 = step_1 + main_direction;
	if (step_2 >= 0 && step_2 < n2 && (edge = EDGE(graph, n2, step_1, step_2)) >= 1 && edge <= 4) {
		moves[(*nb_of_moves)++] = DISPLACEMENT_MOVE(step_2);
		return;
	}

	step_2 = step_1 + secondary_direction;
	if (step_2 >= 0 && step_2 < n2 && (edge = EDGE(graph, n2, step_1, step_2)) >= 1 && edge <= 4) {
		moves[(*nb_of_moves)++] = DISPLACEMENT_MOVE(step_2);
	}

	step_2 = step_1 - secondary_direction;
	if (step_2 >= 0 && step_2 < n2 && (edge = EDGE(graph, n2, step_1, step_2)) >= 1 && edge <= 4) {
		moves[(*nb_of_moves)++] = DISPLACEMENT_MOVE(step_2);
	}
}

void add_wall_moves(const char *graph, int n, SimpleMove *moves, int *nb_of_moves) {
	int n2 = n * n;

	for (int i = 0; i < n - 1; ++i) {
		for (int j = 0; j < n - 1; ++j) {
			/*
			 * a -e- b
			 * |     |
			 * g     h
			 * |     |
			 * c -f- d
			 */
			int a = i * n + j;
			int b = a + 1;
			int c = a + n;
			int d = b + n;
			char e = EDGE(graph, n2, a, b);
			char f = EDGE(graph, n2, c, d);
			char g = EDGE(graph, n2, a, c);
			char h = EDGE(graph, n2, b, d);

			if (e >= 1 && e <= 4 && f >= 1 && f <= 4 && g != 7) {
				moves[(*nb_of_moves)++] = (SimpleMove) {WALL, {a, b, c, d}};
			}

			if (g >= 1 && g <= 4 && h >= 1 && h <= 4 && e != 5) {
				moves[(*nb_of_moves)++] = (SimpleMove) {WALL, {a, c, b, d}};
			}
		}
	}
}

SimpleMove *get_possible_moves(SimpleGameState *game, int n, int *nb_of_moves) {
	SimpleMove *moves = malloc((2 * (n - 1) * (n - 1) + 5) * sizeof(SimpleMove));
	*nb_of_moves = 0;

	add_displacement_moves(game->graph, n, moves, nb_of_moves, game->pos, game->opponent_pos, 1, (char) n);
	add_displacement_moves(game->graph, n, moves, nb_of_moves, game->pos, game->opponent_pos, -1, (char) n);
	add_displacement_moves(game->graph, n, moves, nb_of_moves, game->pos, game->opponent_pos, (char) n, 1);
	add_displacement_moves(game->graph, n, moves, nb_of_moves, game->pos, game->opponent_pos, (char) -n, 1);

	if (game->num_walls > 0) {
		add_wall_moves(game->graph, n, moves, nb_of_moves);
	}

	moves = realloc(moves, *nb_of_moves * sizeof(SimpleMove));
	return moves;
}

int distance(const char *graph, int n, int pos, bool target_is_up) {
	int n2 = n * n;

	int dist = 0;

	bool flags[n2];
	memset(flags, true, n2 * sizeof(bool));
	flags[pos] = false;

	int queue[n2];
	int queue_start = 0;
	int queue_size = 0;

	int iteration_length = 1;
	int next_iteration_length = 0;

	QUEUE_ADD(queue, n2, queue_start, queue_size, pos);

	while (queue_size > 0) {
		int current_pos = QUEUE_REMOVE(queue, n2, queue_start, queue_size);

		if ((target_is_up && current_pos < n) || (!target_is_up && current_pos >= n2 - n)) {
			return dist;
		}

		int new_pos;
		char edge;

		// north
		new_pos = current_pos - n;
		if (new_pos >= 0 && flags[new_pos] && (edge = EDGE(graph, n2, current_pos, new_pos)) >= 1 && edge <= 4) {
			flags[new_pos] = false;
			QUEUE_ADD(queue, n2, queue_start, queue_size, new_pos);
			++next_iteration_length;
		}

		// south
		new_pos = current_pos + n;
		if (new_pos < n2 && flags[new_pos] && (edge = EDGE(graph, n2, current_pos, new_pos)) >= 1 && edge <= 4) {
			flags[new_pos] = false;
			QUEUE_ADD(queue, n2, queue_start, queue_size, new_pos);
			++next_iteration_length;
		}

		// west
		new_pos = current_pos - 1;
		if (new_pos >= 0 && flags[new_pos] && (edge = EDGE(graph, n2, current_pos, new_pos)) >= 1 && edge <= 4) {
			flags[new_pos] = false;
			QUEUE_ADD(queue, n2, queue_start, queue_size, new_pos);
			++next_iteration_length;
		}

		// east
		new_pos = current_pos + 1;
		if (new_pos < n2 && flags[new_pos] && (edge = EDGE(graph, n2, current_pos, new_pos)) >= 1 && edge <= 4) {
			flags[new_pos] = false;
			QUEUE_ADD(queue, n2, queue_start, queue_size, new_pos);
			++next_iteration_length;
		}

		--iteration_length;

		if (iteration_length == 0) {
			iteration_length = next_iteration_length;
			next_iteration_length = 0;
			++dist;
		}
	}

	return -1;
}

int evaluate(const char *graph, int n, int pos, int opponent_pos, bool target_is_up) {
	int dist = distance(graph, n, pos, target_is_up);
	int opponent_dist = distance(graph, n, opponent_pos, !target_is_up);

	// invalid move (no possible path)
	if (dist == -1 || opponent_dist == -1) {
		return -INT_MAX; // we can't use INT_MIN because it must be invertible
	}

	return 2 * opponent_dist - dist;
}

void apply_move(SimpleGameState *dest, SimpleGameState *src, int n, SimpleMove move) {
	int n2 = n * n;

	memcpy(dest->graph, src->graph, n2 * n2);
	dest->opponent_pos = src->opponent_pos;
	dest->opponent_num_walls = src->opponent_num_walls;

	switch (move.type) {
		case MOVE:
			dest->pos = move.action[0];
			dest->num_walls = src->num_walls;
			break;

		case WALL:
			dest->pos = src->pos;
			dest->num_walls = src->num_walls - 1;

			// get nodes
			int first_node = move.action[0];
			int second_node = move.action[1];

			if (first_node + 1 == second_node) {
				// vertical wall
				EDGE(dest->graph, n2, first_node, second_node) = 5;
				EDGE(dest->graph, n2, second_node, first_node) = 5;

				EDGE(dest->graph, n2, first_node + n, second_node + n) = 6;
				EDGE(dest->graph, n2, second_node + n, first_node + n) = 6;
			} else {
				// horizontal wall
				EDGE(dest->graph, n2, first_node, second_node) = 7;
				EDGE(dest->graph, n2, second_node, first_node) = 7;

				EDGE(dest->graph, n2, first_node + 1, second_node + 1) = 8;
				EDGE(dest->graph, n2, second_node + 1, first_node + 1) = 8;
			}

			break;

		default:
			break;
	}
}

SimpleMove get_best_move(SimpleGameState *game, int n, bool target_is_up) {
	int n2 = n * n;

	int nb_of_moves;
	SimpleMove *moves = get_possible_moves(game, n, &nb_of_moves);

	int best_score = INT_MIN;
	SimpleMove best_move;

	for (int i = 0; i < nb_of_moves; ++i) {
		SimpleMove move = moves[i];

		char new_graph[n2 * n2];
		SimpleGameState new_state = { .graph = new_graph };
		apply_move(&new_state, game, n, move);

		int score = evaluate(new_state.graph, n, new_state.pos, new_state.opponent_pos, target_is_up);
		if (score > best_score) {
			best_score = score;
			best_move = move;
		}
	}

	free(moves);
	return best_move;
}

SimpleGameState compress_game(struct game_state_t game, int *n) {
	SimpleGameState compressed = {
		.pos = (int) game.self.pos,
		.num_walls = (int) game.self.num_walls,
		.opponent_pos = (int) game.opponent.pos,
		.opponent_num_walls = (int) game.opponent.num_walls
	};

	int n2 = (int) game.graph->t->size1;
	*n = (int) sqrt(n2);
	compressed.graph = malloc(n2 * n2);

	for (int i = 0; i < n2; ++i) {
		for (int j = 0; j < n2; ++j) {
			EDGE(compressed.graph, n2, i, j) = (char) gsl_spmatrix_uint_get(game.graph->t, i, j);
		}
	}

	return compressed;
}

struct move_t expand_move(struct game_state_t game, SimpleMove move) {
	struct move_t expanded = {
			.c = game.self.color,
			.t = move.type,
			.m = move.type == MOVE ? (size_t) move.action[0] : game.self.pos
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
	bool target_is_up = !gsl_spmatrix_uint_get(game.graph->o, game.self.color, 0);
	int n;
	SimpleGameState compressed_game = compress_game(game, &n);
	struct move_t move = expand_move(game, get_best_move(&compressed_game, n, target_is_up));
	free(compressed_game.graph);
	return move;
}
