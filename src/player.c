#include "player.h"

struct player_t {
    char* name;
    enum color_t id;
    struct graph_t* graph;
    int pos; // Position
    size_t num_walls; // Numbers of wall to place
};

static struct player_t player;

enum direction_t { HORIZONTAL, VERTICAL, NO = -1};

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
void get_adjacents(int arr[4], int i) {
    int n = player.graph->t->size1;

    arr[0] = i >= n ? i - n : -1;
    arr[1] = i <= n * (n - 1) ? i + n : -1;
    arr[2] = i % n != 0 ? i - 1 : -1;
    arr[3] = i % n != n - 1 ? i + 1 : -1;
}

// Fill an array with the reachable vertices (-1 if vertices is not reachable)
int find_available_move(int arr[4], int i) {
    int counter = 0;
    int adj[4];
    get_adjacents(adj, player.pos);

    for (int k = 0; k < 4; k++) {
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
    int available_moves[4];
    int nb_move = find_available_move(available_moves, player.pos);
    if (nb_move == 0)
        return -1;

    int m;
    do {
        m = available_moves[rand() % 4];
    } while (m != -1);
    return m;
}

//// Edge functions ////

/*
// Return the relative position between two vertices
int find_direction(struct edge_t e) {
    struct coords fr = get_coords(e.fr);
    struct coords to = get_coords(e.to);

    if (fr.i > to.i)
        return 1;
    else if (fr.i < to.i)
        return 2;
    else if (fr.j > to.j)
        return 3;
    else if (fr.j < to.j)
        return 4;
    else
        return -1;
}
*/

// Add edges in the graph
void no_wall(struct edge_t e[]) {
    e[0] = no_edge();
    e[1] = no_edge();
}

void put_wall(struct edge_t e[]) {
    int n = player.graph->t->size1;
    int direction = NO;
    do {
    int v = rand() % (n * n);
    int adj[4];
    get_adjacents(adj, v);
    if (adj[0] != -1 && adj[1] != -1)
        direction = VERTICAL;
    if (adj[2] != -1 && adj[3] != -1)
        direction = HORIZONTAL;
    } while(direction == -1);

    // PUT THE WALL

    player.num_walls--;
}

int opposite(int d) {
    switch (d)
    {
    case 1:
        return 2;
    case 2:
        return 1;
    case 3:
        return 4;
    case 4:
        return 3;
    default:
        return -1;
    }
}

void update_graph(struct edge_t e[]) {
    int direction = 0; // TODO //find_direction(e[0]);

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

    return (struct move_t) { m, e, t, c };
}

void finalize() {
    gsl_spmatrix_free(player.graph->t);
    gsl_spmatrix_free(player.graph->o);
}