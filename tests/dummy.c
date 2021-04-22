// A crash test player for tests

#include "client.h"
#include "board.h"
#include "move.h"

char* name = "Dummy";

struct move_t strat(struct graph_t* graph, size_t v, struct move_t last_move) {
    (void) graph;
    (void) v;
    return last_move;
}
