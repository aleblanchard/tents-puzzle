#include "game_aux.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include <stdio.h>

#include "game_ext.h"
#include "game_struct.h"

void game_print(cgame g) {
	if (g == NULL) return;

	printf("   ");
	for (uint k = 0; k < game_nb_cols(g); k++) {
		printf("%d", k % 10);
	}
	printf("\n   ");
	for (uint k = 0; k < game_nb_cols(g); k++) {
		printf("-");
	}
	printf("\n");

	for (uint i = 0; i < game_nb_rows(g); i++) {
		printf("%d |", i % 10);
		for (uint j = 0; j < game_nb_cols(g); j++) {
			if (g->board[i][j] == TREE)
				printf("x");
			else if (g->board[i][j] == TENT)
				printf("*");
			else if (g->board[i][j] == GRASS)
				printf("-");
			else if (g->board[i][j] == BIGTREE)
				printf("#");
			else
				printf(" ");
		}
		printf("| %d\n", game_get_expected_nb_tents_row(g, i));
	}
	printf("   ");
	for (uint k = 0; k < game_nb_cols(g); k++) {
		printf("-");
	}
	printf("\n   ");
	for (uint k = 0; k < game_nb_cols(g); k++) {
		printf("%d", game_get_expected_nb_tents_col(g, k));
	}
	printf("\n");
}

EMSCRIPTEN_KEEPALIVE
game game_default(void) {
	game g = game_new_empty();
	// mettre les arbres
	game_set_square(g, 0, 0, TREE);
	game_set_square(g, 0, 5, TREE);
	game_set_square(g, 0, 6, TREE);
	game_set_square(g, 2, 6, TREE);
	game_set_square(g, 3, 0, TREE);
	game_set_square(g, 3, 2, TREE);
	game_set_square(g, 4, 4, TREE);
	game_set_square(g, 5, 2, TREE);
	game_set_square(g, 7, 1, TREE);
	game_set_square(g, 7, 2, TREE);
	game_set_square(g, 7, 5, TREE);
	game_set_square(g, 7, 6, TREE);

	// Colonnes
	game_set_expected_nb_tents_col(g, 0, 3);
	game_set_expected_nb_tents_col(g, 1, 0);
	game_set_expected_nb_tents_col(g, 2, 2);
	game_set_expected_nb_tents_col(g, 3, 1);
	game_set_expected_nb_tents_col(g, 4, 2);
	game_set_expected_nb_tents_col(g, 5, 1);
	game_set_expected_nb_tents_col(g, 6, 2);
	game_set_expected_nb_tents_col(g, 7, 1);

	// Lignes
	game_set_expected_nb_tents_row(g, 0, 1);
	game_set_expected_nb_tents_row(g, 1, 2);
	game_set_expected_nb_tents_row(g, 2, 1);
	game_set_expected_nb_tents_row(g, 3, 2);
	game_set_expected_nb_tents_row(g, 4, 2);
	game_set_expected_nb_tents_row(g, 5, 0);
	game_set_expected_nb_tents_row(g, 6, 1);
	game_set_expected_nb_tents_row(g, 7, 3);

	while (g->undo != NULL) {  // vider le undo apres initialisation (permet de pas pouvoir supprimer les arbres avec plein de undo)
		lhistory del = g->undo;
		g->undo = g->undo->next;
		for (uint i = 0; i < g->nrows; i++) free(del->board[i]);
		free(del->board);
		free(del);
	}

	return g;
}


game game_default_solution(void) {
	game g = game_default();  // Récupère le jeu avec les arbres bien placés
	game_fill_grass(g);		  // Met de l'herbe partout

	game_set_square(g, 0, 4, TENT);
	game_set_square(g, 1, 0, TENT);
	game_set_square(g, 1, 6, TENT);
	game_set_square(g, 2, 2, TENT);
	game_set_square(g, 3, 4, TENT);
	game_set_square(g, 3, 6, TENT);
	game_set_square(g, 4, 0, TENT);
	game_set_square(g, 4, 2, TENT);
	game_set_square(g, 6, 5, TENT);
	game_set_square(g, 7, 0, TENT);
	game_set_square(g, 7, 3, TENT);
	game_set_square(g, 7, 7, TENT);

	return g;
}


bool game_get_adjacent_square(cgame g, uint i, uint j, direction d, uint* pi_next, uint* pj_next) {
	if (i >= g->nrows || j >= g->ncols) {
		return false;
	}

	if (d == HERE) {
		*pi_next = i;
		*pj_next = j;
	} else if (d == UP) {
		if (i == 0) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pi_next = g->nrows - 1;		 // si deja tout en haut on passe en bas
		} else {
			*pi_next = i - 1;  // on monte normalement
		}
		*pj_next = j;
	} else if (d == UP_RIGHT) {
		if (i == 0) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pi_next = g->nrows - 1;		 // si deja tout en haut on passe en bas
		} else {
			*pi_next = i - 1;  // on monte normalement
		}
		if (j == g->ncols - 1) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pj_next = 0;					 // si deja tout a droite on passe a gauche
		} else {
			*pj_next = j + 1;  // on va a droite normalement
		}
	} else if (d == RIGHT) {
		if (j == g->ncols - 1) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pj_next = 0;					 // si deja tout a droite on passe a gauche
		} else {
			*pj_next = j + 1;  // on va a droite normalement
		}
		*pi_next = i;
	} else if (d == DOWN_RIGHT) {
		if (i == g->nrows - 1) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pi_next = 0;					 // si deja tout en bas on passe en haut
		} else {
			*pi_next = i + 1;  // on descend normalement
		}
		if (j == g->ncols - 1) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pj_next = 0;					 // si deja tout a droite on passe a gauche
		} else {
			*pj_next = j + 1;  // on va a droite normalement
		}
	} else if (d == DOWN) {
		if (i == g->nrows - 1) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pi_next = 0;					 // si deja tout en bas on passe en haut
		} else {
			*pi_next = i + 1;  // on descend normalement
		}
		*pj_next = j;
	} else if (d == DOWN_LEFT) {
		if (i == g->nrows - 1) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pi_next = 0;					 // si deja tout en bas on passe en haut
		} else {
			*pi_next = i + 1;  // on descend normalement
		}
		if (j == 0) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pj_next = g->ncols - 1;		 // si deja tout a gauche on passe a droite
		} else {
			*pj_next = j - 1;  // on va a gauche normalement
		}
	} else if (d == LEFT) {
		if (j == 0) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pj_next = g->ncols - 1;		 // si deja tout a gauche on passe a droite
		} else {
			*pj_next = j - 1;  // on va a gauche normalement
		}
		*pi_next = i;
	} else if (d == UP_LEFT) {
		if (i == 0) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pi_next = g->nrows - 1;		 // si deja tout en haut on passe en bas
		} else {
			*pi_next = i - 1;  // on monte normalement
		}
		if (j == 0) {
			if (!g->wrapping) return false;	 // si pas de wrapping return false
			*pj_next = g->ncols - 1;		 // si deja tout a gauche on passe a droite
		} else {
			*pj_next = j - 1;  // on va a gauche normalement
		}
	} else {
		return false;
	}
	return true;
}


bool game_is_neigh(cgame g, uint i, uint j, square s, bool diag) {
	if (g == NULL || i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
		return false;
	}
	uint x, y;

	direction dir[] = {UP, RIGHT, DOWN, LEFT};
	direction dirdiag[] = {UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};

	for (uint k = 0; k < 4; k++) {	// on regarde les voisins orthogonnaux
		if (game_get_adjacent_square(g, i, j, dir[k], &x, &y)) {
			if (game_get_square(g, x, y) == s) {
				return true;
			}
		}
	}
	if (diag) {	 // si diag a True on regarde les voisins de la diagonale
		for (uint k = 0; k < 4; k++) {
			if (game_get_adjacent_square(g, i, j, dirdiag[k], &x, &y)) {
				if (game_get_square(g, x, y) == s) {
					return true;
				}
			}
		}
	}
	return false;
}


uint game_nb_neigh(cgame g, uint i, uint j, square s, bool diag) {
	if (g == NULL) return 0;
	uint count = 0;
	uint x, y;

	direction dir[] = {UP, RIGHT, DOWN, LEFT};
	direction dirdiag[] = {UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};

	for (uint k = 0; k < 4; k++) {
		if (game_get_adjacent_square(g, i, j, dir[k], &x, &y)) {
			if (game_get_square(g, x, y) == s) {
				count++;
			}
		}
	}

	if (diag) {
		for (uint k = 0; k < 4; k++) {
			if (game_get_adjacent_square(g, i, j, dirdiag[k], &x, &y)) {
				if (game_get_square(g, x, y) == s) {
					count++;
				}
			}
		}
	}

	return count;
}