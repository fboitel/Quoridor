#ifndef _QUOR_BOARD_H_
#define _QUOR_BOARD_H_

#include "opt.h"
#include "graph.h"
#include <stdbool.h>

enum direction_t { NO_DIRECTION, NORTH, SOUTH, WEST, EAST, MAX_DIRECTION };

enum orientation_t { HORIZONTAL, VERTICAL, ERROR_ORIENTATION = -1 };

static inline size_t no_vertex(void) {
	return SIZE_MAX;
}

static inline bool is_no_vertex(const size_t v) {
	return v == no_vertex();
}

// Check if there is an adjacent vertex of v (i.e a vertex linked by an edge) in the direction d
// Return it if so, else return no_vertex()
size_t vertex_from_direction(const struct graph_t* graph, size_t v, enum direction_t d);

// Check if vertex dest is linked to vertex src (i.e there is an edge between them)
bool is_linked(const struct graph_t* graph, size_t src, size_t dest);

size_t get_linked(const struct graph_t* graph, size_t v, size_t vertices[]);

// Initialize a square graph of size n
struct graph_t* graph_init(size_t n, enum shape_t shape);

// Free a graph
void graph_free(struct graph_t* graph);

// Add edges in the graph
void add_edges(struct graph_t* graph, struct edge_t e[]);

int opposite(int d);

void display_board(struct graph_t* board, size_t board_size, size_t position_player_1, size_t position_player_2);


#endif // _QUOR_BOARD_H_