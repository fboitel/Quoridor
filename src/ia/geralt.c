#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "ia.h"
#include "move.h"

#define EDGE(graph, i, j) ((graph)[(i) * n2 + (j)])

#define DISPLACEMENT_MOVE(dest) ((SimpleMove) {MOVE, {.displacement = (dest)}})
#define WALL_MOVE(a, b, c, d)   ((SimpleMove) {WALL, {.wall = {(a), (b), (c), (d)}}})

#define QUEUE_ADD(array, capacity, start, size, value) ((array)[((start) + (size)++) % (capacity)] = (value))
#define QUEUE_REMOVE(array, capacity, start, size) ((array)[(start)++ % (capacity)]); --(size)


// define simplified structures to gain efficiency

typedef struct {
	enum movetype_t type;
	union {
		int displacement;
		int wall[4];
	} action;
} SimpleMove;

typedef struct {
	char *graph;
	int pos;
	int num_walls;
	int opponent_pos;
	int opponent_num_walls;
} SimpleGameState;


// data initialized once (doesn't change from one move to another)

char *name = "Geralt";
int n;
int n2;
int n4;
int nb_of_start_pos;
int *start_pos;
int *opponent_start_pos;
bool target_is_up;

void add_displacement_moves(const char *graph, SimpleMove *moves, int *nb_of_moves, int player_pos, int opponent_pos, char main_direction, char secondary_direction) {
	char edge;

	int step_1 = player_pos + main_direction;
	if (step_1 < 0 || step_1 >= n2 || (edge = EDGE(graph, player_pos, step_1)) == 0 || edge > 4) {
		return;
	}

	if (step_1 != opponent_pos) {
		moves[(*nb_of_moves)++] = DISPLACEMENT_MOVE(step_1);
		return;
	}

	int step_2 = step_1 + main_direction;
	if (step_2 >= 0 && step_2 < n2 && (edge = EDGE(graph, step_1, step_2)) >= 1 && edge <= 4) {
		moves[(*nb_of_moves)++] = DISPLACEMENT_MOVE(step_2);
		return;
	}

	step_2 = step_1 + secondary_direction;
	if (step_2 >= 0 && step_2 < n2 && (edge = EDGE(graph, step_1, step_2)) >= 1 && edge <= 4) {
		moves[(*nb_of_moves)++] = DISPLACEMENT_MOVE(step_2);
	}

	step_2 = step_1 - secondary_direction;
	if (step_2 >= 0 && step_2 < n2 && (edge = EDGE(graph, step_1, step_2)) >= 1 && edge <= 4) {
		moves[(*nb_of_moves)++] = DISPLACEMENT_MOVE(step_2);
	}
}

void add_wall_moves(const char *graph, SimpleMove *moves, int *nb_of_moves) {
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
			char e = EDGE(graph, a, b);
			char f = EDGE(graph, c, d);
			char g = EDGE(graph, a, c);
			char h = EDGE(graph, b, d);

			if (e >= 1 && e <= 4 && f >= 1 && f <= 4 && g != 7) {
				moves[(*nb_of_moves)++] = WALL_MOVE(a, b, c, d);
			}

			if (g >= 1 && g <= 4 && h >= 1 && h <= 4 && e != 5) {
				moves[(*nb_of_moves)++] = WALL_MOVE(a, c, b, d);
			}
		}
	}
}

SimpleMove *get_possible_moves(SimpleGameState *game, int *nb_of_moves) {
	SimpleMove *moves;

	if (game->pos == -1) {
		moves = malloc(nb_of_start_pos * sizeof(SimpleMove));
		for (int i = 0; i < nb_of_start_pos; ++i) {
			moves[i] = DISPLACEMENT_MOVE(start_pos[i]);
		}
		*nb_of_moves = nb_of_start_pos;
		return moves;
	}

	moves = malloc((2 * (n - 1) * (n - 1) + 5) * sizeof(SimpleMove));
	*nb_of_moves = 0;

	add_displacement_moves(game->graph, moves, nb_of_moves, game->pos, game->opponent_pos, 1, (char) n);
	add_displacement_moves(game->graph, moves, nb_of_moves, game->pos, game->opponent_pos, -1, (char) n);
	add_displacement_moves(game->graph, moves, nb_of_moves, game->pos, game->opponent_pos, (char) n, 1);
	add_displacement_moves(game->graph, moves, nb_of_moves, game->pos, game->opponent_pos, (char) -n, 1);

	if (game->num_walls > 0) {
		add_wall_moves(game->graph, moves, nb_of_moves);
	}

	moves = realloc(moves, *nb_of_moves * sizeof(SimpleMove));
	return moves;
}

int distance(const char *graph, int pos, bool self) {
	bool current_target_is_up = self == target_is_up;
	int dist;

	bool flags[n2];
	memset(flags, true, n2 * sizeof(bool));
	flags[pos] = false;

	int queue[n2];
	int queue_start = 0;
	int queue_size;

	int iteration_length;
	int next_iteration_length = 0;

	if (pos == -1) {
		dist = 1;
		iteration_length = nb_of_start_pos;
		queue_size = nb_of_start_pos;

		for (int i = 0; i < nb_of_start_pos; ++i) {
			queue[i] = (self ? start_pos : opponent_start_pos)[i];
		}

	} else {
		dist = 0;
		iteration_length = 1;
		queue_size = 1;
		queue[0] = pos;
	}

	while (queue_size > 0) {
		int current_pos = QUEUE_REMOVE(queue, n2, queue_start, queue_size);

		if ((current_target_is_up && current_pos < n) || (!current_target_is_up && current_pos >= n2 - n)) {
			return dist;
		}

		int new_pos;
		char edge;

		// north
		new_pos = current_pos - n;
		if (new_pos >= 0 && flags[new_pos] && (edge = EDGE(graph, current_pos, new_pos)) >= 1 && edge <= 4) {
			flags[new_pos] = false;
			QUEUE_ADD(queue, n2, queue_start, queue_size, new_pos);
			++next_iteration_length;
		}

		// south
		new_pos = current_pos + n;
		if (new_pos < n2 && flags[new_pos] && (edge = EDGE(graph, current_pos, new_pos)) >= 1 && edge <= 4) {
			flags[new_pos] = false;
			QUEUE_ADD(queue, n2, queue_start, queue_size, new_pos);
			++next_iteration_length;
		}

		// west
		new_pos = current_pos - 1;
		if (new_pos >= 0 && flags[new_pos] && (edge = EDGE(graph, current_pos, new_pos)) >= 1 && edge <= 4) {
			flags[new_pos] = false;
			QUEUE_ADD(queue, n2, queue_start, queue_size, new_pos);
			++next_iteration_length;
		}

		// east
		new_pos = current_pos + 1;
		if (new_pos < n2 && flags[new_pos] && (edge = EDGE(graph, current_pos, new_pos)) >= 1 && edge <= 4) {
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

int evaluate(const char *graph, int pos, int opponent_pos) {
	int dist = distance(graph, pos, target_is_up);
	int opponent_dist = distance(graph, opponent_pos, !target_is_up);

	// invalid move (no possible path)
	if (dist == -1 || opponent_dist == -1) {
		return -INT_MAX; // we can't use INT_MIN because it must be invertible
	}

	// win
	if (dist == 0) {
		return INT_MAX - 1;
	}

	// loose
	if (opponent_dist == 0) {
		return -(INT_MAX - 1);
	}

	int score = (int) (n2 * (1.2 * opponent_dist - dist));
	score -= abs(pos%n - n/2);

	return score;
}

void apply_move(SimpleGameState *dest, SimpleGameState *src, SimpleMove move) {
	memcpy(dest->graph, src->graph, n4);
	dest->opponent_pos = src->opponent_pos;
	dest->opponent_num_walls = src->opponent_num_walls;

	switch (move.type) {
		case MOVE:
			dest->pos = move.action.displacement;
			dest->num_walls = src->num_walls;
			break;

		case WALL:
			dest->pos = src->pos;
			dest->num_walls = src->num_walls - 1;

			// get nodes
			int first_node = move.action.wall[0];
			int second_node = move.action.wall[1];

			if (first_node + 1 == second_node) {
				// vertical wall
				EDGE(dest->graph, first_node, second_node) = 5;
				EDGE(dest->graph, second_node, first_node) = 5;

				EDGE(dest->graph, first_node + n, second_node + n) = 6;
				EDGE(dest->graph, second_node + n, first_node + n) = 6;
			} else {
				// horizontal wall
				EDGE(dest->graph, first_node, second_node) = 7;
				EDGE(dest->graph, second_node, first_node) = 7;

				EDGE(dest->graph, first_node + 1, second_node + 1) = 8;
				EDGE(dest->graph, second_node + 1, first_node + 1) = 8;
			}

			break;

		default:
			break;
	}
}

SimpleMove get_best_move(SimpleGameState *game) {
	int nb_of_moves;
	SimpleMove *moves = get_possible_moves(game, &nb_of_moves);

	int best_score = INT_MIN;
	SimpleMove best_move;

	for (int i = 0; i < nb_of_moves; ++i) {
		SimpleMove move = moves[i];

		char new_graph[n4];
		SimpleGameState new_state = { .graph = new_graph };
		apply_move(&new_state, game, move);

		int score = evaluate(new_state.graph, new_state.pos, new_state.opponent_pos);
		if (score > best_score) {
			best_score = score;
			best_move = move;
		}
	}

	free(moves);
	return best_move;
}

SimpleGameState compress_game(struct game_state_t game) {
	SimpleGameState compressed = {
		.graph = malloc(n4),
		.pos = game.self.pos == SIZE_MAX ? -1 : (int) game.self.pos,
		.num_walls = (int) game.self.num_walls,
		.opponent_pos = game.opponent.pos == SIZE_MAX ? -1 : (int) game.opponent.pos,
		.opponent_num_walls = (int) game.opponent.num_walls
	};

	for (int i = 0; i < n2; ++i) {
		for (int j = 0; j < n2; ++j) {
			EDGE(compressed.graph, i, j) = (char) gsl_spmatrix_uint_get(game.graph->t, i, j);
		}
	}

	return compressed;
}

struct move_t expand_move(struct game_state_t game, SimpleMove move) {
	struct move_t expanded = {
			.c = game.self.color,
			.t = move.type,
			.m = move.type == MOVE ? (size_t) move.action.displacement : game.self.pos
	};

	switch (move.type) {
		case WALL:
			expanded.e[0] = (struct edge_t) {move.action.wall[0], move.action.wall[1]};
			expanded.e[1] = (struct edge_t) {move.action.wall[2], move.action.wall[3]};
			break;

		default:
			expanded.e[0] = no_edge();
			expanded.e[1] = no_edge();
			break;
	}

	return expanded;
}

struct move_t make_move(struct game_state_t game) {
	SimpleGameState compressed_game = compress_game(game);
	SimpleMove move = get_best_move(&compressed_game);
	free(compressed_game.graph);
	return expand_move(game, move);
}

void init_meta(struct game_state_t state) {
	n  = (int) sqrtl(state.graph->t->size1);
	n2 = (int) state.graph->t->size1;
	n4 = n2 * n2;

	start_pos = malloc(sizeof(int) * n);
	nb_of_start_pos = 0;

	opponent_start_pos = malloc(sizeof(int) * n);
	int nb_of_opponent_start_pos = 0;

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < n; ++j) {
			int pos = j;
			if (i) pos += n2 - n;

			if (gsl_spmatrix_uint_get(state.graph->o, state.self.color, pos)) {
				start_pos[nb_of_start_pos++] = pos;
			} else if (gsl_spmatrix_uint_get(state.graph->o, state.opponent.color, pos)) {
				opponent_start_pos[nb_of_opponent_start_pos++] = pos;
			}
		}
	}

	if (nb_of_start_pos != nb_of_opponent_start_pos) {
		exit(EXIT_FAILURE);
	}

	if (nb_of_start_pos < n) {
		start_pos = realloc(start_pos, sizeof(int) * nb_of_start_pos);
		opponent_start_pos = realloc(opponent_start_pos, sizeof(int) * nb_of_start_pos);
	}

	target_is_up = start_pos[0] < n;
}

struct move_t make_first_move(struct game_state_t game) {
	init_meta(game);
	return make_move(game);
}

void finalize_ia() {
	free(start_pos);
	free(opponent_start_pos);
}
