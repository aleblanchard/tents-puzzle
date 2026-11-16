#include "game.h"

#include <assert.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"


game game_new(square* squares, uint* nb_tents_row, uint* nb_tents_col) { return game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, squares, nb_tents_row, nb_tents_col, false); }


game game_new_empty(void) { return game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false); }


game game_copy(cgame g) {
	if (g == NULL) {
		fprintf(stderr, "erreur g est NULL dans game_copy\n");
		return NULL;
	}
	game copy = game_new_empty_ext(g->nrows, g->ncols, g->wrapping);
	if (copy == NULL) {
		fprintf(stderr, "erreur copy est NULL dans game_copy\n");
		return NULL;
	}

	// copie le nombre de tent attendu
	for (uint i = 0; i < g->nrows; i++) {
		copy->rows[i] = g->rows[i];
	}
	for (uint i = 0; i < g->ncols; i++) {
		copy->cols[i] = g->cols[i];
	}

	// copy chaque case de la grille
	for (uint x = 0; x < g->nrows; x++) {
		for (uint y = 0; y < g->ncols; y++) {
			copy->board[x][y] = g->board[x][y];
		}
	}
	return copy;
}


bool game_equal(cgame g1, cgame g2) {
	if (g1 == g2) {
		return true;
	}
	if (g1 == NULL || g2 == NULL) {
		fprintf(stderr, "erreur g1 ou g2 NULL dans game_equal\n");
		return false;
	}

	// verifie que les 2 games ont les memes parametres
	if (g1->nrows != g2->nrows || g1->ncols != g2->ncols || g1->wrapping != g2->wrapping) {
		return false;
	}

	// verifie que le nombre de tent attendu est le meme pour les 2 games
	for (uint i = 0; i < g1->nrows; i++) {
		if (g1->rows[i] != g2->rows[i]) {
			return false;
		}
	}
	for (uint i = 0; i < g1->ncols; i++) {
		if (g1->cols[i] != g2->cols[i]) {
			return false;
		}
	}

	// verifie que le contenu des grilles est le meme sur les 2 games
	for (uint x = 0; x < g1->nrows; x++) {
		for (uint y = 0; y < g1->ncols; y++) {
			if (g1->board[x][y] != g2->board[x][y]) {
				return false;
			}
		}
	}
	return true;
}

EMSCRIPTEN_KEEPALIVE
void game_delete(game g) {
	if (g == NULL) return;
	if (g->board != NULL) {
		for (uint i = 0; i < g->nrows; i++) {
			if (g->board[i] != NULL) {
				free(g->board[i]);
			}
		}
		free(g->board);
	}
	while (g->undo != NULL) {
		lhistory del = g->undo;	 // pop
		g->undo = g->undo->next;
		if (del->board != NULL) {
			for (uint i = 0; i < g->nrows; i++) {
				free(del->board[i]);
			}
			free(del->board);
		}
		free(del);
	}

	while (g->redo != NULL) {
		lhistory del = g->redo;	 // pop
		g->redo = g->redo->next;
		if (del->board != NULL) {
			for (uint i = 0; i < g->nrows; i++) {
				free(del->board[i]);
			}
			free(del->board);
		}
		free(del);
	}

	if (g->rows != NULL) free(g->rows);
	if (g->cols != NULL) free(g->cols);
	free(g);
}


void game_set_square(game g, uint i, uint j, square s) {
	if (g == NULL || i >= g->nrows || j >= g->ncols) {
		fprintf(stderr, "ERREUR : cordonnées invalides\n");
		return;
	}

	while (g->redo != NULL) {  // on vide redo avant(plus valide si on joue)
		lhistory del = g->redo;
		g->redo = g->redo->next;
		for (uint z = 0; z < g->nrows; z++) free(del->board[z]);
		free(del->board);
		free(del);
	}


	// on preparde le undo
	lhistory undo = malloc(sizeof(struct history));
	if (undo == NULL) {
		fprintf(stderr, "ERREUR: allocation");
		return;
	}
	undo->board = malloc(g->nrows * sizeof(square*));  // tableau de pointeurs
	if (undo->board == NULL) {
		fprintf(stderr, "ERREUR: allocation");
		return;
	}

	for (uint i = 0; i < g->nrows; i++) {  // alloue pour chaque pointeur une ligne de col cases
		undo->board[i] = malloc(g->ncols * sizeof(square));
		if (undo->board[i] == NULL) {
			fprintf(stderr, "ERREUR: allocation");
			return;
		}
	}
	// on sauvegarde l'etat actuel de la board
	for (uint z = 0; z < g->nrows; z++) {
		for (uint zz = 0; zz < g->ncols; zz++) {
			undo->board[z][zz] = g->board[z][zz];
		}
	}
	undo->next = g->undo;  // on le met en taine de la liste chainer
	g->undo = undo;

	if (g == NULL || i >= g->nrows || j >= g->ncols) {
		fprintf(stderr, "erreur game_set_square");
		return;
	}
	g->board[i][j] = s;
}

EMSCRIPTEN_KEEPALIVE
square game_get_square(cgame g, uint i, uint j) {
	if (g == NULL || i >= g->nrows || j >= g->ncols) {
		fprintf(stderr, "erreur case hors plateau\n");
		return EMPTY;
	}
	return g->board[i][j];
}


void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents) {
	if (!g || i >= g->nrows) return;
	g->rows[i] = nb_tents;
}


void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents) {
	if (!g || j >= g->ncols) return;
	g->cols[j] = nb_tents;
}


EMSCRIPTEN_KEEPALIVE
	uint game_get_expected_nb_tents_row(cgame g, uint i) {
	if (!g || i >= g->nrows) return 0;
	return g->rows[i];
}


EMSCRIPTEN_KEEPALIVE
	uint game_get_expected_nb_tents_col(cgame g, uint j) {
	if (!g || j >= g->ncols) return 0;
	return g->cols[j];
}


uint game_get_expected_nb_tents_all(cgame g) {
	if (!g) return 0;

	uint total = 0;
	for (uint i = 0; i < g->nrows; i++) {
		for (uint j = 0; j < g->ncols; j++) {
			square s = g->board[i][j];
			if (s == TREE)
				total += 1;
			else if (s == BIGTREE)
				total += 2;
		}
	}
	return total;
}


uint game_get_current_nb_tents_row(cgame g, uint i) {
	if (!g || i >= g->nrows) return 0;
	uint cpt = 0;
	for (uint j = 0; j < g->ncols; j++) {
		if (g->board[i][j] == TENT) cpt++;
	}
	return cpt;
}


uint game_get_current_nb_tents_col(cgame g, uint j) {
	if (!g || j >= g->ncols) return 0;
	uint cpt = 0;
	for (uint i = 0; i < g->nrows; i++) {
		if (g->board[i][j] == TENT) cpt++;
	}
	return cpt;
}


uint game_get_current_nb_tents_all(cgame g) {
	if (!g) return 0;
	uint cpt = 0;
	for (uint i = 0; i < g->nrows; i++) {
		cpt += game_get_current_nb_tents_row(g, i);
	}
	return cpt;
}

bool game_check_row_quota(cgame g, uint i) {
	if (!g || i >= g->nrows) return false;
	return game_get_current_nb_tents_row(g, i) <= game_get_expected_nb_tents_row(g, i);
}

bool game_check_col_quota(cgame g, uint j) {
	if (!g || j >= g->ncols) return false;
	return game_get_current_nb_tents_col(g, j) <= game_get_expected_nb_tents_col(g, j);
}

bool game_check_quotas(cgame g) {
	if (!g) return false;
	for (uint i = 0; i < g->nrows; i++) {
		if (!game_check_row_quota(g, i)) return false;
	}
	for (uint j = 0; j < g->ncols; j++) {
		if (!game_check_col_quota(g, j)) return false;
	}
	return true;
}

EMSCRIPTEN_KEEPALIVE
void game_play_move(game g, uint i, uint j, square s) {
	if (!g) return;
	if (!game_check_move(g, i, j, s)) return;
	if (g->board[i][j] == s) return;

	game_set_square(g, i, j, s);
}


bool game_check_move(cgame g, uint i, uint j, square s) {
	if (!g) return false;
	if (i >= g->nrows || j >= g->ncols) return false;

	if (g->board[i][j] == TREE || g->board[i][j] == BIGTREE) return false;

	// On ne peut pas poser TREE ou BIGTREE
	if (s == TREE || s == BIGTREE) return false;

	return true;
}


EMSCRIPTEN_KEEPALIVE
bool game_won(cgame g) {  // v2
	if (g == NULL) return false;
	int cpt_tent = 0;
	int cpt_tree = 0;
	if (game_get_current_nb_tents_all(g) != game_get_expected_nb_tents_all(g)) {  // on verifie qu'il y a bien autant de tent que d'arbre
		return false;
	}
	// regle 1
	for (uint i = 0; i < g->nrows; i++) {  // on parcour la grille
		for (uint j = 0; j < g->ncols; j++) {
			if (g->board[i][j] == TENT) {
				// regle 3
				cpt_tent += 1;	// on rajoute au compteur a chaque fois qu'il y a une tent
				// regle 1 et 4
				if (game_is_neigh(g, i, j, TENT, true) || (!game_is_neigh(g, i, j, TREE, false) && !game_is_neigh(g, i, j, BIGTREE,
																												  false))) {  // on regarde si il y a des arbres adjacent a la tent ou si
																															  // il n'y a pas des tent adjacente avec la tent ou des BIGTREE
					return false;
				}
			}
			// regle 3
			if (g->board[i][j] == TREE) {
				cpt_tree += 1;	// on rajoute au compteur a chaque fois qu'il y a un arbre
			}
			if (g->board[i][j] == BIGTREE) {
				cpt_tree += 2;
				if (game_nb_neigh(g, i, j, TENT, false) != 2) {
					return false;
				}
			}
		}
	}
	// regle 3
	if (cpt_tree != cpt_tent) {	 // on compare le nombre de tent et d'arbre
		return false;
	}
	// regle 2
	for (uint i = 0; i < g->nrows; i++) {  // on regarde si il y a autant de tent sur la ligne i que de tent attendu
		if (game_get_current_nb_tents_row(g, i) != game_get_expected_nb_tents_row(g, i)) {
			return false;
		}
	}
	for (uint i = 0; i < g->ncols; i++) {  // on regarde si il y a autant de tent sur la colonne j que de tent attendu
		if (game_get_current_nb_tents_col(g, i) != game_get_expected_nb_tents_col(g, i)) {
			return false;
		}
	}
	for (uint i = 0; i < g->nrows; i++) {  // vérifier aussi pour les arbres
		for (uint j = 0; j < g->ncols; j++) {
			if (g->board[i][j] == TREE) {
				if (game_nb_neigh(g, i, j, TENT, false) == 0) return false;
			}
		}
	}
	return true;
}


void game_fill_grass(game g) {
	// vider la pile redo avant de jouer un nv coup
	while (g->redo != NULL) {
		lhistory del = g->redo;
		g->redo = g->redo->next;
		if (del->board != NULL) {
			for (uint i = 0; i < g->nrows; i++) free(del->board[i]);
			free(del->board);
		}
		free(del);
	}
	// on preparde le undo
	lhistory undo = malloc(sizeof(struct history));
	if (undo == NULL) {
		fprintf(stderr, "ERREUR: allocation");
		return;
	}
	undo->board = malloc(g->nrows * sizeof(square*));  // tableau de pointeurs
	if (undo->board == NULL) {
		fprintf(stderr, "ERREUR: allocation");
		return;
	}

	for (uint i = 0; i < g->nrows; i++) {  // alloue pour chaque pointeur une ligne de col cases
		undo->board[i] = malloc(g->ncols * sizeof(square));
		if (undo->board[i] == NULL) {
			fprintf(stderr, "ERREUR: allocation");
			return;
		}
	}

	// on sauvegarde l'etat actuel de la board
	for (uint z = 0; z < g->nrows; z++) {
		for (uint zz = 0; zz < g->ncols; zz++) {
			undo->board[z][zz] = g->board[z][zz];
		}
	}
	undo->next = g->undo;  // on le met en taine de la liste chainer
	g->undo = undo;

	for (uint i = 0; i < g->nrows; i++) {
		for (uint j = 0; j < g->ncols; j++) {
			if (g->board[i][j] == EMPTY) {
				g->board[i][j] = GRASS;
			}
		}
	}
}

EMSCRIPTEN_KEEPALIVE
void game_restart(game g) {
	if (g == NULL) return;

	for (uint i = 0; i < g->nrows; i++) {
		for (uint j = 0; j < g->ncols; j++) {
			if (g->board[i][j] == TENT || g->board[i][j] == GRASS) {
				g->board[i][j] = EMPTY;
			}
		}
	}
	// Vider undo
	while (g->undo != NULL) {
		lhistory tmp = g->undo;
		g->undo = g->undo->next;
		for (uint i = 0; i < g->nrows; i++) free(tmp->board[i]);
		free(tmp->board);
		free(tmp);
	}
	g->undo = NULL;
	// vider redo
	while (g->redo != NULL) {
		lhistory tmp = g->redo;
		g->redo = g->redo->next;
		for (uint i = 0; i < g->nrows; i++) free(tmp->board[i]);
		free(tmp->board);
		free(tmp);
	}
	g->redo = NULL;
}
