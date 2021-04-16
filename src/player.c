#include "player.h"
#include "client.h"
#include "board.h"

struct player_t player;
extern char* name;

char const* get_player_name(void) {
	return name;
}

void initialize(enum color_t id, struct graph_t* graph, size_t num_walls) {
	player.id = id;
	player.graph = graph;
	player.pos = SIZE_MAX; // default value for position
	player.num_walls = num_walls;
}

// TODO should be implemented by client
struct move_t make_first_move() {
	struct move_t move;
	move.c = player.id;
	move.t = MOVE;

	int matches = 0;
	for (size_t i = 0; i < player.graph->o->size2; i++) {
		if (!gsl_spmatrix_uint_get(player.graph->o, player.id, i)) {
			continue;
		}
		// FIXME: hack so white player doesn't start on the same column as black player
		if (player.id == WHITE && matches < 2) {
			++matches;
			continue;
		}
		move.m = i;
		break;
	}

	return move;
}

void update_graph(struct move_t move, bool own_turn) {
	switch (move.t) {
		case MOVE:
			if (own_turn) {
				player.pos = move.m;
			} else {
				player.opponent_pos = move.m;
			}
			break;

		case WALL:
			add_edges(player.graph, move.e);
			if (own_turn) {
				--player.num_walls;
			}
			break;

		case NO_TYPE:
			break;
	}
}

struct move_t play(struct move_t previous_move) {
	update_graph(previous_move, false);

	static bool first_move = true;
	struct move_t move;
	if (first_move) {
		move = make_first_move();
		first_move = false;
	} else {
		move = strat(player.graph, player.pos, previous_move);
	}

	update_graph(previous_move, true);

	printf("%s move to %zu\n", name, move.m);
	return move;
}

void finalize(void) {
	graph_free(player.graph);
}
