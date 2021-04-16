#include "board.h"
#include <gsl/gsl_spmatrix.h>

enum orientation_t { HORIZONTAL, VERTICAL, NO = -1 };

//// Graph structure manipulation functions

// Initialize a square graph of size m
struct graph_t *square_init(size_t m) {
  struct graph_t *graph = malloc(sizeof(*graph));

  size_t n = m * m;
  graph->num_vertices = n;
  graph->t = gsl_spmatrix_uint_alloc(n, n);

  // fill the hole graph with 0
  gsl_spmatrix_uint_set_zero(graph->t);

  // fill the strict bottom corner
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

  graph->o = gsl_spmatrix_uint_alloc(2, n);

  return graph;
}

struct graph_t *graph_init(size_t m, enum shape_t shape) {
  switch (shape) {
  case SQUARE:
    return square_init(m);
  default:
    return square_init(m);
  }
}

void graph_free(struct graph_t *graph) {
  gsl_spmatrix_uint_free(graph->t);
  gsl_spmatrix_uint_free(graph->o);
}

// Graph operations
void add_edges(struct graph_t *graph, struct edge_t e[]) {
  for (int i = 0; i < 2; i++) {
    if (!is_no_edge(e[i])) {
      gsl_spmatrix_uint_set(graph->t, e[i].fr, e[i].to, -1);
      gsl_spmatrix_uint_set(graph->t, e[i].to, e[i].fr, -1);
    }
  }
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
bool is_linked(const struct graph_t *graph, size_t src, size_t dest) {
  return gsl_spmatrix_uint_get(graph->t, src, dest) > 0;
}

// Check if there is an adjacent vertex of v (i.e a vertex linked by an edge) in
// the direction d Return it if so, else return no_vertex()
size_t vertex_from_direction(const struct graph_t *graph, size_t v,
                             enum direction_t d) {
  for (size_t i = 0; i < graph->num_vertices; i++) {
    if (gsl_spmatrix_uint_get(graph->t, v, i) == d)
      return i;
  }
  return no_vertex();
}

// Get all the linked vertices from the vertex v
// Fill an array with the linked vertices (no_vertex() if there is no vertex in
// the direction)
size_t get_linked(const struct graph_t *graph, size_t v, size_t vertices[]) {
  size_t linked = 0;
  vertices[NO_DIRECTION] = v; // First one is himself

  for (enum direction_t i = NO_DIRECTION + 1; i < MAX_DIRECTION; i++) {
    vertices[i] = vertex_from_direction(graph, v, i);
    linked += !is_no_vertex(vertices[i]);
  }
  return linked;
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