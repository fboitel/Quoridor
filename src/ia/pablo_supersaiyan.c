#include "ia.h"
#include "ia_utils.h"
#include "board.h"
#include "move.h"

#define MAX_POSSIBLE_WALLS 500
#define IMPOSSIBLE_DISTANCE 500000
#define IMPOSSIBLE_ID 1234500
#define MAX_MOVE_PLACES 7
char *name = "Pablo Super Saiyan";


//Pablo always does everything possible to lengthen the path of his opponents


//Returns the number of vertices owned by each player
size_t vertices_owned(struct graph_t *graph){
	int i = 0;
	while (gsl_spmatrix_uint_get(graph->o, 0, i) == 1 || gsl_spmatrix_uint_get(graph->o, 1, i) == 1)
		i++;
	return i;
}

//init the array d of the distance and v of the vertices
void release_init(size_t d[], size_t v[], size_t pos, size_t length){
		for (size_t i = 0; i < length; i++) {
		d[i] = IMPOSSIBLE_DISTANCE;
		v[i] = i;
	}
	d[pos] = 0;
}

//Returns the vertex that have the smallest distance, and make it desapear of the array v.
size_t extract_min(size_t d[], size_t v[],size_t length){
	size_t ind = length+1;
	size_t min = length*4;
	for(size_t i = 0; i < length; i++)
		if(d[v[i]] < min){
			min = d[v[i]];
			ind = i;
		}
	size_t vertex = v[ind];
	v[ind] = v[length - 1];
	return vertex;
}

//Returns true if elem belong to arr
bool belong(size_t elem, size_t arr[], size_t length){
	for (size_t i = 0; i < length; i++)
		if (elem == arr[i])
			return true;
	return false;
}

//Releases the edge between vertex u and vertex v
void release_dijk(size_t u, size_t v, size_t d[]){
	if (d[v] > d[u] + 1)
		d[v] = d[u] + 1;
}

//Returns the shortest distance in array d under condition that the vertes is a target vertex.
size_t get_shortest(size_t d[], struct graph_t *graph, enum color_t color){
	size_t min = IMPOSSIBLE_DISTANCE;
	for (size_t i = 0; i < graph->num_vertices; i++)
		if (d[i] < min && gsl_spmatrix_uint_get(graph->o, 1 - color, i)){
			min = d[i];
		}
	return min;
}

size_t dijkstra(struct graph_t *graph, size_t pos, enum color_t color){
	size_t nb_v = graph->num_vertices;
	size_t d[nb_v];
	size_t v[nb_v];
	release_init(d, v, pos, nb_v);
	while (nb_v){
		size_t u = extract_min(d, v, nb_v);
		nb_v--;
		size_t succ[MAX_DIRECTION];
		get_linked(graph, u, succ);
		for (size_t i = 0; i < MAX_DIRECTION; i++)
			if (belong(succ[i], v, nb_v))
				release_dijk(u, succ[i], d);
	}
	return get_shortest(d, graph, color);
}

//Gather all the place where a wall can be set and returns its number
size_t get_possible_walls(struct game_state_t game, struct edge_t walls[MAX_POSSIBLE_WALLS][2]) {
	size_t nb_wall = 0;
	for (size_t i = 0; i < game.graph->num_vertices; i++) {
		// To verify if a wall can be put on the south of the vertex i and the vertex on right of it
			if (vertex_from_direction(game.graph, i, EAST) != no_vertex()) {
				if (vertex_from_direction(game.graph, i, SOUTH) != no_vertex() &&
					vertex_from_direction(game.graph, vertex_from_direction(game.graph, i, EAST), SOUTH) !=
					no_vertex()) {//Add a wall to the list if the place is free
					walls[nb_wall][0].fr = i;
					walls[nb_wall][0].to = vertex_from_direction(game.graph, i, SOUTH);
					walls[nb_wall][1].fr = vertex_from_direction(game.graph, i, EAST);
					walls[nb_wall][1].to = vertex_from_direction(game.graph, vertex_from_direction(game.graph, i, EAST), SOUTH);
					nb_wall++;
				}
			}
			// To verify if a wall can be put on the EAST of the vertex i and the vertex below
			if (vertex_from_direction(game.graph, i, SOUTH) != no_vertex()) {
				if (vertex_from_direction(game.graph, i, EAST) != no_vertex() &&
					vertex_from_direction(game.graph, vertex_from_direction(game.graph, i, SOUTH), EAST) !=
					no_vertex()) {//Add a wall to the list uf the place is free
					walls[nb_wall][0].fr = i;
					walls[nb_wall][0].to = vertex_from_direction(game.graph, i, EAST);
					walls[nb_wall][1].fr = vertex_from_direction(game.graph, i, SOUTH);
					walls[nb_wall][1].to = vertex_from_direction(game.graph, vertex_from_direction(game.graph, i, SOUTH), EAST);
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
size_t get_the_better_wall_id(struct game_state_t game, struct edge_t posswall[MAX_POSSIBLE_WALLS][2], size_t nb_wall) {
	size_t dist = dijkstra(game.graph, game.opponent.pos, game.opponent.color);
	size_t wall_id = IMPOSSIBLE_ID;
	size_t closest = dist;
	bool dist_upgraded = false;
	for (size_t i = 0; i < nb_wall; i++) {
		size_t dir = gsl_spmatrix_uint_get(game.graph->t, posswall[i][0].fr, posswall[i][0].to);
		put_wall_opti(game.graph, posswall[i]);
		size_t new_dist = dijkstra(game.graph, game.opponent.pos, game.opponent.color);
		size_t new_closest = dijkstra(game.graph, posswall[i][0].fr, game.opponent.color);
		if (dijkstra(game.graph, game.self.pos, game.self.color) < IMPOSSIBLE_DISTANCE){
			if (new_dist > dist && new_dist < IMPOSSIBLE_DISTANCE) {
				dist_upgraded = true;
				dist = new_dist;
				closest = new_closest;
				wall_id = i;
			}
			if (new_dist == dist && new_dist < IMPOSSIBLE_DISTANCE && new_closest > closest && dist_upgraded){
				dist = new_dist;
				closest = new_closest;
				wall_id = i;
			}
		}
		//display_board(graph, (size_t)sqrtl(graph->num_vertices), 0, graph->num_vertices - 1);
		//printf("OK3OK");
		remove_wall_opti(game.graph, posswall[i], dir);
	}
	// printf("(%zu %zu)", 2 * (graph->num_vertices) + 5, dist + 5);
	return wall_id;
}

size_t get_linked_for_move(struct graph_t *graph, size_t self_pos, size_t opp_pos, size_t linked[]){
	size_t num = get_linked(graph, self_pos, linked);
	int ind = MAX_DIRECTION;
	linked[ind] = no_vertex();
	linked[ind + 1] = no_vertex();
	for (int i = 1; i < MAX_DIRECTION; i++){
		if (linked[i] == opp_pos){
			if (vertex_from_direction(graph, linked[i], i) != no_vertex()){
				linked[i] = vertex_from_direction(graph, linked[i], i);
				return num;
			}
			else {
				for (int j = 1; j < MAX_DIRECTION; j++)
					if (vertex_from_direction(graph, linked[i], j) != no_vertex() && vertex_from_direction(graph, linked[i], j) != self_pos){
						linked[ind] = vertex_from_direction(graph, linked[i], j);
						ind++;
						num++;
					}
				linked[i] = no_vertex();

			}
		}
	}
	return num;
}

//Moves forward
size_t move_forward(struct game_state_t game) {
	size_t linked[MAX_MOVE_PLACES];
	size_t num = get_linked_for_move(game.graph, game.self.pos, game.opponent.pos, linked);
	int dir = NO_DIRECTION;
	if (num == 0) {
		fprintf(stderr, "ERROR: Player is blocked\n");
	}
	size_t shortest = 2 * (game.graph->num_vertices);
	for (int i = 1; i < MAX_MOVE_PLACES; i++) {
		if (!is_no_vertex(linked[i])) {
			size_t dist_tmp = dijkstra(game.graph, linked[i], game.self.color);
			if (dist_tmp < shortest) {
				shortest = dist_tmp;
				dir = i;
				
			}
		}
	}
	fprintf(stderr, "%ld\n", game.self.pos);
	for (int i = 0; i < 6; i++)
		fprintf(stderr, "(%zu, dist : %zu) ", linked[i], dijkstra(game.graph, linked[i], game.self.color));
	return linked[dir];
}

struct move_t make_first_move(struct game_state_t game) {
	return make_default_first_move(game);
}

// Compute the move
struct move_t make_move(struct game_state_t game) {
	struct move_t move;
	struct edge_t poss_walls[MAX_POSSIBLE_WALLS][2];
	//size_t size_board = sqrt(game.graph->num_vertices);
	if (dijkstra(game.graph, game.opponent.pos, game.opponent.color) >= dijkstra(game.graph, game.self.pos, game.self.color) ||
		dijkstra(game.graph, game.self.pos, game.self.color) == 1 || game.self.num_walls == 0){
		move.m = move_forward(game);
		move.t = MOVE;
		}
	else {
		size_t nb_of_walls = get_possible_walls(game, poss_walls);
		size_t id_wall = get_the_better_wall_id(game, poss_walls, nb_of_walls);

		if (id_wall != IMPOSSIBLE_ID) {
			// Put a wall if it's possible
			move.m = game.self.pos;
			move.e[0].fr = poss_walls[id_wall][0].fr;
			move.e[0].to = poss_walls[id_wall][0].to;
			move.e[1].fr = poss_walls[id_wall][1].fr;
			move.e[1].to = poss_walls[id_wall][1].to;
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
