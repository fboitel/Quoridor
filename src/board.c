#include "board.h"

enum orientation_t { HORIZONTAL, VERTICAL, NO = -1 };

//// Graph structure manipulation functions

// Initialize a square graph of size m
struct graph_t* square_init(size_t m) {
    struct graph_t* graph = malloc(sizeof(*graph));

    size_t n = m * m;
    graph->num_vertices = n;
    graph->t = gsl_spmatrix_alloc(n, n);
    graph->o = gsl_spmatrix_alloc(2, n);

    return graph;
}

struct graph_t* graph_init(size_t m, enum shape_t shape) {
    switch (shape)
    {
    case SQUARE:
        return square_init(m);
    default:
        return square_init(m);
    }
}

void graph_free(struct graph_t* graph) {
    gsl_spmatrix_free(graph->t);
    gsl_spmatrix_free(graph->o);
}


// Graph operations
void add_edges(struct graph_t* graph, struct edge_t e[]) {
    for (int i = 0; i < 2; i++) {
        gsl_spmatrix_set(graph->t, e[i].fr, e[i].to, -1);
        gsl_spmatrix_set(graph->t, e[i].to, e[i].fr, -1);
    }
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
        return NO_DIRECTION;
    }
}

// Check if a vertex dest is directly reachable (i.e at a distance of 1) from a vertex src
bool is_reachable(const struct graph_t* graph, size_t src, size_t dest) {
    // Check if dest actually exists in the graph
    if (is_no_vertex(dest) || dest >= graph->num_vertices) {
        return false;
    }
    // Check if there is an edge between src and dest
    // TODO maybe check the validity of the direction ?
    printf("%zu is reachable from %zu", dest, src);
    if (gsl_spmatrix_get(graph->t, src, dest) <= 0)
        return false;

    return true;
}

// Get all the reachables vertices from the vertex v
// Fill an array with the reachable vertices (no_vertex() if vertex is not reachable)
size_t get_reachables(const struct graph_t* graph, size_t v, size_t vertices[]) {
    size_t reachables = 0;
    size_t n = graph->t->size1;
    vertices[NORTH] = v >= n ? v - n : no_vertex();
    vertices[SOUTH] = v + n;
    vertices[WEST] = v > 0 ? v - 1 : no_vertex();
    vertices[EAST] = v + 1;

    for (int i = 0; i < MAX_DIRECTION; i++) {
        if (is_reachable(graph, v, vertices[i]))
            reachables++;
        else
            vertices[i] = no_vertex();
    }
    return reachables;
}

// Get a random wall
// May not work for other than SQUARE board
struct wall_t random_wall(const struct graph_t* graph) {
    int n = graph->t->size1;
    struct wall_t wall = no_wall();

    do {
        size_t reachables[MAX_DIRECTION];
        size_t v = rand() % graph->num_vertices;
        get_reachables(graph, v, reachables);

        // Check if we can put a whole wall
        if (!is_no_vertex(reachables[WEST]) && !is_no_vertex(reachables[EAST])) {
            wall.e1 = (struct edge_t){ v, reachables[WEST] };
            wall.e2 = (struct edge_t){ v, reachables[EAST] };
        }
        else if (!is_no_vertex(reachables[NORTH]) && !is_no_vertex(reachables[SOUTH])) {
            wall.e1 = (struct edge_t){ v, reachables[NORTH] };
            wall.e2 = (struct edge_t){ v, reachables[SOUTH] };
        }
    } while (is_no_wall(wall));

    return wall;
}

//// GARBAGE
/*
struct graph_t* graph_copy(struct graph_t* graph) {
    struct graph_t* copy = malloc(sizeof(*copy));
    copy->num_vertices = graph->num_vertices;
    copy->t = gsl_spmatrix_alloc(graph->t->size1, graph->t->size2);
    copy->o = gsl_spmatrix_alloc(graph->o->size1, graph->o->size2);
    gsl_spmatrix_memcpy(copy->t, graph->t);
    gsl_spmatrix_memcpy(copy->o, graph->o);

    return copy;
}
*/

/*
// Return the direction of an edge
int find_direction(struct graph_t* graph, struct edge_t e) {
    int n = graph->t->size1;
    int d = e.to - e.fr;

    return
        d == -1 ? WEST :
        d == 1 ? EAST :
        d == -n ? NORTH :
        d == n ? SOUTH :
        ERROR_DIRECTION;
}*/