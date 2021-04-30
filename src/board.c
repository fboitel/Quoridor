#include "board.h"
#include <gsl/gsl_spmatrix.h>
#include <string.h>
#include <math.h>

//// Graph structure manipulation functions

// Initialize a square graph of size m
struct graph_t* square_init(size_t m) {
	struct graph_t* graph = malloc(sizeof(*graph));

	size_t n = m * m;
	graph->num_vertices = n;

	// Initialize the actual board
	graph->t = gsl_spmatrix_uint_alloc(n, n);

	// Fill the whole graph with 0
	gsl_spmatrix_uint_set_zero(graph->t);

	// Fill the strict bottom corner
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < i; ++j) {

			if (j % m != m - 1 && i == j + 1) {
				gsl_spmatrix_uint_set(graph->t, i, j, 3);
				gsl_spmatrix_uint_set(graph->t, j, i, 4);
			}

			if (j / m < m - 1 && i == j + m - 1) {
				gsl_spmatrix_uint_set(graph->t, i + 1, j, 1);
				gsl_spmatrix_uint_set(graph->t, j, i + 1, 2);
			}
		}
	}

	// Initialize proprietary matrix
	graph->o = gsl_spmatrix_uint_alloc(2, n);
	for (size_t i = 0; i < m; i++) {
		gsl_spmatrix_uint_set(graph->o, BLACK, i, 1);
		gsl_spmatrix_uint_set(graph->o, WHITE, n - i - 1, 1);
	}

	return graph;
}

struct graph_t* graph_init(size_t m, enum shape_t shape) {
	switch (shape) {
	case SQUARE:
		return square_init(m);
	default:
		return square_init(m);
	}
}

void graph_free(struct graph_t* graph) {
	gsl_spmatrix_uint_free(graph->t);
	gsl_spmatrix_uint_free(graph->o);
	free(graph);
}

// Graph operations
void placeWall(struct graph_t* graph, struct edge_t e[]) {

		for (int i = 0; i < 2; i++) {
			if (!is_no_edge(e[i])) {

				gsl_spmatrix_uint_set(graph->t, e[i].fr, e[i].to, 0);
				gsl_spmatrix_uint_set(graph->t, e[i].to, e[i].fr, 0);

			}
		}
		/*


	int board_size = sqrt(graph->num_vertices);

	// get nodes
	size_t first_node = e[0].fr;
	size_t second_node = e[0].to;

	// sort nodes
	if (first_node > second_node) {
		size_t tmp = first_node;
		first_node = second_node;
		second_node = tmp;
	}

	if (first_node + 1 == second_node) { // vertical wall
		if (first_node + board_size == e[1].fr || first_node + board_size == e[1].to) {
			gsl_spmatrix_uint_set(graph->t, first_node, second_node, 5);
			gsl_spmatrix_uint_set(graph->t, second_node, first_node, 5);

			gsl_spmatrix_uint_set(graph->t, first_node + board_size, second_node + board_size, 6);
			gsl_spmatrix_uint_set(graph->t, second_node + board_size, first_node + board_size, 6);

		}
		else {
			gsl_spmatrix_uint_set(graph->t, first_node + board_size, second_node + board_size, 6);
			gsl_spmatrix_uint_set(graph->t, second_node + board_size, first_node + board_size, 6);

			gsl_spmatrix_uint_set(graph->t, first_node, second_node, 5);
			gsl_spmatrix_uint_set(graph->t, second_node, first_node, 5);

		}
	}

	else { // horizontal wall
		if (first_node + 1 == e[1].fr || first_node + 1 == e[1].to) {
			gsl_spmatrix_uint_set(graph->t, first_node, second_node, 7);
			gsl_spmatrix_uint_set(graph->t, second_node, first_node, 7);

			gsl_spmatrix_uint_set(graph->t, first_node + 1, second_node + 1, 8);
			gsl_spmatrix_uint_set(graph->t, second_node + 1, first_node + 1, 8);

		}
		else {
			gsl_spmatrix_uint_set(graph->t, first_node, second_node, 8);
			gsl_spmatrix_uint_set(graph->t, second_node, first_node, 8);

			gsl_spmatrix_uint_set(graph->t, first_node + 1, second_node + 1, 7);
			gsl_spmatrix_uint_set(graph->t, second_node + 1, first_node + 1, 7);

		}
	}
*/
}

int opposite(int d) {
	switch (d) {
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

// Check if vertex dest is linked to vertex src (i.e there is an edge between
// them)
bool is_linked(const struct graph_t* graph, size_t src, size_t dest) {
	return gsl_spmatrix_uint_get(graph->t, src, dest) > 0;
}

// Check if there is an adjacent vertex of v (i.e a vertex linked by an edge) in
// the direction d
// Return it if so, else return no_vertex()
size_t vertex_from_direction(const struct graph_t* graph, size_t v, enum direction_t d) {
	// TODO : this function can be optimised

	if (v >= graph->num_vertices)
		return no_vertex();

	for (size_t i = 0; i < graph->num_vertices; i++) {
		if (gsl_spmatrix_uint_get(graph->t, v, i) == d)
			return i;
	}

	return no_vertex();
}

// Get all the linked vertices from the vertex v
// Fill an array with the linked vertices (no_vertex() if there is no vertex in
// the direction)
size_t get_linked(const struct graph_t* graph, size_t v, size_t vertices[]) {
	size_t linked = 0;
	vertices[NO_DIRECTION] = v; // First one is himself

	for (enum direction_t i = NO_DIRECTION + 1; i < MAX_DIRECTION; i++) {
		vertices[i] = vertex_from_direction(graph, v, i);
		linked += !is_no_vertex(vertices[i]);
	}
	return linked;
}

void display_board(struct graph_t* board, size_t board_size, size_t position_player_1, size_t position_player_2) {
	/*
	 5 = wall on east + next line
	 6 = wall on east

	 7 = wall on south + south right
	 8 = wall on south
	*/

	// care about out of tab
	char next_line[100] = "";

	for (size_t i = 0; i < board_size * board_size; ++i) {

		if (i % board_size == 0) {
			printf("\n");
			printf("%s", next_line);
			printf("\n");
			strcpy(next_line, "");
		}

		if (i == position_player_1) {
			printf("\033[31m1\033[m");
		}
		else if (i == position_player_2) {
			printf("\033[34m1\033[m");
		}
		else {
			printf("0");
		}
		int end_pattern = 0;

		unsigned int matrix_state_1 = 0;
		if (i + 1 < board_size * board_size) {
			matrix_state_1 = gsl_spmatrix_uint_get(board->t, i, i + 1);
		}

		unsigned int matrix_state_2 = 0;
		if (i + board_size < board_size * board_size) {
			matrix_state_2 = gsl_spmatrix_uint_get(board->t, i, i + board_size);
		}

		// south connection
		if (matrix_state_2 == 2) {
			strcat(next_line, "| ");
		}
		else if (matrix_state_2 == 7) {
			strcat(next_line, "\033[33m──\033[m");
			end_pattern = 1;
		}
		else if (matrix_state_2 == 8) {
			strcat(next_line, "\033[33m─\033[m ");
		}
		else {
			strcat(next_line, "  ");
		}

		// east connection
		if (matrix_state_1 == 4) {
			printf(" - ");
		}
		else if (matrix_state_1 == 5) {
			printf(" \033[33m│\033[m ");
			strcat(next_line, "\033[33m│\033[m ");
			end_pattern = 2;
		}
		else if (matrix_state_1 == 6) {
			printf(" \033[33m│\033[m ");
		}
		else {
			printf("   ");
		}

		if (end_pattern == 0) {
			strcat(next_line, "  ");
		}
		else if (end_pattern == 1) {
			strcat(next_line, "\033[33m──\033[m");
		}
	}
	printf("\n");
}
