#ifndef _QUOR_CLIENT_H_
#define _QUOR_CLIENT_H_

#include "graph.h"
#include "move.h"

struct player_t {
	enum color_t id;
	enum color_t opponent_id;
	struct graph_t *graph;
	size_t pos;          // Position
	size_t opponent_pos;
	size_t num_walls;    // Numbers of wall available to place
};

// Return a move based on the client strategy
struct move_t strat(struct player_t player);

#endif // _QUOR_CLIENT_H_
