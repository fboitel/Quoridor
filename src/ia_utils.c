#include "ia_utils.h"

struct move_t make_default_first_move(struct game_state_t game) {
	struct move_t move = {
			.e = {no_edge(), no_edge()},
			.t = MOVE,
			.c = game.self.color,
			.m = gsl_spmatrix_uint_get(game.graph->o, game.self.color, 0) ? (size_t)((sqrtl(game.graph->num_vertices)-1)/2) : game.graph->num_vertices - (size_t)(sqrtl(game.graph->num_vertices)/2)
	};
	/*
	int matches = 0;
	for (size_t i = 0; i < game.graph->o->size2; i++) {
		if (!gsl_spmatrix_uint_get(game.graph->o, game.self.color, i)) {
			continue;
		}
		// FIXME: hack so white player doesn't start on the same column as black player
		if (game.self.color == WHITE && matches < 2) {
			++matches;
			continue;
		}
		move.m = i;
		break;
	}
*/
	return move;
}
