#include "client.h"
#include "board.h"
#include "move.h"

char* name = "Tom";

// Do a move in random direction
size_t random_move(struct graph_t* graph, size_t v, struct move_t last_move) {
    size_t linked[MAX_DIRECTION];
    get_linked(graph, v, linked);

    enum direction_t d;
    do {
        // Exclude NO_DIRECTION
        d = (rand() % MAX_DIRECTION - 1) + 1;
    } while(is_no_vertex(linked[d]) && linked[d] != last_move.m);
    return linked[d];
}

// Get a random wall that can be put in the graph
// May not work for other than SQUARE board
void random_wall(struct graph_t* graph, struct edge_t wall[]) {
    wall[0] = no_edge();
    wall[1] = no_edge();
    (void) graph;
    /*
    do {
        size_t linked[MAX_DIRECTION];
        size_t v = rand() % graph->num_vertices;
        get_linked(graph, v, linked);

        // Check if we can put a whole wall
        if (!is_no_vertex(linked[WEST]) && !is_no_vertex(linked[EAST])) {
            wall[0] = (struct edge_t){ v, linked[WEST] };
            wall[1] = (struct edge_t){ v, linked[EAST] };
        }
        else if (!is_no_vertex(linked[NORTH]) && !is_no_vertex(linked[SOUTH])) {
            wall[0] = (struct edge_t){ v, linked[NORTH] };
            wall[1] = (struct edge_t){ v, linked[SOUTH] };
        }
    } while (is_no_wall(wall));
    */
}

// Tom's strategy is to do a random move
struct move_t strat(struct graph_t* graph, size_t v, struct move_t last_move) {
    struct move_t move;

    if (rand() % 2) {
        move.t = WALL;
        random_wall(graph, move.e);
    }
    else {
        move.t = MOVE;
        move.m = random_move(graph, v, last_move);
    }
    return move;
}