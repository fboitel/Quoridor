#include "ia.h"
#include "ia_utils.h"
#include "board.h"
#include "move.h"

#define MAX_POSSIBLE_WALLS 500
//#define MAX_DISTANCE 500000
#define IMPOSSIBLE_ID 1234500
char *name = "Pablo";


//Pablo always does everything possible to lengthen the path of his opponents


//Returns the number of vertices owned by each player
size_t vertices_owned(struct graph_t *graph){
	int i = 0;
	while (gsl_spmatrix_uint_get(graph->o, 0, i) == 1 || gsl_spmatrix_uint_get(graph->o, 1, i) == 1)
		i++;
	return i;
}

//Initialise the parents and weights arrays
void init_w_arrays(size_t weight[], size_t pos, size_t length) {
	for (size_t i = 0; i < length; i++) {
		weight[i] = 2 * length;
	}
	weight[pos] = 0;
}

//Releases arcs linked with position pos in graph and returns the next vertex to visit
void release(struct graph_t *graph, size_t weight[], size_t pos) {
	size_t linked[MAX_DIRECTION];
	get_linked(graph, pos, linked);
	for (int i = 1; i < MAX_DIRECTION; i++) {
		if (linked[i] != no_vertex()) {
			size_t new_w = weight[pos] + 1;
			if (new_w < weight[linked[i]]) {
				weight[linked[i]] = new_w;
			}
		}
	}
}

//Returns the smallest value of the tab if it respects some conditions
size_t shorter(struct graph_t *graph, size_t weight[], enum color_t color, size_t n) {
	size_t min = n * n * n;
	for (size_t i = 0; i < n; i++) {
		if (gsl_spmatrix_uint_get(graph->o, 1 - color, i) == 1 && weight[i] < min) {
			min = weight[i];
		}
	}
	return min;
}

//Returns the shortest distance between a position on the graph and the target zone using Bellman-Ford algorithm
size_t get_distance(struct graph_t *graph, size_t pos, enum color_t color) {
	size_t weight[graph->num_vertices];
	init_w_arrays(weight, pos, graph->num_vertices);
	for (size_t i = 0; i < graph->num_vertices; i++) {
		for (size_t j = 0; j < graph->num_vertices; j++) {
			release(graph, weight, j);
		}
	}
	size_t sh = shorter(graph, weight, color, graph->num_vertices);
	return sh;
}

//Gather all the place where a wall can be set and returns its number
size_t get_possible_walls(struct graph_t *graph, struct edge_t walls[MAX_POSSIBLE_WALLS][2], enum color_t color) {
	size_t nb_wall = 0;
	size_t side1 = 0;
	size_t side2 = 0;
	size_t board_size = vertices_owned(graph);
	if (gsl_spmatrix_uint_get(graph->o, color, 0) == 1)
		side1 = board_size*5;
	else 
		side2 = board_size*5;
	printf("(((((%zu, %zu)))))", side1, side2);
	for (size_t i = 0 + side1; i < graph->num_vertices-side2; i++) {
		// To verify if a wall can be put on the south of the vertex i and the vertex on right of it
			if (vertex_from_direction(graph, i, EAST) != no_vertex()) {
				if (vertex_from_direction(graph, i, SOUTH) != no_vertex() &&
					vertex_from_direction(graph, vertex_from_direction(graph, i, EAST), SOUTH) !=
					no_vertex()) {//Add a wall to the list if the place is free
					walls[nb_wall][0].fr = i;
					walls[nb_wall][0].to = vertex_from_direction(graph, i, SOUTH);
					walls[nb_wall][1].fr = vertex_from_direction(graph, i, EAST);
					walls[nb_wall][1].to = vertex_from_direction(graph, vertex_from_direction(graph, i, EAST), SOUTH);
					nb_wall++;
				}
			}
			// To verify if a wall can be put on the EAST of the vertex i and the vertex below
			if (vertex_from_direction(graph, i, SOUTH) != no_vertex()) {
				if (vertex_from_direction(graph, i, EAST) != no_vertex() &&
					vertex_from_direction(graph, vertex_from_direction(graph, i, SOUTH), EAST) !=
					no_vertex()) {//Add a wall to the list uf the place is free
					walls[nb_wall][0].fr = i;
					walls[nb_wall][0].to = vertex_from_direction(graph, i, EAST);
					walls[nb_wall][1].fr = vertex_from_direction(graph, i, SOUTH);
					walls[nb_wall][1].to = vertex_from_direction(graph, vertex_from_direction(graph, i, SOUTH), EAST);
					nb_wall++;
			}
		}
	}
	return nb_wall;
}

//Put a wall on the graph
void put_wall_opti(struct graph_t *graph, struct edge_t wall[2]) {
	gsl_spmatrix_uint_set(graph->t, wall[0].fr, wall[0].to, 0);
	gsl_spmatrix_uint_set(graph->t, wall[0].to, wall[0].fr, 0);
	gsl_spmatrix_uint_set(graph->t, wall[1].fr, wall[1].to, 0);
	gsl_spmatrix_uint_set(graph->t, wall[1].to, wall[1].fr, 0);
}

void remove_wall_opti(struct graph_t *graph, struct edge_t wall[2], enum direction_t dir) {
	gsl_spmatrix_uint_set(graph->t, wall[0].fr, wall[0].to, dir);
	gsl_spmatrix_uint_set(graph->t, wall[0].to, wall[0].fr, opposite(dir));
	gsl_spmatrix_uint_set(graph->t, wall[1].fr, wall[1].to, dir);
	gsl_spmatrix_uint_set(graph->t, wall[1].to, wall[1].fr, opposite(dir));
}


//Returns the best place to put a wall in order to delay the opponent
size_t get_the_better_wall_id(struct graph_t *graph, struct edge_t posswall[MAX_POSSIBLE_WALLS][2], size_t nb_wall, size_t pos, enum color_t color) {
	size_t dist = get_distance(graph, pos, color);
	size_t wall_id = IMPOSSIBLE_ID;
	for (size_t i = 0; i < nb_wall; i++) {		
		size_t dir = gsl_spmatrix_uint_get(graph->t, posswall[i][0].fr, posswall[i][0].to);
		put_wall_opti(graph, posswall[i]);
		size_t new_dist = get_distance(graph, pos, color);
		if (new_dist > dist && new_dist < 2 * (graph->num_vertices)) {
			dist = new_dist;
			wall_id = i;
		}
		remove_wall_opti(graph, posswall[i], dir);
	}
	printf("(%zu %zu)", 2 * (graph->num_vertices) + 5, dist + 5);
	return wall_id;
}

//Returns the best place to put a wall in order to delay the opponent
size_t get_a_good_wall_id(struct graph_t *graph, struct edge_t posswall[MAX_POSSIBLE_WALLS][2], size_t nb_wall, size_t pos, enum color_t color){
	size_t dist = get_distance(graph, pos, color);

	for (size_t i = 0; i < nb_wall; i++) {
		size_t dir = gsl_spmatrix_uint_get(graph->t, posswall[i][0].fr, posswall[i][0].to);
		put_wall_opti(graph, posswall[i]);
		size_t new_dist = get_distance(graph, pos, color);
		if (new_dist > dist && new_dist < 2 * (graph->num_vertices)) 
			return i;	
		remove_wall_opti(graph, posswall[i], dir);
	}
	printf("(%zu %zu)", 2 * (graph->num_vertices) + 5, dist + 5);
	return IMPOSSIBLE_ID;
}


//Moves forward
size_t move_forward(struct game_state_t game) {
	size_t linked[MAX_DIRECTION];
	size_t num = get_linked(game.graph, game.self.pos, linked);
	int dir = NO_DIRECTION;
	if (num == 0) {
		fprintf(stderr, "ERROR: Player is blocked\n");
	}
	size_t shortest = 2 * (game.graph->num_vertices);
	for (int i = 1; i < MAX_DIRECTION; i++) {
		if (!is_no_vertex(linked[i])) {
			if (linked[i] != game.opponent.pos){
				size_t dist_tmp = get_distance(game.graph, linked[i], game.self.color);
				if (dist_tmp < shortest) {
					shortest = dist_tmp;
					dir = i;
				}
			}
		}
	}
	return linked[dir];
}

struct move_t make_first_move(struct game_state_t game) {
	return make_default_first_move(game);
}

// Compute the move
struct move_t make_move(struct game_state_t game) {
	struct move_t move;
	struct edge_t poss_walls[MAX_POSSIBLE_WALLS][2];
	size_t size_board = sqrt(game.graph->num_vertices);
	if (get_distance(game.graph, game.opponent.pos, game.opponent.color) > size_board/3){
		move.m = move_forward(game);
		move.t = MOVE;
		}
	else {
		size_t nb_of_walls = get_possible_walls(game.graph, poss_walls, game.opponent.color);
		size_t id_wall = get_a_good_wall_id(game.graph, poss_walls, nb_of_walls, game.opponent.pos, game.opponent.color);

		if (id_wall != IMPOSSIBLE_ID) {
			// Put a wall if it's possible
			move.m = game.self.pos;
			move.e[0].fr = poss_walls[id_wall][0].fr;
			move.e[0].to = poss_walls[id_wall][0].to;
			move.e[1].fr = poss_walls[id_wall][1].fr;
			move.e[1].to = poss_walls[id_wall][1].to;
			printf("wall : %ld:%ld - %ld:%ld \n", move.e[0].fr,  move.e[0].to,  move.e[1].fr,  move.e[1].to);
			move.t = WALL;
		} else {
			// Move forward
			move.m = move_forward(game);
			move.t = MOVE;
		}
		}
	move.c = game.self.color;
	return move;
}
