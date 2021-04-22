#include "player.h"
#include "ia.h"
#include "board.h"

struct game_state_t game;
extern char *name;

char const *get_player_name(void) {
	return name;
}

void initialize(enum color_t id, struct graph_t *graph, size_t num_walls) {
	game.graph = graph;

	game.self = (struct player_state_t) {
			.color = id,
			.pos = SIZE_MAX, // default value (no position for now)
			.num_walls = num_walls
	};

	game.opponent = (struct player_state_t) {
			.color = id == BLACK ? WHITE : BLACK,
			.pos = SIZE_MAX, // default value (no position for now)
			.num_walls = num_walls
	};
}

// TODO should be implemented by client
struct move_t make_first_move() {
	struct move_t move;
	move.c = game.self.color;
	move.t = MOVE;
	move.e[0] = no_edge();
	move.e[1] = no_edge();

	int matches = 0;
	for (size_t i = 0; i < game.graph->o->size2; i++) {
		if (!gsl_spmatrix_uint_get(game.graph->o, game.self.color, i)) {
			continue;
		}
		// FIXME: hack so white game doesn't start on the same column as black game
		if (game.self.color == WHITE && matches < 2) {
			++matches;
			continue;
		}
		move.m = i;
		break;
	}

	return move;
}

void update_graph(struct move_t move) {
	struct player_state_t *player = move.c == game.self.color ? &game.self : &game.opponent;

	switch (move.t) {
		case MOVE:
			player->pos = move.m;
			break;

		case WALL:
			add_edges(game.graph, move.e);
			--player->num_walls;
			break;

		case NO_TYPE:
			break;
	}
}

void print_move(struct move_t move) {
	printf("\n\nMove from %u (%s):\n", move.c, get_player_name());
	printf("vertex: %zu\n", move.m);
	printf("walls: ");

	if (is_no_edge(move.e[0])) {
		printf("(no edge)");
	} else {
		printf("(%zu, %zu)", move.e[0].fr, move.e[0].to);
	}

	printf(" ");

	if (is_no_edge(move.e[1])) {
		printf("(no edge)");
	} else {
		printf("(%zu, %zu)", move.e[1].fr, move.e[1].to);
	}

	printf("\n");
	printf("move type: %s\n\n\n", move.t == 0 ? "WALL" : "MOVE");
}

struct move_t play(struct move_t previous_move) {
	update_graph(previous_move);

	static bool first_move = true;
	struct move_t move;
	if (first_move) {
		move = make_first_move();
		first_move = false;
	} else {
		move = strat(game);
	}
	// print_move(move);

	update_graph(move);

	return move;
}

void finalize(void) {
	graph_free(game.graph);
}
