#include "player.h"

struct player_t {
    char* name;
    enum color_t id;
    struct graph_t* graph;
    size_t num_walls;
};

struct player_t player;

char const* get_player_name() {
    return player.name;
}

void initialize(enum color_t id, struct graph_t* graph, size_t num_walls) {
    player.id = id;
    player.graph = graph;
    player.num_walls = num_walls;
}

// Return the index in the graph of a subgraph vertex
int find_pos(int id);

// Return the relative position between two vertices
int find_direction(struct edge_t e) {
    int fr_pos = find_pos(e.fr);
    int to_pos = find_pos(e.to);
    int m = player.graph->t->size1;
    int fr_i = fr_pos / m;
    int fr_j = fr_pos % m;
    int to_i = to_pos / m;
    int to_j = to_pos % m;

    if (fr_i > to_i)
        return 1;
    else if (fr_i < to_i)
        return 2;
    else if (fr_j > to_j)
        return 3;
    else if (fr_j < to_j)
        return 4;
    else
        return -1;
}

// Add edges in the graph
void update_graph(struct edge_t e[]) {
    int direction = find_direction(e[0]);

    for (int i = 0; i < 2; i++) {
        gsl_matrix_set(player.graph->t, e[i].fr, e[i].to, direction);
        gsl_matrix_set(player.graph->t, e[i].fr, e[i].to, opposite(direction));
    }
}

struct move_t play(struct move_t previous_move) {
    if (previous_move.t == WALL)
        update_graph(previous_move.e);

    struct edge_t e;
    enum movetype_t t;
    enum color_t c = player.id;

    if (0) {
        t = WALL;

    }
    else {
        t = MOVE;
        e = no_edge();
    }

    return (struct move_t) { .e = e, .c = c };
}

void finalize() {

}