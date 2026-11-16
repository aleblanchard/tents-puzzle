#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_tools.h"

int main(int argc, char* argv[]) {
	if (argc < 3) {	 // verifie le nombre d'argument
		printf("Usage : ./game_solve <option> <input> [<output>]\n");
		printf("-s pour résoudre, -c ppour compter le nombre de solutions\n");
		return EXIT_FAILURE;
	}

	char* option = argv[1];
	char* input = argv[2];

	game g = game_load(input);
	if (g == NULL) {
		return EXIT_FAILURE;
	}

	if (strcmp(option, "-s") == 0) {  // si l'option est solveur (-s)
		bool solved = game_solve(g);
		if (!solved) {	// on verifie si il y a une solution
			game_delete(g);
			return EXIT_FAILURE;
		}

		if (argc >= 4) {  // si il y a un output on save la game dedans
			game_save(g, argv[3]);
		} else {
			game_print(g);	// sinon on affiche juste la solution
		}

	} else if (strcmp(option, "-c") == 0) {	 // si l'option est compteur (-c)
		int nb_sol = game_nb_solutions(g);

		if (argc >= 4) {  // sauvegarde le nb de solution dans fichier si fichier output
			FILE* f = fopen(argv[3], "w");
			if (f != NULL) {
				fprintf(f, "%d\n", nb_sol);
				fclose(f);
			}
		} else {
			printf("%d\n", nb_sol);	 // sinon on affiche juste le nb de solution
		}
	}
	game_delete(g);
	return EXIT_SUCCESS;
}