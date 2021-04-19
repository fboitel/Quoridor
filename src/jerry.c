#include "client.h"
#include "board.h"
#include "move.h"

char* name = "Jerry";

// Move to the closest vertex to the finish
size_t move_forward(struct graph_t* graph, size_t v, struct move_t last_move) {
	size_t linked[MAX_DIRECTION];
	size_t num = get_linked(graph, v, linked);

	if (num == 0)
		fprintf(stderr, "ERROR: Player is blocked\n");

	// TODO get the closest to the finish
	enum direction_t GOAL = 1 - last_move.c == BLACK ? SOUTH : NORTH;

	return
		!is_no_vertex(linked[GOAL]) && linked[GOAL] != last_move.m ? linked[GOAL] :
		!is_no_vertex(linked[WEST]) && linked[WEST] != last_move.m ? linked[WEST] :
		!is_no_vertex(linked[EAST]) && linked[EAST] != last_move.m ? linked[EAST] :
		!is_no_vertex(linked[opposite(GOAL)]) && linked[opposite(GOAL)] != last_move.m ? linked[opposite(GOAL)] :
		no_vertex();
}

// Jerry's strategy is to run directly to the arrival without placing any walls
struct move_t strat(struct graph_t* graph, size_t v, struct move_t last_move) {
	struct move_t move;
	move.c = 1 - last_move.c;
	move.t = MOVE;
	move.m = move_forward(graph, v, last_move);
	move.e[0] = no_edge();
	move.e[1] = no_edge();

	return move;
}
