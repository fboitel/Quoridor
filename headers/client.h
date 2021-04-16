#ifndef _QUOR_CLIENT_H_
#define _QUOR_CLIENT_H_

#include "graph.h"
#include "move.h"

struct player_t {
    char* name;
    enum color_t id;
    struct graph_t* graph;
    size_t pos;          // Position
    size_t num_walls;    // Numbers of wall to place
};

// Return a move based on the client strategy
struct move_t strat(struct graph_t* graph, size_t v, struct move_t last_move);

#endif // _QUOR_CLIENT_H_