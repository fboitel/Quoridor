/**
 * @file player.h
 *
 * @brief Player interface
 */

#ifndef _QUOR_PLAYER_H_
#define _QUOR_PLAYER_H_

#include "graph.h"
#include "move.h"

/**
 * @brief Access to player information
 *
 * @return The player name as an [a-zA-Z0-9 -_]* string
 */
char const* get_player_name(void);

/**
 * @brief Initialize the player
 * 
 * @details Preconditions:
 * - `id` is either `BLACK` or `WHITE`
 * - `graph` is a heap-allocated copy of the graph where the game is
 *   played, that must be freed in the end
 * - `num_walls` is the number of edges of `graph` divided by 15,
	 rounded up
 * - `initialize()` has never been called before
 * 
 * @param id The color assigned to the player
 * @param graph The graph where the game is played
 * @param num_walls The number of walls assigned to the player
 */
void initialize(enum color_t id, struct graph_t* graph, size_t num_walls);

/** @brief Computes next move
 *
 * @param previous_move The move from the previous player
 * 
 * @return The next move for the player
 */
struct move_t play(struct move_t previous_move);

/**
 * @brief Announces the end of the game to the player, and cleans up the
 * memory he may have been using
 * 
 * @details Every allocation done during the calls to initialize and play
 * functions will be freed
 */
void finalize(void);

#endif // _QUOR_PLAYER_H_
