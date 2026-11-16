#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

int test_dummy() { return EXIT_SUCCESS; }

//---------------TEST V1-------------------//

int test_game_fill_grass() {
	game g = game_default_solution();  // on crée une game g deja fini donc sans empty
	assert(g != NULL);				   // erreur allocation game g

	game g2;
	g2 = game_copy(g);	 // on copie cette game dans une autre game g2
	assert(g2 != NULL);	 // on vérifie l'allocation

	for (uint i = 0; i < DEFAULT_SIZE; ++i) {  // on remplace dans g2 toutes les cases GRASS par des cases EMPTY
		for (uint j = 0; j < DEFAULT_SIZE; ++j) {
			if (game_get_square(g2, i, j) == GRASS) {
				game_set_square(g2, i, j, EMPTY);
			}
		}
	}
	game_fill_grass(g2);  // on utilise la fonction pour remplacer toutes les cases EMPTY par de des cases GRASS

	// on regarde si la fonction renvoie le même résultat
	assert(game_equal(g, g2));

	// on supprime toutes les games
	game_delete(g);
	game_delete(g2);
	return EXIT_SUCCESS;
}

int test_game_won() {
	game g4 = game_default_solution();	// on crée une game g4 deja gagné
	assert(g4 != NULL);					// erreur allocation game g4

	game_set_square(g4, 1, 1, TREE);  // on change une case en arbre pour qu'il y ai un arbre sans tent et donc que la regle 4 sois fausse

	// si la partie affiche gagner c'est que la fonction n'a pas marché
	assert(!game_won(g4));

	game_delete(g4);

	game g5 = game_default_solution();	// on crée une game g5 deja gagné
	assert(g5 != NULL);					// erreur allocation game g5

	// on retire une tente pour qu'il n'y est plus le bon nombre de tente dans la ligne et la colonne
	for (uint i = 0; i < DEFAULT_SIZE; ++i) {
		for (uint j = 0; j < DEFAULT_SIZE; ++j) {
			if (game_get_square(g5, i, j) == TENT) {  // dès qu'une tent est trouvé on la remplace
				game_set_square(g5, i, j, EMPTY);
				goto tent_found;
			}
		}
	}
tent_found:
	assert(!game_won(g5));	// erreur il manque une tent

	game_delete(g5);
	return EXIT_SUCCESS;
}

int test_game_check_move() {
	game g = game_default();  // on crée une game g par défaut
	assert(g != NULL);		  // erreur allocation game g

	// on test de mettre une tent en dehors du plateau
	assert(!game_check_move(g, -1, 0, TENT));			 // erreur tent hors plateau
	assert(!game_check_move(g, 0, DEFAULT_SIZE, TENT));	 // erreur tent hors plateau

	game_set_square(g, 1, 1, TREE);	 // on ajoute un TREE et un BIGTREE
	game_set_square(g, 2, 2, BIGTREE);

	// on tent de changer la case TREE et BIGTREE par autre chose
	assert(!game_check_move(g, 1, 1, EMPTY));  // erreur on ne peut pas changer un TREE
	assert(!game_check_move(g, 2, 2, EMPTY));  // erreur on ne peut pas changé un BIGTREE

	// on test de poser un arbre
	assert(!game_check_move(g, 0, 0, TREE));	 // erreur on ne peut pas poser un TREE
	assert(!game_check_move(g, 0, 0, BIGTREE));	 // erreur on ne peut pas poser un BIGTREE

	// on test de poser une TENT avec un TREE à coté
	game_set_square(g, 0, 0, EMPTY);
	game_set_square(g, 0, 1, TREE);
	assert(game_check_move(g, 0, 0, TENT));	 // erreur TENT doit pouvoir être placé

	// on test de poser une case GRASS sur un EMPTY
	assert(game_check_move(g, 0, 0, GRASS));  // erreur GRASS doit pouvoir être placé

	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_play_move() {
	game g = game_default();  // on crée une game g par défaut
	assert(g != NULL);		  // erreur allocation game g
	game g2 = game_copy(g);	  // on copie game g dans game g2
	assert(g2 != NULL);		  // erreur allocation game g2

	// on met une TENT pour voir si les compteurs augmentes
	//  on met bien un arbres à coter et une case EMPTY pour la TENT
	game_set_square(g, 1, 1, TREE);
	game_set_square(g, 1, 2, EMPTY);
	// on regarde si les case a coter ne sont pas des TENT
	for (int i = -1; i <= 1; ++i)
		for (int j = -1; j <= 1; ++j)
			if (!(i == 0 && j == 0)) {
				int a = 1 + i;
				int b = 2 + j;
				if (a < DEFAULT_SIZE && b < DEFAULT_SIZE) game_set_square(g, a, b, EMPTY);
			}

	uint tent_ligne = game_get_current_nb_tents_row(g, 1);
	uint tent_colonne = game_get_current_nb_tents_col(g, 2);
	uint tent_tot = game_get_current_nb_tents_all(g);
	(void)tent_ligne;
	(void)tent_colonne;
	(void)tent_tot;

	game_play_move(g, 1, 2, TENT);

	// erreur game_play_move n'a pas placé une TENT sur (1,2)
	assert(game_get_square(g, 1, 2) == TENT);
	// erreur le compteur de TENT sur la ligne na pas augmenté
	assert(game_get_current_nb_tents_row(g, 1) == tent_ligne + 1);
	// erreur le compteur de TENT sur la colonne na pas augmenté
	assert(game_get_current_nb_tents_col(g, 2) == tent_colonne + 1);
	// erreur le compteur total de TENT na pas augmenté
	assert(game_get_current_nb_tents_all(g) == tent_tot + 1);

	// on pose de la GRASS et on vois si ca change le nombre de TENT
	game_set_square(g, 1, 3, EMPTY);
	tent_ligne = game_get_current_nb_tents_row(g, 1);
	tent_colonne = game_get_current_nb_tents_col(g, 3);
	tent_tot = game_get_current_nb_tents_all(g);

	game_play_move(g, 1, 3, GRASS);
	// erreur GRASS ne c'est pas placé
	assert(game_get_square(g, 1, 3) == GRASS);
	// erreur GRASS a modifié les compteurs de tent
	assert(game_get_current_nb_tents_row(g, 1) == tent_ligne && game_get_current_nb_tents_col(g, 3) == tent_colonne && game_get_current_nb_tents_all(g) == tent_tot);

	// on crée une tent en avec un arbre à coté
	game_set_square(g, 3, 2, TREE);
	game_set_square(g, 3, 3, EMPTY);
	game_play_move(g, 3, 3, TENT);
	// erreur la tent ne c'est pas placé
	assert(game_get_square(g, 3, 3) == TENT);

	// on place un arbre pour mettre une tent
	game_set_square(g, 3, 5, TREE);
	game_set_square(g, 3, 4, EMPTY);

	tent_ligne = game_get_current_nb_tents_row(g, 3);
	tent_colonne = game_get_current_nb_tents_col(g, 4);
	tent_tot = game_get_current_nb_tents_all(g);

	// on met la tent a coté de l'autre tent
	game_play_move(g, 3, 4, TENT);
	// erreur la tent ne c'est pas placé
	assert(game_get_square(g, 3, 4) == TENT);
	// erreur le compteurs na pas été mis à jour
	assert(game_get_current_nb_tents_row(g, 3) == tent_ligne + 1 && game_get_current_nb_tents_col(g, 4) == tent_colonne + 1 && game_get_current_nb_tents_all(g) == tent_tot + 1);

	// on delete les games
	game_delete(g2);
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_get_square() {
	game g = game_default();  // on crée une game g
	assert(g != NULL);		  // erreur allocation game g

	game_set_square(g, 1, 1, EMPTY);  // on pose toutes les case dont on va avoir besoin plus tard
	game_set_square(g, 1, 2, TREE);
	game_set_square(g, 1, 3, TENT);
	game_set_square(g, 1, 4, GRASS);
	game g2 = game_copy(g);
	assert(g2 != NULL);	 // erreur allocation game g2

	// on test chaque case que l'on a mise
	assert(game_get_square(g, 1, 1) == EMPTY);	// erreur game_get_square (1,1) est une case EMPTY
	assert(game_get_square(g, 1, 2) == TREE);	// erreur game_get_square (1,2) est une case TREE
	assert(game_get_square(g, 1, 3) == TENT);	// erreur game_get_square (1,3) est une case TENT
	assert(game_get_square(g, 1, 4) == GRASS);	// erreur game_get_square (1,4) est une case GRASS

	// on vérifie bien que game_get_square n'a pas modifier le jeu
	assert(game_equal(g, g2));	// erreur game_get_square a modifié le jeu

	game_delete(g);
	game_delete(g2);
	return EXIT_SUCCESS;
}

//---------------TEST V2-------------------//

int test_game_fill_grass_v2() {
	// creation d'une game avec des dimensions choisis
	uint nb_rows = 4;
	uint nb_cols = 7;
	bool wrapping = true;
	game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
	assert(g != NULL);	// erreur g, game_new_empty_ext est NULL

	// erreur game_new_empty_ext le wrapping devrait etre true
	assert(game_is_wrapping(g));
	// erreur game_new_empty_ext dimensions incorrectes
	assert(game_nb_rows(g) == nb_rows && game_nb_cols(g) == nb_cols);

	game_set_square(g, 0, 0, TREE);
	game_set_square(g, 1, 1, GRASS);

	game_fill_grass(g);
	for (uint i = 0; i < nb_rows; i++) {
		for (uint j = 0; j < nb_cols; j++) {
			// Il ne doit plus rester de cases vides
			assert(game_get_square(g, i, j) != EMPTY);
		}
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_won_v2() {
	// initialisation d'une game V2
	square squares[4];
	uint nb_tents_row[2];
	uint nb_tents_col[2];

	// REMPLACE init_game_array par des boucles locales (méthode de ton collègue)
	for (int i = 0; i < 4; i++) squares[i] = EMPTY;
	for (int i = 0; i < 2; i++) {
		nb_tents_row[i] = 0;
		nb_tents_col[i] = 0;
	}

	squares[0] = TREE;	// on mets un TREE à la case 0 et on met une tent attendu sur chaque case
	nb_tents_row[0] = 1;
	nb_tents_col[1] = 1;

	game g = game_new_ext(2, 2, squares, nb_tents_row, nb_tents_col, false);  // on crée la game g
	assert(g != NULL);														  // erreur allocation game g

	// on verifie que le jeu n'est pas gangé dès le debut
	assert(!game_won(g));  // erreur game_won revoie gagné alors qu'il est vide

	game_play_move(g, 0, 1, TENT);	// on joue le coup gangant
	game_fill_grass(g);				// On remplit le reste d'herbe

	// on regarde si ca renvoie bien gagné
	assert(game_won(g));  // erreur le jeu devrait être gagné

	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_play_move_v2() {
	// creation d'une game avec des dimensions choisis
	uint nb_rows = 3;
	uint nb_cols = 8;
	bool wrapping = true;
	game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
	assert(g != NULL);	// erreur g, game_new_empty_ext est NULL

	// erreur game_new_empty_ext le wrapping devrait etre true
	assert(game_is_wrapping(g));
	// erreur game_new_empty_ext dimensions incorrectes
	assert(game_nb_rows(g) == nb_rows && game_nb_cols(g) == nb_cols);

	game_play_move(g, 2, 7, TENT);	// on met une tent au bout de la ligne

	// erreur test_game_play_move le compteur de tent n'est pas bon
	assert(game_get_current_nb_tents_row(g, 2) == 1);

	game_play_move(g, 2, 7, GRASS);	 // on enleve la tent pour voir si le compteur diminue
	// erreur test_game_play_move_v2 le compteur n'a pas diminué
	assert(game_get_current_nb_tents_row(g, 2) == 0);

	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_get_square_v2() {
	// creation d'une game avec des dimensions choisis
	uint nb_rows = 5;
	uint nb_cols = 5;
	bool wrapping = false;
	game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
	assert(g != NULL);	// erreur g, game_new_empty_ext est NULL

	// erreur game_new_empty_ext le wrapping devrait etre false (ajusté selon ton paramètre)
	assert(game_is_wrapping(g) == wrapping);
	// erreur game_new_empty_ext dimensions incorrectes
	assert(game_nb_rows(g) == nb_rows && game_nb_cols(g) == nb_cols);

	game_set_square(g, 0, 0, BIGTREE);	// on place un BIGTREE

	// on regarde si game_get_square le voit bien
	assert(game_get_square(g, 0, 0) == BIGTREE);  // erreur test_game_get_square_v2 n'a pas reconnu la case BIGTREE

	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_load() {
	const char* filename = "test_txt";
	FILE* f = fopen(filename, "w");
	assert(f != NULL);

	fprintf(f, "2 3 1\n");
	fprintf(f, "1 1\n");
	fprintf(f, "1 1 1\n");
	fprintf(f, "X T E\n");
	fprintf(f, "G B E\n");
	fclose(f);

	game g = game_load((char*)filename);
	assert(g != NULL);

	assert(game_nb_rows(g) == 2 && game_nb_cols(g) == 3);
	assert(game_is_wrapping(g));

	assert(game_get_expected_nb_tents_row(g, 0) == 1 && game_get_expected_nb_tents_row(g, 1) == 1);
	assert(game_get_expected_nb_tents_col(g, 0) == 1 && game_get_expected_nb_tents_col(g, 1) == 1 && game_get_expected_nb_tents_col(g, 2) == 1);

	assert(game_get_square(g, 0, 0) == TREE);
	assert(game_get_square(g, 0, 1) == TENT);
	assert(game_get_square(g, 0, 2) == EMPTY);
	assert(game_get_square(g, 1, 0) == GRASS);
	assert(game_get_square(g, 1, 1) == BIGTREE);
	assert(game_get_square(g, 1, 2) == EMPTY);

	game_delete(g);
	remove(filename);
	return EXIT_SUCCESS;
}

int test_game_save() {
	const char* filename = "test.txt";

	uint nb_rows = 2;
	uint nb_cols = 3;
	square squares[6];
	uint nb_rows_expected[2];
	uint nb_cols_expected[3];

	for (int i = 0; i < 6; i++) squares[i] = EMPTY;
	squares[0] = TREE;
	squares[1] = TENT;
	squares[4] = BIGTREE;

	nb_rows_expected[0] = 1;
	nb_rows_expected[1] = 1;

	nb_cols_expected[0] = 1;
	nb_cols_expected[1] = 1;
	nb_cols_expected[2] = 0;

	game g = game_new_ext(nb_rows, nb_cols, squares, nb_rows_expected, nb_cols_expected, false);
	assert(g != NULL);
	game_save(g, (char*)filename);

	game g2 = game_load((char*)filename);
	assert(g2 != NULL);
	assert(game_equal(g, g2));

	game_delete(g);
	game_delete(g2);
	remove(filename);
	return EXIT_SUCCESS;
}


int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "erreur il faut au moins 1 parametre\n");
		return EXIT_FAILURE;
	}
	if (strcmp("dummy", argv[1]) == 0) {
		return test_dummy();
	} else if (strcmp("test_game_fill_grass", argv[1]) == 0) {
		return test_game_fill_grass();
	} else if (strcmp("test_game_won", argv[1]) == 0) {
		return test_game_won();
	} else if (strcmp("test_game_check_move", argv[1]) == 0) {
		return test_game_check_move();
	} else if (strcmp("test_game_play_move", argv[1]) == 0) {
		return test_game_play_move();
	} else if (strcmp("test_game_get_square", argv[1]) == 0) {
		return test_game_get_square();
		// test V2
	} else if (strcmp("test_game_fill_grass_v2", argv[1]) == 0) {
		return test_game_fill_grass_v2();
	} else if (strcmp("test_game_won_v2", argv[1]) == 0) {
		return test_game_won_v2();
	} else if (strcmp("test_game_play_move_v2", argv[1]) == 0) {
		return test_game_play_move_v2();
	} else if (strcmp("test_game_get_square_v2", argv[1]) == 0) {
		return test_game_get_square_v2();
	} else if (strcmp("test_game_load", argv[1]) == 0) {
		return test_game_load();
	} else if (strcmp("test_game_save", argv[1]) == 0) {
		return test_game_save();
	}
	fprintf(stderr, "erreur test inconnu\n");
	return EXIT_FAILURE;
}
