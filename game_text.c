#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

int main(int argc, char* argv[]) {
	game g;
	if (argc > 2) return EXIT_FAILURE;
	if (argc == 2) {
		g = game_load(argv[1]);
	} else
		g = game_default();
	while (!game_won(g)) {
		game_print(g);
		printf("HELP: h \ntouche choisie: ");
		char c;
		uint x;
		uint y;
		char nom[50] = {0};

		int nbv2 = -1;
		int nbv1 = scanf(" %c", &c);
		if (c == 't' || c == 'g' || c == 'e') {
			nbv2 = scanf(" %u %u", &x, &y);
		}
		if (c == 's') {
			nbv2 = scanf(" %49s", nom);
		}
		printf("\n");

		if (c == 'h')  // help
		{
			printf("t : Mettre une pièce TENT dans la case (x,y)\n");
			printf("g : Mettre la pièce GRASS dans la case (x,y)\n");
			printf("e : Mettre la pièce EMPTY dans la case (x,y)\n");
			printf("r : Restart / Reinitialiser la grille\n");
			printf("f : Remplir la grille avec de l'herbe\n");
			printf("z : UNDO / Annuler le dernier coup\n");
			printf("y : REDO / Rejouer le dernier coup annulé\n");
			printf("s : Sauvegarde le tableau au nom donner\n");
			printf("q : Quitter\n");
		} else if (c == 'q' && nbv1 == 1)  // fermer le jeu
		{
			printf("shame\n");
			break;
		} else if (c == 'r' && nbv1 == 1)  // restart
		{
			game_restart(g);
		} else if (c == 'f' && nbv1 == 1) {
			game_fill_grass(g);
		} else if (c == 't' && nbv1 == 1 && nbv2 == 2) {
			game_play_move(g, x, y, TENT);
		} else if (c == 'g' && nbv1 == 1 && nbv2 == 2) {
			game_play_move(g, x, y, GRASS);
		} else if (c == 'e' && nbv1 == 1 && nbv2 == 2) {
			game_play_move(g, x, y, EMPTY);
		} else if (c == 'z' && nbv1 == 1) {
			game_undo(g);
		} else if (c == 'y' && nbv1 == 1) {
			game_redo(g);
		} else if (c == 's' && nbv1 == 1) {
			game_save(g, nom);
		} else {
			printf("Imput inconnue\n");
		}
	}
	printf("congratulation\n");
	game_delete(g);

	return EXIT_SUCCESS;
}