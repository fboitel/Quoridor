#include "player.h"
#include "client.h"
#include "board.h"
#include "debug.h"

struct player_t player;
extern char* name;

char const* get_player_name(void) {
    return name;
}

void initialize(enum color_t id, struct graph_t* graph, size_t num_walls) {
    player.id = id;
    player.graph = graph;
    size_t n = player.graph->t->size1;
    player.pos = n / 2 + (player.id == BLACK ? 0 : n * (n - 1));
    player.num_walls = num_walls;
}

struct move_t play(struct move_t previous_move) {
    struct move_t move;
    // Initial move
    if (previous_move.t == NO_TYPE) {
        move.c = player.id;
        move.t = MOVE;
        move.m = player.pos;
    } else {
        move = strat(player.graph, player.pos, previous_move);
        // Decrease the wall counter if the player move is to put one
        player.num_walls -= (move.t == WALL);
    }
    return move;
}

void finalize(void) {
    // graph_free(player.graph);
}
