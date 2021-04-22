#include "client.h"
#include "board.h"
#include "move.h"

char *name = "Jerry";

// Move to the closest vertex to the finish
size_t move_forward(struct graph_t *graph, size_t v, struct move_t last_move) {
	size_t linked[MAX_DIRECTION];
	size_t num = get_linked(graph, v, linked);

	if (num == 0) {
		fprintf(stderr, "ERROR: Player is blocked\n");
	}

	// TODO get the closest to the finish
	enum direction_t GOAL = 1 - last_move.c == BLACK ? SOUTH : NORTH;

	enum direction_t directions[] = {GOAL, WEST, EAST, linked[opposite(GOAL)]};

	for (int i = 0; i < 4; ++i) {
		if (!is_no_vertex(linked[directions[i]]) && linked[directions[i]] != last_move.m) {
			return linked[directions[i]];
		}
	}

	return no_vertex();
}

// Jerry's strategy is to run directly to the arrival without placing any walls
struct move_t strat(struct graph_t *graph, size_t v, struct move_t last_move) {
	struct move_t move;
	move.c = 1 - last_move.c;
	move.t = MOVE;
	move.m = move_forward(graph, v, last_move);
	move.e[0] = no_edge();
	move.e[1] = no_edge();

	return move;
}
