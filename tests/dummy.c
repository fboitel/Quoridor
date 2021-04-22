// A crash test player for tests

#include "client.h"
#include "board.h"
#include "move.h"

char* name = "Dummy";

struct move_t strat(struct player_t player) {
    return (struct move_t) {
    	.m = no_vertex(),
    	.e = {no_edge(), no_edge()},
    	.t = NO_TYPE,
    	.c = player.id
    };
}
