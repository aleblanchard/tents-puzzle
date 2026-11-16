#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"


// Fonctions utilitaires :

// vérifie qu'une game est bien initialisé vide
int game_is_empty(game g, int nb_rows, int nb_cols, bool is_wrap) {
	assert(g != NULL);

	// verifie les dimensions de la game
	assert(game_nb_rows(g) == nb_rows);
	assert(game_nb_cols(g) == nb_cols);
	assert(game_is_wrapping(g) == is_wrap);

	// verifie que le nombre de tent attendu est bien 0
	for (int i = 0; i < nb_cols; i++) {
		assert(game_get_expected_nb_tents_col(g, i) == 0);
	}
	for (int x = 0; x < nb_rows; x++) {
		assert(game_get_expected_nb_tents_row(g, x) == 0);

		// verifie que chaque case est bien empty
		for (int y = 0; y < nb_cols; y++) {
			assert(game_get_square(g, x, y) == EMPTY);
		}
	}
	return EXIT_SUCCESS;
}


// initialise la taille d'une grille vide
void init_game_array(square *squares, uint *nb_tents_row, uint *nb_tents_col, int nb_rows, int nb_cols) {
	for (int i = 0; i < nb_rows * nb_cols; i++) {
		squares[i] = EMPTY;	 // met chaque case de la grille à EMPTY
	}

	// met pour chaque case de la grille le nombre de tent attendu à 0
	for (int i = 0; i < nb_rows; i++) {
		nb_tents_row[i] = 0;
	}
	for (int i = 0; i < nb_cols; i++) {
		nb_tents_col[i] = 0;
	}
}


//---------------TEST V1-------------------//



int test_dummy() { return EXIT_SUCCESS; }

int test_game_new_empty() {
	game g = game_new_empty();
	assert(g != NULL);

	assert(game_is_empty(g, DEFAULT_SIZE, DEFAULT_SIZE, false) == EXIT_SUCCESS);

	game_delete(g);
	return EXIT_SUCCESS;
}


int test_game_new() {
	// initialisation d'une game
	square squares[DEFAULT_SIZE * DEFAULT_SIZE];
	uint nb_tents_row[DEFAULT_SIZE];
	uint nb_tents_col[DEFAULT_SIZE];
	init_game_array(squares, nb_tents_row, nb_tents_col, DEFAULT_SIZE, DEFAULT_SIZE);
	game g = game_new(squares, nb_tents_row, nb_tents_col);
	assert(g != NULL);


	// on place les tents et les tents attendu aux endroits voulu
	game_set_square(g, 0, 1, TREE);
	game_set_square(g, 2, 3, TREE);
	game_set_square(g, 1, 1, TENT);
	game_set_square(g, 2, 4, TENT);

	game_set_expected_nb_tents_row(g, 0, 1);
	game_set_expected_nb_tents_row(g, 1, 1);
	game_set_expected_nb_tents_row(g, 2, 1);

	game_set_expected_nb_tents_col(g, 1, 2);
	game_set_expected_nb_tents_col(g, 3, 1);
	game_set_expected_nb_tents_col(g, 4, 1);


	// on verifie que tout est bien au bon endroit et que rien d'autre à été modifié
	assert(game_get_square(g, 0, 1) == TREE);
	assert(game_get_square(g, 2, 3) == TREE);
	assert(game_get_square(g, 1, 1) == TENT);
	assert(game_get_square(g, 2, 4) == TENT);
	assert(game_get_square(g, 0, 0) == EMPTY);
	assert(game_get_square(g, 3, 3) == EMPTY);

	assert(game_get_expected_nb_tents_row(g, 0) == 1);
	assert(game_get_expected_nb_tents_row(g, 1) == 1);
	assert(game_get_expected_nb_tents_row(g, 2) == 1);
	assert(game_get_expected_nb_tents_row(g, 7) == 0);

	assert(game_get_expected_nb_tents_col(g, 1) == 2);
	assert(game_get_expected_nb_tents_col(g, 3) == 1);
	assert(game_get_expected_nb_tents_col(g, 4) == 1);
	assert(game_get_expected_nb_tents_col(g, 0) == 0);

	game_delete(g);
	return EXIT_SUCCESS;
}


int test_game_equal() {
	game g1 = game_default();
	game g2 = game_default();

	assert(g1 != NULL);
	assert(g2 != NULL);

	assert(game_equal(g1, g2) == true);	 // verifie qu'une game_default est bien égale à une game_default

	game_set_square(g2, 0, 1, TENT);
	game_set_square(g2, 1, 0, TENT);
	assert(game_equal(g1, g2) == false);  // vérifie qu'apres avoir placé des TENT sur g2, g1 et g2 ne sont plus égaux

	game_set_square(g1, 0, 1, TENT);
	game_set_square(g1, 1, 0, TENT);
	assert(game_equal(g1, g2) == true);	 // vérifie qu'apres avoir placé les memes TENT sur g1 que sur g2, g1 et g2 sont de nouveaux égaux

	game_set_expected_nb_tents_row(g2, 0, 5);
	assert(game_equal(g1, g2) == false);  // vérifie qu'apres avoir placé des tent attendu sur ligne (0,5) sur g2, g1 et g2 ne sont plus égaux

	game_set_expected_nb_tents_row(g1, 0, 5);
	assert(game_equal(g1, g2) == true);	 // vérifie qu'apres avoir placé les memes tent attendu sur g1 que sur g2, g1 et
										 // g2 sont de nouveaux égaux

	game_set_expected_nb_tents_col(g2, 3, 7);
	assert(game_equal(g1, g2) == false);  // vérifie qu'apres avoir placé des tent attendu sur colonne (3,7) sur g2, g1
										  // et g2 ne sont plus égaux

	game_set_expected_nb_tents_col(g1, 3, 7);
	assert(game_equal(g1, g2) == true);	 // vérifie qu'apres avoir placé les memes tent attendu sur g1 que sur g2, g1 et
										 // g2 sont de nouveaux égaux

	game_delete(g1);
	game_delete(g2);
	return EXIT_SUCCESS;
}

int test_game_copy() {
	game g1 = game_default();
	assert(g1 != NULL);
	game g2 = game_copy(g1);
	assert(g2 != NULL);

	// verifie que la copie g2 de game_default g1 à bien marché; (game_default à un TREE sur 0,0)
	assert(game_get_square(g2, 0, 0) == TREE);

	assert(game_equal(g1, g2) == true);	 // verifie que les jeux sont bien égaux apres une copie

	game_set_square(g2, 0, 1, TENT);
	game_set_square(g2, 1, 0, TENT);
	assert(game_equal(g1, g2) == false);  // verifie qu'apres la modification d'une copie (g2) cela ne modifie pas la source (g1)

	game_set_expected_nb_tents_row(g1, 2, 4);
	game_delete(g2);
	g2 = game_copy(g1);
	assert(game_get_expected_nb_tents_row(g2, 2) == 4);	 // verifie que game_copy copy bien les expected_nb_tent_row

	game_set_expected_nb_tents_row(g1, 2, 9);
	assert(game_get_expected_nb_tents_row(g2, 2) != 9);	 // verifie que apres modification de tent attendu sur g1 la copie g2 ne change pas

	game_delete(g1);
	game_delete(g2);
	return EXIT_SUCCESS;
}


int test_game_set_square() {
	game g = game_new_empty();
	assert(g != NULL);

	game_set_square(g, 0, 1, TREE);
	game_set_square(g, 1, 4, TREE);
	// verifie que le placement des TREE à réussi
	assert(game_get_square(g, 0, 1) == TREE);
	assert(game_get_square(g, 1, 4) == TREE);

	// verifie que game_set_square ne modifie aucune autres cases
	assert(game_get_square(g, 0, 0) == EMPTY);
	assert(game_get_square(g, 2, 3) == EMPTY);

	game_set_square(g, 0, 1, TENT);
	game_set_square(g, 1, 4, TENT);
	// place des TENT au meme endroit que les TREE placé juste avant pour voir si cela marche
	assert(game_get_square(g, 0, 1) == TENT);
	assert(game_get_square(g, 1, 4) == TENT);

	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_delete() {
	game g1 = game_default();
	assert(g1 != NULL);

	game g3 = game_copy(g1);
	game_delete(g3);  // verifie si on peux supprimer une copie
	game_delete(g1);

	game g2 = game_default();
	assert(g2 != NULL);
	game_set_square(g2, 0, 1, TENT);
	game_set_square(g2, 2, 3, TENT);
	game_delete(g2);  // verifie si on peux supprimer une game_default apres y avoir placé des TENT

	// initialisation d'une game, pour essayer de la delete
	square squares[DEFAULT_SIZE * DEFAULT_SIZE];
	uint nb_tents_row[DEFAULT_SIZE];
	uint nb_tents_col[DEFAULT_SIZE];
	init_game_array(squares, nb_tents_row, nb_tents_col, DEFAULT_SIZE, DEFAULT_SIZE);

	game g4 = game_new(squares, nb_tents_row, nb_tents_col);
	assert(g4 != NULL);
	game_delete(g4);
	return EXIT_SUCCESS;
}




//---------------TEST V2-------------------//


int test_game_new_empty_v2() {
	// creation d'une game avec des dimensions choisis
	uint nb_rows = 5;
	uint nb_cols = 4;
	bool wrapping = true;
	game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
	assert(g != NULL);

	assert(game_is_wrapping(g) == true);
	assert(game_nb_rows(g) == nb_rows);
	assert(game_nb_cols(g) == nb_cols);

	assert(game_is_empty(g, nb_rows, nb_cols, wrapping) == EXIT_SUCCESS);

	game_delete(g);
	return EXIT_SUCCESS;
}



int test_game_new_ext_v2() {
	// initialisation d'une game v2
	square squares[4];
	uint nb_tents_row[2];
	uint nb_tents_col[2];
	init_game_array(squares, nb_tents_row, nb_tents_col, 2, 2);

	squares[1] = BIGTREE;  // on place un BIGTREE à la case 1 et on met une tent attendu sur chaque case
	for (int i = 0; i < 2; i++) {
		nb_tents_row[i] = 1;
		nb_tents_col[i] = 1;
	}

	game g = game_new_ext(2, 2, squares, nb_tents_row, nb_tents_col,
						  true);  // creer le jeu avec les modification apporté et avec wrapping true
	assert(g != NULL);
	assert(game_get_square(g, 0, 1) == BIGTREE);		// verifie que le BIGTREE à bien était placé
	assert(game_get_expected_nb_tents_row(g, 0) == 1);	// verifie le nombre de tent attendu

	// modification du tableau local tent attendu
	nb_tents_row[0] = 10;
	nb_tents_col[0] = 10;
	// verifie que le tableau du jeu n'a pas était modifié apres modification du tableau local
	assert(game_get_expected_nb_tents_row(g, 0) != 10);
	assert(game_get_expected_nb_tents_col(g, 0) != 10);

	game_delete(g);
	return EXIT_SUCCESS;
}



int test_game_equal_v2() {
	game g1 = game_new_empty_ext(4, 5, true);
	game g2 = game_new_empty_ext(4, 5, true);
	assert(g1 != NULL);
	assert(g2 != NULL);
	assert(game_equal(g1, g2) == true);

	game g4 = game_new_empty_ext(3, 5, true);
	assert(game_equal(g1, g4) == false);  // verifie si une game de taille differente avec une autre est égal
	game_delete(g4);

	game g3 = game_new_empty_ext(4, 5, false);
	assert(game_equal(g1, g3) == false);  // verifie si une game de taille égal avec une autre game mais avec wrapping
										  // true et l'autre wrapping off est égal
	game_delete(g3);

	game_set_square(g2, 0, 1, TENT);
	game_set_square(g2, 1, 0, TENT);
	assert(game_equal(g1, g2) == false);  // verifi que apres modification de g2 game_equal detecte que g1 et g2 ne sont pas egaux

	game_set_square(g1, 0, 1, TENT);
	game_set_square(g1, 1, 0, TENT);
	assert(game_equal(g1, g2) == true);	 // verifie que apres modification de g1 game_equal detecte que g1 et g2 sont de nouveau egaux

	game_delete(g1);
	game_delete(g2);
	return EXIT_SUCCESS;
}

int test_game_copy_v2() {
	game g1 = game_new_empty_ext(4, 5, true);
	assert(g1 != NULL);
	game_set_expected_nb_tents_row(g1, 0, 2);

	game g2 = game_copy(g1);
	assert(g2 != NULL);
	assert(game_is_wrapping(g2) == true);  // verifie que game_copy copy bien le wrapping
	assert(game_equal(g1, g2) == true);	   // verifie que les games g1 et g2 sont bien egal apres une copie

	game_set_expected_nb_tents_row(g1, 0, 5);
	assert(game_get_expected_nb_tents_row(g2, 0) != 5);	 // verifie que apres modification de la source g1 la copie g2 n'est pas modifié

	game_delete(g1);
	game_delete(g2);
	return EXIT_SUCCESS;
}

int test_game_set_square_v2() {
	game g = game_new_empty_ext(10, 10, false);
	assert(g != NULL);
	game_set_square(g, 9, 9, BIGTREE);
	assert(game_get_square(g, 9, 9) == BIGTREE);

	game_set_square(g, 0, 1, TREE);
	game_set_square(g, 1, 4, TREE);
	assert(game_get_square(g, 0, 1) == TREE);
	assert(game_get_square(g, 1, 4) == TREE);

	// verifie que case aux hasard autre que celle modifié est était modifié
	assert(game_get_square(g, 0, 0) == EMPTY);
	assert(game_get_square(g, 2, 3) == EMPTY);

	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_delete_v2() {
	game g1 = game_new_empty_ext(4, 5, true);
	assert(g1 != NULL);
	game_set_square(g1, 0, 1, TREE);
	game g3 = game_copy(g1);  // verifie qu'on peut supprimer une copie de game_new_empty_ext
	game_delete(g3);
	game_delete(g1);

	// initialisation d'une game pour essayer de la supprimer
	square squares[4];
	uint nb_tents_row[2];
	uint nb_tents_col[2];
	init_game_array(squares, nb_tents_row, nb_tents_col, 2, 2);
	game g4 = game_new_ext(2, 2, squares, nb_tents_row, nb_tents_col, false);
	assert(g4 != NULL);
	game_delete(g4);
	return EXIT_SUCCESS;
}



int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "erreur il faut au moins 1 parametre\n");
		return EXIT_FAILURE;
	}
	if (strcmp("dummy", argv[1]) == 0) {
		return test_dummy();
		// Tests V1
	} else if (strcmp("test_game_new_empty", argv[1]) == 0) {
		return test_game_new_empty();
	} else if (strcmp("test_game_equal", argv[1]) == 0) {
		return test_game_equal();
	} else if (strcmp("test_game_copy", argv[1]) == 0) {
		return test_game_copy();
	} else if (strcmp("test_game_set_square", argv[1]) == 0) {
		return test_game_set_square();
	} else if (strcmp("test_game_delete", argv[1]) == 0) {
		return test_game_delete();
	} else if (strcmp("test_game_new", argv[1]) == 0) {
		return test_game_new();
	}

	// Tests v2
	else if (strcmp("test_game_new_empty_v2", argv[1]) == 0) {
		return test_game_new_empty_v2();
	} else if (strcmp("test_game_equal_v2", argv[1]) == 0) {
		return test_game_equal_v2();
	} else if (strcmp("test_game_copy_v2", argv[1]) == 0) {
		return test_game_copy_v2();
	} else if (strcmp("test_game_set_square_v2", argv[1]) == 0) {
		return test_game_set_square_v2();
	} else if (strcmp("test_game_delete_v2", argv[1]) == 0) {
		return test_game_delete_v2();
	} else if (strcmp("test_game_new_ext_v2", argv[1]) == 0) {
		return test_game_new_ext_v2();
	}
	fprintf(stderr, "tu dois utiliser un parametre connu comme 'dummy' ou le nom d'une fonction a test\n");
	return EXIT_FAILURE;
}