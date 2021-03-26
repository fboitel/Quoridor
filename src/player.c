#include "player.h"

struct player_t {
    char* name;
    enum color_t id;
    struct graph_t* graph;
    int pos; // Position
    size_t num_walls; // Numbers of wall to place
};

static struct player_t player;

char const* get_player_name() {
    return player.name;
}

// Copy a graph
struct graph_t* graph_copy(struct graph_t* graph) {
    struct graph_t* copy = malloc(sizeof(*copy));
    copy->num_vertices = graph->num_vertices;
    copy->t = gsl_spmatrix_alloc(graph->t->size1, graph->t->size2);
    copy->o = gsl_spmatrix_alloc(graph->o->size1, graph->o->size2);
    gsl_spmatrix_memcpy(copy->t, graph->t);
    gsl_spmatrix_memcpy(copy->o, graph->o);
}

// Return the index in the graph of a subgraph vertex
int find_pos(int id) {
    return id;
}

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
        gsl_spmatrix_set(player.graph->t, e[i].fr, e[i].to, direction);
        gsl_spmatrix_set(player.graph->t, e[i].fr, e[i].to, opposite(direction));
    }
}

void initialize(enum color_t id, struct graph_t* graph, size_t num_walls) {
    player.id = id;
    player.graph = graph_copy(graph);
    player.num_walls = num_walls;
}

struct move_t play(struct move_t previous_move) {
    if (previous_move.t == WALL)
        update_graph(previous_move.e);

    size_t m;
    struct edge_t e;
    enum movetype_t t;
    enum color_t c = 1 - previous_move.c;

    if (0) {
        t = WALL;
        e = new_e
            m = player.pos;

    }
    else {
        t = MOVE;
        e = no_edge();
        m = new_m
    }

    return (struct move_t) { .e = e, .c = c };
}

void finalize() {
    gsl_spmatrix_free(player.graph->t);
    gsl_spmatrix_free(player.graph->o);
}