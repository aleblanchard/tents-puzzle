#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"


int test_game_play_move_complementaire(void) {	// le test de gauthier ne teste pas tous les cas (bigtree, jouer meme valeur)
	game g = game_new_empty_ext(4, 4, false);
	if (!g) {
		fprintf(stderr, "game_new_empty_ext retourne nulln\n");
		return EXIT_FAILURE;
	}

	game_set_square(g, 0, 0, TENT);
	game_play_move(g, 0, 0, TENT);	// meme valeur
	if (game_get_square(g, 0, 0) != TENT) {
		fprintf(stderr, "game_play_move a changé une case deja du meme type\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_square(g, 1, 1, TREE);
	game_play_move(g, 1, 1, TENT);	// on essaye de remplacer sur un TREE (pas possible)
	if (game_get_square(g, 1, 1) != TREE) {
		fprintf(stderr, "game_play_move ne devrait pas remplacer un TREE\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_square(g, 2, 2, BIGTREE);
	game_play_move(g, 2, 2, TENT);	// essaye de remplacer un BIGTREE (pas possible)
	if (game_get_square(g, 2, 2) != BIGTREE) {
		fprintf(stderr, "game_play_move ne devrait pas remplacer un BIGTREE\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_play_move(g, 3, 3, TREE);	// placer un TREE (pas possible)
	if (game_get_square(g, 3, 3) == TREE) {
		fprintf(stderr, "game_play_move n'a pas le droit de placer un TREE\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_play_move(g, 3, 3, BIGTREE);  // placer un BIGTREE (pas possible)
	if (game_get_square(g, 3, 3) == BIGTREE) {
		fprintf(stderr, "game_play_move n'a pas le droit de placer un BIGTREE\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_game_nb_neigh_wrapping(void) {
	game g = game_new_empty_ext(4, 4, true);
	if (!g) {
		fprintf(stderr, "game_new_empty_ext retourne nulln\n");
		return EXIT_FAILURE;
	}

	game_set_square(g, 3, 0, TENT);
	if (game_nb_neigh(g, 0, 0, TENT, false) != 1) {	 // on devrait trouver TENT qui wrappe en bas en 3,0 collé a 0,0
		fprintf(stderr, "game_nb_neigh avec wrapping devrait trouver 1\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_square(g, 0, 3, TENT);
	if (game_nb_neigh(g, 0, 0, TENT, false) != 2) {	 // on devrait trouver l'autre TENT qui wrappe en bas en 0,3 collé a 0,0
		fprintf(stderr, "game_nb_neigh avec wrapping devrait trouver 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_game_nb_neigh(void) {	// test de la fonction game_nb_neigh
	game g = game_new_empty_ext(4, 4, false);
	game_set_square(g, 0, 1, TENT);	 // on place trois tentes autour de la case 1,1
	game_set_square(g, 1, 2, TENT);
	game_set_square(g, 2, 1, TENT);
	if (game_nb_neigh(g, 1, 1, TENT, false) != 3) {
		fprintf(stderr, "game_nb_neigh ne calcule pas correctement\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_dummy() { return EXIT_SUCCESS; }

int test_undo() {
	game g = game_new_empty_ext(4, 4, false);
	if (!g) return EXIT_FAILURE;
	game_undo(g);

	game_set_square(g, 0, 0, TENT);
	game_set_square(g, 0, 1, TENT);
	game_set_square(g, 0, 2, TENT);

	game_undo(g);
	if (game_get_square(g, 0, 2) != EMPTY || game_get_square(g, 0, 1) != TENT) {
		fprintf(stderr, "undo marche pô\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	// on vérifie que les autres cases n'ont pas été modifiées
	for (uint i = 0; i < 4; i++) {
		for (uint j = 0; j < 4; j++) {
			if (i == 0 && (j == 0 || j == 1)) continue;	 // doivent être TENT
			if (game_get_square(g, i, j) != EMPTY) {
				fprintf(stderr, "undo a modifié case (%u,%u)\n", i, j);
				game_delete(g);
				return EXIT_FAILURE;
			}
		}
	}

	game_undo(g);
	if (game_get_square(g, 0, 1) != EMPTY) {
		fprintf(stderr, "undo successifs marchent pas\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_undo(g);
	if (game_get_square(g, 0, 0) != EMPTY) {
		fprintf(stderr, "bug\n");
		game_delete(g);
		return EXIT_FAILURE;
	}

	game_set_square(g, 2, 3, GRASS);  // test sur plusieurs positions pour détecter bug coordonnées
	if (game_get_square(g, 2, 3) != GRASS) {
		fprintf(stderr, "set square erreur\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_undo(g);
	if (game_get_square(g, 2, 3) != EMPTY) {
		fprintf(stderr, "undo erreur a (2,3)\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	// verifier que toutes les autres cases sont toujours EMPTY
	for (uint i = 0; i < 4; i++) {
		for (uint j = 0; j < 4; j++) {
			if (game_get_square(g, i, j) != EMPTY) {
				fprintf(stderr, "case (%u,%u) != EMPTY apres undo\n", i, j);
				game_delete(g);
				return EXIT_FAILURE;
			}
		}
	}

	game_delete(g);
	return EXIT_SUCCESS;
}

int test_redo() {
	game g = game_new_empty_ext(4, 4, false);
	if (!g) return EXIT_FAILURE;
	game_redo(g);

	game_set_square(g, 0, 0, TENT);
	game_undo(g);
	game_redo(g);
	if (game_get_square(g, 0, 0) != TENT) {
		fprintf(stderr, "redo marche pas\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	// on verifie que les autres cases n'ont pas été modifiées
	for (uint i = 0; i < 4; i++) {
		for (uint j = 0; j < 4; j++) {
			if (i == 0 && j == 0) continue;	 // doit etre TENT
			if (game_get_square(g, i, j) != EMPTY) {
				fprintf(stderr, "redo a modifié case (%u,%u)\n", i, j);
				game_delete(g);
				return EXIT_FAILURE;
			}
		}
	}

	game_undo(g);
	game_set_square(g, 0, 0, GRASS);
	game_redo(g);
	if (game_get_square(g, 0, 0) == TENT) {
		fprintf(stderr, "redo n'a pas été effacé après un nouveau coup\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_square(g, 0, 0) != GRASS) {
		fprintf(stderr, "bug\n");
		game_delete(g);
		return EXIT_FAILURE;
	}

	// test sur position différente pour détecter bug coordonnées
	game_set_square(g, 3, 2, TENT);
	game_undo(g);
	if (game_get_square(g, 3, 2) != EMPTY) {
		fprintf(stderr, "undo erreur at (3,2)\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_redo(g);
	if (game_get_square(g, 3, 2) != TENT) {
		fprintf(stderr, "redo erreur at (3,2)\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_square(g, 0, 0) != GRASS) {
		fprintf(stderr, "redo a modifié case (0,0)\n");
		game_delete(g);
		return EXIT_FAILURE;
	}

	game_delete(g);
	g = game_new_empty_ext(4, 4, false);
	game_set_square(g, 1, 1, TENT);
	game_set_square(g, 1, 2, GRASS);
	game_set_square(g, 2, 1, TENT);
	game_undo(g);  // annule (2,1)=TENT
	game_undo(g);  // annule (1,2)= GRASS
	game_undo(g);  // annule (1,1)=TENT
	game_redo(g);  // refait (1,1)=TENT
	game_redo(g);  // refait (1,2)= GRASS
	game_redo(g);  // refait (2,1)= TENT
	if (game_get_square(g, 1, 1) != TENT || game_get_square(g, 1, 2) != GRASS || game_get_square(g, 2, 1) != TENT) {
		fprintf(stderr, "redo multiples erreur\n");
		game_delete(g);
		return EXIT_FAILURE;
	}

	game_delete(g);
	return EXIT_SUCCESS;
}


int test_bords_wrapping(void) {
	game g = game_new_empty_ext(4, 4, true);
	if (!g) return EXIT_FAILURE;

	uint a, b;
	if (!game_get_adjacent_square(g, 0, 0, UP, &a, &b) || a != 3 || b != 0) {
		fprintf(stderr, "wrapping UP marche pas (on a eu %u %u)\n", a, b);
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (!game_get_adjacent_square(g, 0, 0, LEFT, &a, &b) || a != 0 || b != 3) {
		fprintf(stderr, "wrapping UP marche pas (on a eu %u %u)\n", a, b);
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (!game_get_adjacent_square(g, 3, 0, DOWN, &a, &b) || a != 0 || b != 0) {
		fprintf(stderr, "wrapping UP marche pas (on a eu %u %u)\n", a, b);
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (!game_get_adjacent_square(g, 0, 3, RIGHT, &a, &b) || a != 0 || b != 0) {
		fprintf(stderr, "wrapping UP marche pas (on a eu %u %u)\n", a, b);
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_total(void) {
	game g = game_new_empty_ext(4, 4, false);
	if (!g) return EXIT_FAILURE;
	game_set_expected_nb_tents_row(g, 0, 0);
	game_set_square(g, 0, 0, TREE);		//=1
	game_set_square(g, 0, 1, BIGTREE);	//=2
	if (game_get_expected_nb_tents_all(g) != 3) {
		fprintf(stderr, "total attendu incorrect\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}



int test_ext(void) {
	game g = game_new_empty_ext(4, 6, true);
	if (!g) return EXIT_FAILURE;
	if (game_nb_rows(g) != 4) {
		fprintf(stderr, "Erreur nb rows pas a 4\n mais a %u\n", game_nb_rows(g));
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_nb_cols(g) != 6) {
		fprintf(stderr, "Erreur nb cols pas a 6\n mais a %u\n", game_nb_cols(g));
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (!game_is_wrapping(g)) {
		fprintf(stderr, "Erreur le wrapping devrait etre a true\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_undo_redo(void) {
	game g = game_new_empty_ext(8, 8, false);
	game_set_square(g, 0, 0, TENT);
	if (game_get_square(g, 0, 0) != TENT) {
		fprintf(stderr, "Erreur case 0 0 n'est pas TENT\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_undo(g);  // doit redevenir EMPTY;
	if (game_get_square(g, 0, 0) != EMPTY) {
		fprintf(stderr, "Erreur case 0 0 devrait etre EMPTY apres undo\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_redo(g);  // doit redevenir TENT;
	if (game_get_square(g, 0, 0) != TENT) {
		fprintf(stderr, "Erreur case 0 0 devrait etre TENT apres redo\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_undo_ecrase(void) {
	game g = game_new_empty_ext(8, 8, false);
	game_set_square(g, 0, 0, TENT);
	game_undo(g);
	game_set_square(g, 0, 0, GRASS);
	game_redo(g);
	if (game_get_square(g, 0, 0) != GRASS) {
		fprintf(stderr, "Erreur redo ne devrait pas marcher après un nouveau coup\n");
		fprintf(stderr, "Res = %d\n (attendu GRASS)\n", game_get_square(g, 0, 0));
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}
/* -----------------------------------------------------TEST DE game_set_expected_nb_tents_row(game g, uint i, uint
 * nb_tents)---------------------------------*/
int test_ecriture(void) { /*on écrit sur la ligne 0 et on verifie que c'est pris en compte */
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}
	game_set_expected_nb_tents_row(g, 0, 2);
	if (game_get_expected_nb_tents_row(g, 0) != 2) {
		fprintf(stderr, "row 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}
int test_effet_de_bord(void) { /*change ligne 0 et test si ligne 1 a ete affectée par le changement*/
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}
	unsigned int row1_avant = game_get_expected_nb_tents_row(g, 1);
	game_set_expected_nb_tents_row(g, 0, 3);

	if (game_get_expected_nb_tents_row(g, 1) != row1_avant) {
		fprintf(stderr, "row 1 a été modifié par erreur\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_reecriture(void) { /*reecrit sur une ligne et on teste que la derniere valeur est bien lu (lue?)*/
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_expected_nb_tents_row(g, 0, 2);
	if (game_get_expected_nb_tents_row(g, 0) != 2) {
		fprintf(stderr, "après set a 2, row 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_expected_nb_tents_row(g, 0, 3);
	if (game_get_expected_nb_tents_row(g, 0) != 3) {
		fprintf(stderr, "après reecriture a 3, row 0 != 3\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}
int test_reecriture_meme_valeur(void) { /*pareil avec la meme valeur (rien doit changer)*/
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_expected_nb_tents_row(g, 0, 2);
	if (game_get_expected_nb_tents_row(g, 0) != 2) {
		fprintf(stderr, "après set a 2, row 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_expected_nb_tents_row(g, 0, 2);
	if (game_get_expected_nb_tents_row(g, 0) != 2) {
		fprintf(stderr, "après reecriture a 2, row 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_bornes(void) { /*test ecrit au debut et a la fin*/
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}
	unsigned int dernier = game_nb_rows(g) - 1;

	game_set_expected_nb_tents_row(g, 0, 1);
	game_set_expected_nb_tents_row(g, dernier, 3);

	if (game_get_expected_nb_tents_row(g, 0) != 1) {
		fprintf(stderr, "row 0 devrait valoir 1\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_row(g, dernier) != 3) {
		fprintf(stderr, "row %u devrait valoir 3\n", dernier);
		game_delete(g);
		return EXIT_FAILURE;
	}

	game_delete(g);
	return EXIT_SUCCESS;
}


int test_miseajour(void) {
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_expected_nb_tents_row(g, 0, 2);
	unsigned int cptrows = 0, cptcols = 0;
	for (unsigned int i = 0; i < game_nb_rows(g); i++) cptrows += game_get_expected_nb_tents_row(g, i);
	for (unsigned int j = 0; j < game_nb_cols(g); j++) cptcols += game_get_expected_nb_tents_col(g, j);

	unsigned int all = game_get_expected_nb_tents_all(g);

	if (all != cptrows && all != cptcols) {
		fprintf(stderr, "all=%u (somme lignes=%u, somme colonnes=%u)\n", all, cptrows, cptcols);
		game_delete(g);
		return EXIT_FAILURE;
	}

	game_delete(g);
	return EXIT_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/


/* ----------------------------------------------------TEST DE game_set_expected_nb_tents_col(game g, uint i, uint
 * nb_tents)----------------------------*/
int test_col_ecriture(void) { /*on écrit sur la ligne 0 et on verifie que c'est pris en compte */
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}
	game_set_expected_nb_tents_col(g, 0, 2);
	if (game_get_expected_nb_tents_col(g, 0) != 2) {
		fprintf(stderr, "col 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_col_effet_de_bord(void) { /*change ligne 0 et test si ligne 1 a ete affectée par le changement*/
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}
	unsigned int col1_avant = game_get_expected_nb_tents_col(g, 1);
	game_set_expected_nb_tents_col(g, 0, 3);

	if (game_get_expected_nb_tents_col(g, 1) != col1_avant) {
		fprintf(stderr, "col 1 a été modifié par erreur\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_col_reecriture(void) { /*reecrit sur une ligne et on teste que la derniere valeur est bien lu (lue?)*/
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_expected_nb_tents_col(g, 0, 2);
	if (game_get_expected_nb_tents_col(g, 0) != 2) {
		fprintf(stderr, "après set a 2, col 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_expected_nb_tents_col(g, 0, 3);
	if (game_get_expected_nb_tents_col(g, 0) != 3) {
		fprintf(stderr, "après reecriture a 3, col 0 != 3\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_col_reecriture_meme_valeur(void) { /*pareil avec la meme valeur (rien doit changer)*/
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_expected_nb_tents_col(g, 0, 2);
	if (game_get_expected_nb_tents_col(g, 0) != 2) {
		fprintf(stderr, "après set a 2, col 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_expected_nb_tents_col(g, 0, 2);
	if (game_get_expected_nb_tents_col(g, 0) != 2) {
		fprintf(stderr, "après reecriture a 2, col 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_col_bornes(void) { /*test ecrit au debut et a la fin*/
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}
	unsigned int dernier = game_nb_cols(g) - 1;

	game_set_expected_nb_tents_col(g, 0, 1);
	game_set_expected_nb_tents_col(g, dernier, 3);

	if (game_get_expected_nb_tents_col(g, 0) != 1) {
		fprintf(stderr, "col 0 devrait valoir 1\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_col(g, dernier) != 3) {
		fprintf(stderr, "col %u devrait valoir 3\n", dernier);
		game_delete(g);
		return EXIT_FAILURE;
	}

	game_delete(g);
	return EXIT_SUCCESS;
}



int test_col_miseajour(void) {
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	unsigned int all_before = game_get_expected_nb_tents_all(g);
	game_set_expected_nb_tents_col(g, 0, 2);

	if (game_get_expected_nb_tents_col(g, 0) != 2) {
		fprintf(stderr, "col 0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_all(g) != all_before) {
		fprintf(stderr, "all a changé apres modif d'une col\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/




/************************************************************************test de
 * game_get_expected_nb_tents_col************************************* */

int test_row_vide(void) {
	game g = game_new_empty(); /*tout a zero*/
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	for (unsigned int i = 0; i < game_nb_rows(g); i++) {
		if (game_get_expected_nb_tents_row(g, i) != 0) {
			fprintf(stderr, "row %u devrait valoir 0\n", i);
			game_delete(g);
			return EXIT_FAILURE;
		}
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_apres_set(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_expected_nb_tents_row(g, 0, 2);
	game_set_expected_nb_tents_row(g, 1, 1);
	game_set_expected_nb_tents_row(g, 3, 3);

	if (game_get_expected_nb_tents_row(g, 0) != 2) {
		fprintf(stderr, "row0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_row(g, 1) != 1) {
		fprintf(stderr, "row1 != 1\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_row(g, 3) != 3) {
		fprintf(stderr, "row3 != 3\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_get_row_bornes(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	unsigned int dernier = game_nb_rows(g) - 1;
	game_set_expected_nb_tents_row(g, 0, 4);
	game_set_expected_nb_tents_row(g, dernier, 2);
	if (game_get_expected_nb_tents_row(g, 0) != 4) {
		fprintf(stderr, "row 0 != 4\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_row(g, dernier) != 2) {
		fprintf(stderr, "row %u != 2\n", dernier);
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}




/* *****************************************************************************************************************************************************************/



/*----------------------------------------------------------------------------------------------------------test de
 * game_get_expected_nb_tents_col-----------------------------*/

int test_col_vide(void) {
	game g = game_new_empty(); /*tout a zero*/
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	for (unsigned int i = 0; i < game_nb_cols(g); i++) {
		if (game_get_expected_nb_tents_col(g, i) != 0) {
			fprintf(stderr, "col %u devrait valoir 0\n", i);
			game_delete(g);
			return EXIT_FAILURE;
		}
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_apres_set_col(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_expected_nb_tents_col(g, 0, 2);
	game_set_expected_nb_tents_col(g, 1, 1);
	game_set_expected_nb_tents_col(g, 3, 3);

	if (game_get_expected_nb_tents_col(g, 0) != 2) {
		fprintf(stderr, "col0 != 2\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_col(g, 1) != 1) {
		fprintf(stderr, "col1 != 1\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_col(g, 3) != 3) {
		fprintf(stderr, "col3 != 3\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_get_col_bornes(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	unsigned int dernier = game_nb_cols(g) - 1;
	game_set_expected_nb_tents_col(g, 0, 4);
	game_set_expected_nb_tents_col(g, dernier, 2);
	if (game_get_expected_nb_tents_col(g, 0) != 4) {
		fprintf(stderr, "col 0 != 4\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_expected_nb_tents_col(g, dernier) != 2) {
		fprintf(stderr, "col %u != 2\n", dernier);
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++test de
 * game_get_expected_nb_tents_all+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int test_all_empty(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	if (game_get_expected_nb_tents_all(g) != 0) {
		fprintf(stderr, "expected_all devrait etre vide\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_all_apres_set(void) {
	game g = game_default();
	if (!g) {
		fprintf(stderr, "game_default() retourne NULL\n");
		return EXIT_FAILURE;
	}

	unsigned int row = 0, col = 0;
	for (unsigned int i = 0; i < game_nb_rows(g); i++) row += game_get_expected_nb_tents_row(g, i);
	for (unsigned int j = 0; j < game_nb_cols(g); j++) col += game_get_expected_nb_tents_col(g, j);

	unsigned int total = game_get_expected_nb_tents_all(g);

	if (total != row || total != col) {
		fprintf(stderr, "total = %u rows = %u cols = %u \n", total, row, col);
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*										TEST DE GAME_GET_CURRENT_NB_TENTS_ROW					*/
int test_current_row_vide(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	for (unsigned int i = 0; i < game_nb_rows(g); i++) {
		if (game_get_current_nb_tents_row(g, i) != 0) {
			fprintf(stderr, "row %u devrait compter 0 tentes", i);
			game_delete(g);
			return EXIT_FAILURE;
		}
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_current_row_apres_set(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_square(g, 0, 0, TENT);
	game_set_square(g, 0, 2, TENT);
	game_set_square(g, 0, 3, TENT);
	game_set_square(g, 0, 1, GRASS);
	game_set_square(g, 0, 4, EMPTY);

	if (game_get_current_nb_tents_row(g, 0) != 3) {
		fprintf(stderr, "row 0 devrait avoir 3 tentes\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_current_nb_tents_row(g, 1) != 0) {
		fprintf(stderr, "row 1 devrait avoir 0 tentes");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}




int test_get_current_row_bornes(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}
	unsigned int dernier = game_nb_cols(g) - 1;

	game_set_square(g, 3, 0, TENT);
	game_set_square(g, 3, dernier, TENT);

	if (game_get_current_nb_tents_row(g, 3) != 2) {
		fprintf(stderr, "row 3 devrait avoir 2 tentes\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_current_row_effet_de_bord(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	unsigned int avant = game_get_current_nb_tents_row(g, 1);

	game_set_square(g, 0, 0, TENT);
	game_set_square(g, 0, 1, TENT);
	game_set_square(g, 0, 2, EMPTY);
	game_set_square(g, 0, 3, GRASS);

	unsigned int apres = game_get_current_nb_tents_row(g, 1);
	if (apres != avant) {
		fprintf(stderr, "row 1 a ete modifé par erreur (avant = %u apres = %u)\n", avant, apres);
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*=================================================TEST game_get_current_nb_col====================================*/

int test_current_col_vide(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	for (unsigned int i = 0; i < game_nb_cols(g); i++) {
		if (game_get_current_nb_tents_col(g, i) != 0) {
			fprintf(stderr, "col %u devrait compter 0 tentes", i);
			game_delete(g);
			return EXIT_FAILURE;
		}
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_current_col_apres_set(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_square(g, 0, 0, TENT);
	game_set_square(g, 2, 0, TENT);
	game_set_square(g, 3, 0, TENT);
	game_set_square(g, 1, 0, GRASS);
	game_set_square(g, 4, 0, EMPTY);

	if (game_get_current_nb_tents_col(g, 0) != 3) {
		fprintf(stderr, "col 0 devrait avoir 3 tentes\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (game_get_current_nb_tents_col(g, 1) != 0) {
		fprintf(stderr, "col 1 devrait avoir 0 tentes");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}




int test_get_current_col_bornes(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}
	unsigned int dernier = game_nb_rows(g) - 1;

	game_set_square(g, 0, 3, TENT);
	game_set_square(g, dernier, 3, TENT);

	if (game_get_current_nb_tents_col(g, 3) != 2) {
		fprintf(stderr, "col 3 devrait avoir 2 tentes\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_current_col_effet_de_bord(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	unsigned int avant = game_get_current_nb_tents_col(g, 1);

	game_set_square(g, 0, 0, TENT);
	game_set_square(g, 1, 0, TENT);
	game_set_square(g, 2, 0, EMPTY);
	game_set_square(g, 3, 0, GRASS);

	unsigned int apres = game_get_current_nb_tents_col(g, 1);
	if (apres != avant) {
		fprintf(stderr, "col 1 a ete modifé par erreur (avant = %u apres = %u)\n", avant, apres);
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

/*==================================================================================================================*/

/*=================================================TEST game_get_current_nb_all====================================*/

int test_current_all_vide(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	if (game_get_current_nb_tents_all(g) != 0) {
		fprintf(stderr, "current_all devrait etre 0\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}


int test_current_all_apres_set(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_square(g, 0, 0, TENT);
	game_set_square(g, 0, 2, TENT);
	game_set_square(g, 2, 1, TENT);
	game_set_square(g, 3, 3, TENT);

	game_set_square(g, 1, 0, GRASS);
	game_set_square(g, 1, 1, EMPTY);

	unsigned int row = 0, col = 0;
	for (unsigned int i = 0; i < game_nb_rows(g); i++) row += game_get_current_nb_tents_row(g, i);
	for (unsigned int j = 0; j < game_nb_cols(g); j++) col += game_get_current_nb_tents_col(g, j);

	unsigned int total = game_get_current_nb_tents_all(g);

	if (row != 4 || col != 4) {
		fprintf(stderr, "rows = %u col = %u (attendu 4/4)\n", row, col);
		game_delete(g);
		return EXIT_FAILURE;
	}
	if (total != row || total != col) {
		fprintf(stderr, "curent_all = %u different de row = %u et col = %u\n", total, row, col);
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

int test_current_all_miseajour(void) {
	game g = game_new_empty();
	if (!g) {
		fprintf(stderr, "game_new_empty() retourne NULL\n");
		return EXIT_FAILURE;
	}

	game_set_square(g, 0, 0, TENT);
	game_set_square(g, 1, 1, TENT);
	if (game_get_current_nb_tents_all(g) != 2) {
		fprintf(stderr, "current_all devrait etre a 2 apres 2 placements\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_square(g, 0, 0, EMPTY);
	if (game_get_current_nb_tents_all(g) != 1) {
		fprintf(stderr, "current_all devrait etre a 1 apres un retrait\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_set_square(g, 2, 2, GRASS);
	if (game_get_current_nb_tents_all(g) != 1) {
		fprintf(stderr, "current_all devrait pas avoir changé\n");
		game_delete(g);
		return EXIT_FAILURE;
	}
	game_delete(g);
	return EXIT_SUCCESS;
}

/*==================================================================================================================*/

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <testname>\n", argv[0]);
		return EXIT_FAILURE;
	}
	if (strcmp(argv[1], "test_game_play_move_complementaire") == 0) return test_game_play_move_complementaire();
	if (strcmp(argv[1], "test_game_nb_neigh_wrapping") == 0) return test_game_nb_neigh_wrapping();
	if (strcmp(argv[1], "test_game_nb_neigh") == 0) return test_game_nb_neigh();
	if (strcmp(argv[1], "test_bords_wrapping") == 0) return test_bords_wrapping();
	if (strcmp(argv[1], "test_total") == 0) return test_total();
	if (strcmp(argv[1], "test_undo") == 0) return test_undo();
	if (strcmp(argv[1], "test_redo") == 0) return test_redo();
	if (strcmp(argv[1], "test_ext") == 0) return test_ext();
	// if (strcmp(argv[1], "test_undo_redo") == 0) return test_undo_redo();
	if (strcmp(argv[1], "test_undo_ecrase") == 0) return test_undo_ecrase();
	if (strcmp(argv[1], "test_current_all_vide") == 0) return test_current_all_vide();
	if (strcmp(argv[1], "test_current_all_apres_set") == 0) return test_current_all_apres_set();
	if (strcmp(argv[1], "test_current_all_miseajour") == 0) return test_current_all_miseajour();
	if (strcmp(argv[1], "test_current_col_vide") == 0) return test_current_col_vide();
	if (strcmp(argv[1], "test_current_col_apres_set") == 0) return test_current_col_apres_set();
	if (strcmp(argv[1], "test_get_current_col_bornes") == 0) return test_get_current_col_bornes();
	if (strcmp(argv[1], "test_current_col_effet_de_bord") == 0) return test_current_col_effet_de_bord();
	if (strcmp(argv[1], "test_current_row_vide") == 0) return test_current_row_vide();
	if (strcmp(argv[1], "test_current_row_apres_set") == 0) return test_current_row_apres_set();
	if (strcmp(argv[1], "test_get_current_row_bornes") == 0) return test_get_current_row_bornes();
	if (strcmp(argv[1], "test_current_row_effet_de_bord") == 0) return test_current_row_effet_de_bord();
	if (strcmp(argv[1], "test_all_apres_set") == 0) return test_all_apres_set();
	if (strcmp(argv[1], "test_all_empty") == 0) return test_all_empty();
	if (strcmp(argv[1], "test_row_vide") == 0) return test_row_vide();
	if (strcmp(argv[1], "test_apres_set") == 0) return test_apres_set();
	if (strcmp(argv[1], "test_get_row_bornes") == 0) return test_get_row_bornes();
	if (strcmp(argv[1], "test_col_ecriture") == 0) return test_col_ecriture();
	if (strcmp(argv[1], "test_col_effet_de_bord") == 0) return test_col_effet_de_bord();
	if (strcmp(argv[1], "test_col_reecriture") == 0) return test_col_reecriture();
	if (strcmp(argv[1], "test_col_reecriture_meme_valeur") == 0) return test_col_reecriture_meme_valeur();
	if (strcmp(argv[1], "test_col_bornes") == 0) return test_col_bornes();
	if (strcmp(argv[1], "test_col_miseajour") == 0) return test_col_miseajour();
	if (strcmp("test_effet_de_bord", argv[1]) == 0) return test_effet_de_bord();
	if (strcmp("test_bornes", argv[1]) == 0) return test_bornes();
	if (strcmp("test_reecriture", argv[1]) == 0) return test_reecriture();
	if (strcmp("test_reecriture_meme_valeur", argv[1]) == 0) return test_reecriture_meme_valeur();
	if (strcmp("dummy", argv[1]) == 0) {
		return test_dummy();
	}
	if (strcmp("test_miseajour", argv[1]) == 0) return test_miseajour();
	if (strcmp("test_ecriture", argv[1]) == 0) {
		return test_ecriture();
	}

	fprintf(stderr, "test \"%s\" inconnu\n", argv[1]);
	return EXIT_FAILURE;
}
