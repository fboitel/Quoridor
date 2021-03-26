#include "player.h"

struct player_t {
    char* name;
    enum color_t id;
    struct graph_t* graph;
    int pos; // Position
    size_t num_walls; // Numbers of wall to place
};

static struct player_t player;

enum direction_t { HORIZONTAL, VERTICAL, NO = -1 };
enum orientation_t { NORTH = 1, SOUTH = 2, WEST = 3, EAST = 4, MAX_DIRECTION = 5, ERROR_DIRECTION = -1};

// Copy a graph
struct graph_t* graph_copy(struct graph_t* graph) {
    struct graph_t* copy = malloc(sizeof(*copy));
    copy->num_vertices = graph->num_vertices;
    copy->t = gsl_spmatrix_alloc(graph->t->size1, graph->t->size2);
    copy->o = gsl_spmatrix_alloc(graph->o->size1, graph->o->size2);
    gsl_spmatrix_memcpy(copy->t, graph->t);
    gsl_spmatrix_memcpy(copy->o, graph->o);

    return copy;
}

//// Move function ////

// Return adjacent vertices of the given vertex on a SQUARE square
void get_adjacents(int arr[], int i) {
    int n = player.graph->t->size1;

    arr[0] = i >= n ? i - n : -1;
    arr[1] = i <= n * (n - 1) ? i + n : -1;
    arr[2] = i % n != 0 ? i - 1 : -1;
    arr[3] = i % n != n - 1 ? i + 1 : -1;
}

// Fill an array with the reachable vertices (-1 if vertices is not reachable)
int find_available_move(int arr[], int i) {
    int counter = 0;
    int adj[MAX_DIRECTION - 1];
    get_adjacents(adj, player.pos);

    for (int k = 0; k < MAX_DIRECTION - 1; k++) {
        if (adj[k] != -1) {
            int m = gsl_spmatrix_get(player.graph->t, i, adj[k]);
            arr[k] = m != 0 ? adj[k] : -1;
            counter++;
        }
    }
    return counter;
}

// Do a move
size_t move() {
    int available_moves[MAX_DIRECTION - 1];
    int nb_move = find_available_move(available_moves, player.pos);
    if (nb_move == 0)
        return -1;

    int m;
    do {
        m = available_moves[rand() % (MAX_DIRECTION - 1)];
    } while (m != -1);
    return m;
}

//// Edge functions ////

// Return the direction of an edge
int find_direction(struct edge_t e) {
    int n = player.graph->t->size1;
    int d = e.to - e.fr;

    return
        d == -1 ? WEST  :
        d == 1  ? EAST  :
        d == -n ? NORTH :
        d == n  ? SOUTH :
        ERROR_DIRECTION;
}


// Add edges in the graph
void no_wall(struct edge_t e[]) {
    e[0] = no_edge();
    e[1] = no_edge();
}

void put_wall(struct edge_t e[]) {
    int n = player.graph->t->size1;
    int direction = NO;
    int v = -1;
    do {
        v = rand() % (n * n);
        int availables[MAX_DIRECTION - 1];
        find_available_move(availables, v);

        if (availables[0] != -1 && availables[1] != -1)
            direction = VERTICAL;
        if (availables[2] != -1 && availables[3] != -1)
            direction = HORIZONTAL;
    } while (direction == -1);

    if (direction == VERTICAL) {
        gsl_spmatrix_set(player.graph->t, v, v - 1, EAST);
        gsl_spmatrix_set(player.graph->t, v, v + 1, WEST);
    }
    else {
        gsl_spmatrix_set(player.graph->t, v, v - n, NORTH);
        gsl_spmatrix_set(player.graph->t, v, v + n, SOUTH);
    }

    player.num_walls--;
}

int opposite(int d) {
    switch (d)
    {
    case NORTH:
        return SOUTH;
    case SOUTH:
        return NORTH;
    case WEST:
        return EAST;
    case EAST:
        return WEST;
    default:
        return ERROR_DIRECTION;
    }
}

void update_graph(struct edge_t e[]) {
    int direction = find_direction(e[0]);

    for (int k = 0; k < 2; k++) {
        gsl_spmatrix_set(player.graph->t, e[k].fr, e[k].to, direction);
        gsl_spmatrix_set(player.graph->t, e[k].to, e[k].fr, opposite(direction));
    }
}


//// Game functions ////

char const* get_player_name() {
    return player.name;
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
    struct edge_t e[2];
    enum movetype_t t;
    enum color_t c = 1 - previous_move.c;

    if (player.num_walls > 0) {
        t = WALL;
        put_wall(e);
        m = player.pos;
    }
    else {
        t = MOVE;
        no_wall(e);
        m = move();
    }

    return (struct move_t) { m, { e[0], e[1] }, t, c };
}

void finalize() {
    gsl_spmatrix_free(player.graph->t);
    gsl_spmatrix_free(player.graph->o);
}