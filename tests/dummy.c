// A crash test game for tests

#include "ia.h"
#include "board.h"
#include "move.h"

char* name = "Dummy";

struct move_t strat(struct game_state_t game) {
    return (struct move_t) {
    	.m = no_vertex(),
    	.e = {no_edge(), no_edge()},
    	.t = NO_TYPE,
    	.c = game.self.color
    };
}
