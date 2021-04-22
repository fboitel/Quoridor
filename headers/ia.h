#ifndef _QUOR_IA_H_
#define _QUOR_IA_H_

#include "graph.h"
#include "move.h"

struct player_state_t {
	enum color_t color;
	size_t pos;
	size_t num_walls;
};

struct game_state_t {
	struct graph_t *graph;
	struct player_state_t self;
	struct player_state_t opponent;
};

// Return a first move based on the IA strategy
struct move_t make_first_move(struct game_state_t game);

// Return a move based on the IA strategy
struct move_t make_move(struct game_state_t game);

#endif // _QUOR_IA_H_
