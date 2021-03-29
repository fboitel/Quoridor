#include "player.h"
#include "client.h"
#include "board.h"
#include "debug.h"

struct player_t player;

//// Move function ////

// Do a move
size_t move(void) {
    size_t reachables[MAX_DIRECTION];
    size_t nb_move = get_reachables(player.graph, player.pos, reachables);
    if (nb_move == 0)
        return NO_DIRECTION;

    size_t v;
    do {
        v = rand() % MAX_DIRECTION;
    } while (!reachables[v]);

    return v;
}

//// Game functions ////

char const* get_player_name(void) {
    return player.name;
}

void initialize(enum color_t id, struct graph_t* graph, size_t num_walls) {
    player.id = id;
    player.graph = graph;
    size_t n = player.graph->t->size1;
    player.pos = n / 2 + (player.id == BLACK ? 0 : n * (n - 1));
    player.num_walls = num_walls;
}

struct move_t play(struct move_t previous_move) {
    size_t m;
    struct wall_t wall;
    enum movetype_t t;
    enum color_t c = player.id;

    if (player.num_walls == 0) {
        t = WALL;
        m = player.pos;
        wall = random_wall(player.graph);
        player.num_walls--;
    }
    else {
        t = MOVE;
        m = move();
        wall = no_wall();
    }

    return (struct move_t) { m, { wall.e1, wall.e2 }, t, c };
}

void finalize(void) {
    // graph_free(player.graph);
}

// Debug
void print_edge(struct edge_t e) {
    if (e.fr == SIZE_MAX)
        printf("No edge\n");
    else
        printf("%zu --> %zu\n", e.fr, e.to);
}

void print_move(struct move_t m) {
    printf("Move debug\n");
    printf("Player: %d\n", m.c);
    printf("Type: %s\n", m.t == MOVE ? "move" : "wall");
    printf("Position %zu\n", m.m);
    printf("Edge:\n");
    print_edge(m.e[0]);
    print_edge(m.e[1]);
}

void print_player() {
    printf("Player: %d\n", player.id);
    printf("Position %zu\n", player.pos);
}
