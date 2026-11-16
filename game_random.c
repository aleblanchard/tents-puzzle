#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_tools.h"

int main(int argc, char* argv[]) {
	if (argc < 7) {	 // verifier le nb d'args
		printf("Usage : /game_random <nb_rows> <nb_cols> <wrapping> <nb_trees> <nb_bigtrees> <solution> [<filename>]\n");
		printf("Example: ./game_random 8 8 0 4 2 1 random.sol\n");
		return EXIT_FAILURE;
	}

	srand(time(NULL));

	uint nb_rows = atoi(argv[1]);  // transformer les strings en int pour pouvoir les utiliser
	uint nb_cols = atoi(argv[2]);
	uint wrapping = atoi(argv[3]);	// 0 false 1 true
	uint nb_trees = atoi(argv[4]);
	uint nb_bigtrees = atoi(argv[5]);
	uint solution = atoi(argv[6]);	// 0 false 1 true

	game g = game_random(nb_rows, nb_cols, wrapping, nb_trees, nb_bigtrees, solution);
	game_print(g);

	if (argc >= 8) {  // sauvegarde si filename
		game_save(g, argv[7]);
	}


	game_delete(g);
	return EXIT_SUCCESS;
}