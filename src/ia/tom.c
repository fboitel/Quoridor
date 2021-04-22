#include "client.h"
#include "board.h"
#include "move.h"

char *name = "Tom";

// Do a move in random direction
size_t random_move(struct player_t player) {
	size_t linked[MAX_DIRECTION];
	get_linked(player.graph, player.pos, linked);

	enum direction_t d;
	do {
		// Exclude NO_DIRECTION
		d = (rand() % MAX_DIRECTION - 1) + 1;
	} while (is_no_vertex(linked[d]) && linked[d] != player.opponent_pos);

	return linked[d];
}

// Get a random wall that can be put in the graph
// May not work for other than SQUARE board
void random_wall(struct graph_t *graph, struct edge_t wall[]) {
	size_t v1;
	size_t v2;
	size_t v3;
	size_t v4;
	do {
		v1 = rand() % graph->num_vertices;
		enum orientation_t ORIENTATION = rand() % 2;

		if (ORIENTATION == HORIZONTAL) {
			v2 = vertex_from_direction(graph, v1, SOUTH);
			v3 = vertex_from_direction(graph, v1, EAST);
			v4 = vertex_from_direction(graph, v3, SOUTH);
		} else {
			v2 = vertex_from_direction(graph, v1, EAST);
			v3 = vertex_from_direction(graph, v1, SOUTH);
			v4 = vertex_from_direction(graph, v3, EAST);
		}

		if (is_no_vertex(v2) || is_no_vertex(v3) || is_no_vertex(v4)) {
			wall[0] = no_edge();
			wall[1] = no_edge();
		} else {
			wall[0] = (struct edge_t) {v1, v2};
			wall[1] = (struct edge_t) {v3, v4};
		}
	} while (is_no_edge(wall[0]) || is_no_edge(wall[1]));
}

// Tom's strategy is to do a random move
struct move_t strat(struct player_t player) {
	struct move_t move;
	move.c = player.id;

	if (rand() % 2) {
		move.t = WALL;
		random_wall(player.graph, move.e);
	} else {
		move.t = MOVE;
		move.m = random_move(player);
		move.e[0] = no_edge();
		move.e[1] = no_edge();
	}

	return move;
}
