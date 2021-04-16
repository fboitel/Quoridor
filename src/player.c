#include "player.h"
#include "client.h"
#include "board.h"
#include "debug.h"

struct player_t player;
extern char* name;
static bool first_move = true;

char const* get_player_name(void) {
    return name;
}

void initialize(enum color_t id, struct graph_t* graph, size_t num_walls) {
    player.id = id;
    player.graph = graph;

    for (size_t i = 0; i < graph->o->size2; i++) {
        if (gsl_spmatrix_uint_get(graph->o, id, i) == 1) {
            player.pos = i;
            break;
        }
    }
    player.num_walls = num_walls;
}

struct move_t play(struct move_t previous_move) {
    struct move_t move;
    // Initial move
    if (first_move) {
        move.c = player.id;
        move.t = MOVE;
        move.m = player.pos;
        first_move = false;
    }
    else {
        move = strat(player.graph, player.pos, previous_move);
        // Decrease the wall counter if the player move is to put one
        player.num_walls -= (move.t == WALL);
    }
    printf("%s move to %zu\n", get_player_name(), move.m);
    return move;
}

void finalize(void) {
    graph_free(player.graph);
}
