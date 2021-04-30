#include "ia.h"
#include "ia_utils.h"
#include "board.h"
#include "move.h"
#include <stdio.h>

/** TODO
 * Jump over opponent
 * Choose first move
 * Make wall placing better
 **/

char* name = "GoodBoy";

struct move_t make_first_move(struct game_state_t game) {
	return make_default_first_move(game);
}

enum movetype_t get_type(struct game_state_t game) {
	char input;
	enum movetype_t type = NO_TYPE;

	while (type > MOVE) {
		printf("\nWhat do you want to do ?\n");
		printf("WALL (W), MOVE (M): ");
		input = getchar();
		fflush(stdin);

		switch (input) {
		case 'W':
			type = WALL;
			break;
		case 'M':
			type = MOVE;
			break;
		default:
			printf("Invalid type\n");
		}
		if (type == WALL && game.self.num_walls == 0) {
			printf("You do not have walls anymore !\n");
			type = NO_TYPE;
		}
	}
	return type;
}

size_t get_move(struct game_state_t game) {
	char input;
	size_t linked[MAX_DIRECTION];
	get_linked(game.graph, game.self.pos, linked);
	enum direction_t d = NO_DIRECTION;

	while (true) {
		printf("\nWhere do you want to go ?\n");
		printf("NORTH (N), SOUTH (S), WEST (W), EAST (E): ");
		input = getchar();
		fflush(stdin);

		switch (input) {
		case 'N':
			d = NORTH;
			break;
		case 'S':
			d = SOUTH;
			break;
		case 'W':
			d = WEST;
			break;
		case 'E':
			d = EAST;
			break;
		default:
			printf("Invalid direction\n");
			continue;
		}

		if (linked[d] == game.opponent.pos)
			printf("There is someone here !\n");
		else if (is_no_vertex(linked[d]))
			printf("You can not go this way\n");
		else
			return linked[d];
	}
}

void get_wall(struct game_state_t game, struct edge_t edges[]) {
	char input;
	enum orientation_t orientation;
	size_t vertex;

	while (true) {
		printf("\nOrientation ?\n");
		printf("HORIZONTAL (H), VERTICAL (V): ");
		input = getchar();
		fflush(stdin);

		switch (input) {
		case 'H':
			orientation = HORIZONTAL;
			break;
		case 'V':
			orientation = VERTICAL;
			break;
		default:
			printf("Invalid orientation\n");
			continue;
		}

		printf("\nTop left corner vertex ?\n");
		printf("(0-%zu): ", game.graph->num_vertices - 1);
		scanf("%zu", &vertex);
		fflush(stdin);

		if (vertex >= game.graph->num_vertices)
			printf("Invalid vertex\n");
		else
			break;
	}
	if (orientation == HORIZONTAL) {
		size_t v1 = vertex;
		size_t v2 = vertex_from_direction(game.graph, vertex, EAST);
		edges[0] = (struct edge_t){ v1, vertex_from_direction(game.graph, v1, SOUTH) };
		edges[1] = (struct edge_t){ v2, vertex_from_direction(game.graph, v2, SOUTH) };
	}
	else {
		size_t v1 = vertex;
		size_t v2 = vertex_from_direction(game.graph, vertex, SOUTH);
		edges[0] = (struct edge_t){ v1, vertex_from_direction(game.graph, v1, EAST) };
		edges[1] = (struct edge_t){ v2, vertex_from_direction(game.graph, v2, EAST) };
	}
}

// Jerry's strategy is to run directly to the arrival without placing any walls
struct move_t make_move(struct game_state_t game) {
	struct move_t move;
	move.c = game.self.color;
	move.t = get_type(game);

	if (move.t == WALL) {
		get_wall(game, move.e);
	}
	else {
		move.m = get_move(game);
		move.e[0] = no_edge();
		move.e[1] = no_edge();
	}
	return move;
}
