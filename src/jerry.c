#include "client.h"
#include "board.h"
#include "move.h"

char* name = "Jerry";

// Move to the closest vertex to the finish
size_t move_forward(struct graph_t* graph, size_t v, struct move_t last_move) {
    size_t linked[MAX_DIRECTION];
    get_linked(graph, v, linked);

    // TODO get the closest to the finish
    return
        !is_no_vertex(linked[NORTH]) && linked[NORTH] != last_move.m ? linked[NORTH] :
        !is_no_vertex(linked[WEST]) && linked[WEST] != last_move.m ? linked[WEST] :
        !is_no_vertex(linked[EAST]) && linked[EAST] != last_move.m ? linked[EAST] :
        !is_no_vertex(linked[SOUTH]) && linked[SOUTH] != last_move.m ? linked[SOUTH] :
        no_vertex();
}

// Jerry's strategy is to run directly to the arrival without placing any walls
struct move_t strat(struct graph_t* graph, size_t v, struct move_t last_move) {
    struct move_t move;

    move.t = MOVE;
    move.m = move_forward(graph, v, last_move);

    return move;
}