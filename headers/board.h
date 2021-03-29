#ifndef _QUOR_BOARD_H_
#define _QUOR_BOARD_H_

#include "opt.h"
#include "graph.h"
#include <stdbool.h>

enum direction_t { NORTH, SOUTH, WEST, EAST, MAX_DIRECTION, NO_DIRECTION = -1 };

struct wall_t { struct edge_t e1, e2; };

static inline size_t no_vertex(void) {
    return SIZE_MAX;
}

static inline bool is_no_vertex(const size_t v) {
    return v == no_vertex();
}

// A special wall to tell that we put no wall
static inline struct wall_t no_wall(void) {
    return (struct wall_t) { no_edge(), no_edge() };
}

static inline bool is_no_wall(const struct wall_t wall) {
    return is_no_edge(wall.e1) && is_no_edge(wall.e2);
}

// Check if a vertex dest is directly reachable (i.e at a distance of 1) from a vertex src
bool is_reachable(const struct graph_t* graph, size_t src, size_t dest);

size_t get_reachables(const struct graph_t* graph, size_t v, size_t vertices[]);

// Initialize a square graph of size n
struct graph_t* graph_init(size_t n, enum shape_t shape);

// Free a graph
void graph_free(struct graph_t* graph);

// Return a random wall that can be put in the graph
struct wall_t random_wall(const struct graph_t* graph);

// Add edges in the graph
void add_edges(struct graph_t* graph, struct edge_t e[]);

#endif // _QUOR_BOARD_H_