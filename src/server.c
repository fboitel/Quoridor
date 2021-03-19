#include "player.h"         // temporaly
#include <getopt.h>         // for option
#include <gsl/gsl_matrix.h> // for matrix
#include <string.h> // for strcmp
#include <unistd.h> // for check access to file


enum board_shape_t {SQUARE, TORIC, H, SNAKE, NOT_SHAPE = -1};

int boardSize = 10;
enum board_shape_t boardShape = SQUARE;
char * pathPlayer1 = NULL;
char * pathPlayer2 = NULL;

enum board_shape_t parseBoardShape(char * t) {
	if ( strlen(t) > 1) {
		return NOT_SHAPE;
	}
	switch (*t) {
		case 'c' :
		return SQUARE;
		
		case 't' :
		return TORIC;
		
		case 'h' :
		return H;

		case 's' :
		return SNAKE;

		default :
			return NOT_SHAPE;
	}
}

void parse_opts(int argc, char *argv[]) {

	int nbPlayers = 0;

	for (int i = 1; i < argc; ++i) {
  //  	printf("%s \n", argv[i]);
		char *arg = argv[i];


// TODO check if it's a number
		if( !strcmp(arg, "-m")) {
			++i;

			if ( i >= argc || (boardSize = atoi(argv[i])) <= 0 ) { // check size
			printf("USAGE1\n");
			exit(1);
			}
		} else if( !strcmp(arg, "-t")) {
			++i;
			if ( i >= argc || (boardShape = parseBoardShape(argv[i])) == NOT_SHAPE) { // check c t h or s
				printf("USAGE2\n");
				exit(1);
			}
		} else {
			if (pathPlayer2 != NULL || access( argv[i], F_OK)) {
				printf("USAGE3\n");
				exit(1);
			}
			if ( pathPlayer1 == NULL) {
				pathPlayer1 = argv[i];
			} else {
				pathPlayer2 = argv[i];
			}

		}
    }
	if (pathPlayer2 == NULL) {
				printf("USAGE4\n");
				exit(1);
			}
}

int main(int argc, char *argv[]) {

  parse_opts(argc, argv);

  // init a new matrix size of 3x3
  gsl_matrix *board = gsl_matrix_alloc(3, 3);

  // free the matrix
  gsl_matrix_free(board);

  /* pseudo code for game loop
  start_player = random()
  for each player p
      p->initialize(p == start_player ? BLACK : WHITE, graph, num_walls)
  while true
     p = compute_next_player()
     move = p->play(move)
     if is_winning()
        break
  for each player p
     p->finalize()
     */
}
