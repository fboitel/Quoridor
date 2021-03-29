#include "debug.h"

// Debug
void print_edge(struct edge_t* e) {
    if (e->fr == SIZE_MAX)
        printf("No edge\n");
    else
        printf("%zu --> %zu\n", e->fr, e->to);
}

void print_move(struct move_t* mv) {
    printf("Move debug\n");
    printf("Player: %d\n", mv->c);
    printf("Type: %s\n", mv->t == MOVE ? "move" : "wall");
    printf("Position %zu\n", mv->m);
    printf("Edge:\n");
    print_edge(&mv->e[0]);
    print_edge(&mv->e[1]);
}

void print_player(struct player_t* p) {
    printf("Player: %d\n", p->id);
    printf("Position %zu\n", p->pos);
}
