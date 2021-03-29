#ifndef _QUOR_CLIENT_H_
#define _QUOR_CLIENT_H_

#include "graph.h"

struct player_t {
    char* name;
    enum color_t id;
    struct graph_t* graph;
    size_t pos;          // Position
    size_t num_walls; // Numbers of wall to place
};

#endif // _QUOR_CLIENT_H_