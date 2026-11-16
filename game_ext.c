#include "game_ext.h"

#include <assert.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include <stdio.h>
#include <stdlib.h>

#include "game_struct.h"

game game_new_ext(uint nb_rows, uint nb_cols, square* squares, uint* nb_tents_row, uint* nb_tents_col, bool wrapping) {
	assert(squares != NULL);
	assert(nb_tents_row != NULL);
	assert(nb_tents_col != NULL);

	game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
	if (g == NULL) return NULL;

	// initialise le nombre de tent attendu sur la game
	for (uint i = 0; i < nb_rows; i++) {
		g->rows[i] = nb_tents_row[i];
	}
	for (uint i = 0; i < nb_cols; i++) {
		g->cols[i] = nb_tents_col[i];
	}

	// initialise chaque case de la grille par la valeur du tableau squares
	uint count = 0;
	for (uint x = 0; x < nb_rows; x++) {
		for (uint y = 0; y < nb_cols; y++) {
			g->board[x][y] = squares[count];
			count++;
		}
	}
	return g;
}


game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping) {
	game g = malloc(sizeof(struct game_s));
	if (g == NULL) return NULL;

	// initialisation des parametres
	g->nrows = nb_rows;
	g->ncols = nb_cols;
	g->wrapping = wrapping;
	g->undo = NULL;
	g->redo = NULL;

	g->rows = malloc(nb_rows * sizeof(uint));
	g->cols = malloc(nb_cols * sizeof(uint));
	g->board = malloc(nb_rows * sizeof(square*));

	if (g->rows == NULL || g->cols == NULL || g->board == NULL) {
		game_delete(g);
		return NULL;
	}

	// initialise le nombre de tentes attendu à 0 sur la grille
	for (uint i = 0; i < nb_cols; i++) {
		g->cols[i] = 0;
	}
	for (uint x = 0; x < nb_rows; x++) {
		g->rows[x] = 0;


		g->board[x] = malloc(nb_cols * sizeof(square));
		if (g->board[x] == NULL) {
			game_delete(g);
			return NULL;
		}

		for (uint y = 0; y < nb_cols; y++) {  // initialine chaque case de la grille à EMPTY
			g->board[x][y] = EMPTY;
		}
	}
	return g;
}

EMSCRIPTEN_KEEPALIVE
uint game_nb_rows(cgame g) {
	if (g == NULL) return 0;
	return g->nrows;
}

EMSCRIPTEN_KEEPALIVE
uint game_nb_cols(cgame g) {
	if (g == NULL) return 0;
	return g->ncols;
}


bool game_is_wrapping(cgame g) {
	if (g == NULL) return false;
	return g->wrapping;
}


void copy_board(square** src, square** dst, uint rows, uint cols) {
	for (uint i = 0; i < rows; i++) {
		for (uint j = 0; j < cols; j++) dst[i][j] = src[i][j];
	}
}

EMSCRIPTEN_KEEPALIVE
void game_undo(game g) {
	if (g == NULL || g->undo == NULL) return;  // on verrifie que le tableau et bien initialiser et on verifie que ce n'est pas la premiere action


	// on a cree une pile undo et redo
	// en gros a chaque etape on savegarde l'etat du tableau et quand on undo on revient a l'etat d'avant

	// on preparde le redo
	lhistory redo_bis = malloc(sizeof(struct history));
	if (redo_bis == NULL) {
		fprintf(stderr, "ERREUR: allocation\n");
		return;
	}
	redo_bis->board = malloc(g->nrows * sizeof(square*));  // tableau de pointeurs
	if (redo_bis->board == NULL) {
		fprintf(stderr, "ERREUR: allocation\n");
		free(redo_bis);
		return;
	}


	for (uint i = 0; i < g->nrows; i++) {  // alloue pour chaque pointeur une ligne de col cases
		redo_bis->board[i] = malloc(g->ncols * sizeof(square));

		if (redo_bis->board[i] == NULL) {  // en cas d'erreur
			fprintf(stderr, "ERREUR: allocation\n");
			for (uint j = 0; j < i; j++) {
				free(redo_bis->board[j]);
			}
			free(redo_bis->board);
			free(redo_bis);
			return;
		}
	}

	// copie du tableau actuel
	for (uint i = 0; i < g->nrows; i++) {			 // pour chaque ligne
		for (uint j = 0; j < g->ncols; j++) {		 // pour chaque colones
			redo_bis->board[i][j] = g->board[i][j];	 // on copie la valeur de la case
		}
	}

	// on transforme le tableau actuel pour qu'il devient undo
	for (uint i = 0; i < g->nrows; i++) {			// pour chaque ligne
		for (uint j = 0; j < g->ncols; j++) {		// pour chaque colones
			g->board[i][j] = g->undo->board[i][j];	// on modifie la valeur de la case
		}
	}

	// on fait le chainage
	redo_bis->next = g->redo;
	g->redo = redo_bis;

	// on recupère le undo qu'on vient d'appliquer
	lhistory del = g->undo;
	g->undo = g->undo->next;

	// et on le supprime
	for (uint i = 0; i < g->nrows; i++) {
		free(del->board[i]);
	}
	free(del->board);
	free(del);
}

EMSCRIPTEN_KEEPALIVE
void game_redo(game g) {
	if (g == NULL || g->redo == NULL) return;  // on verrifie que le tableau et bien initialiser et on verifie que ce n'est pas la premiere action


	// on preparde le undo
	lhistory undo_bis = malloc(sizeof(struct history));
	if (undo_bis == NULL) {
		fprintf(stderr, "ERREUR: allocation\n");
		return;
	}

	undo_bis->board = malloc(g->nrows * sizeof(square*));  // tableau de pointeurs
	if (undo_bis->board == NULL) {
		fprintf(stderr, "ERREUR: allocation\n");
		free(undo_bis);
		return;
	}


	for (uint i = 0; i < g->nrows; i++) {  // on cree la board du undo
		undo_bis->board[i] = malloc(g->ncols * sizeof(square));
		if (undo_bis->board[i] == NULL) {
			fprintf(stderr, "ERREUR: allocation\n");
			for (uint j = 0; j < i; j++) {
				free(undo_bis->board[j]);
			}
			free(undo_bis->board);
			free(undo_bis);
			return;
		}
	}

	// on sauvegarde l'etat actuel de la board
	for (uint i = 0; i < g->nrows; i++) {
		for (uint j = 0; j < g->ncols; j++) {
			undo_bis->board[i][j] = g->board[i][j];
		}
	}

	// et on fait le redo (la board de la game devient le dernier redo)
	for (uint i = 0; i < g->nrows; i++) {
		for (uint j = 0; j < g->ncols; j++) {
			g->board[i][j] = g->redo->board[i][j];
		}
	}

	// on fait le chainage
	undo_bis->next = g->undo;
	g->undo = undo_bis;

	// on recupere le redo qui vient d'etre fait
	lhistory del = g->redo;	  // on met un pointeur pour le free apres
	g->redo = g->redo->next;  // on le pop

	// et on le supprime
	for (uint i = 0; i < g->nrows; i++) {
		free(del->board[i]);
	}
	free(del->board);
	free(del);
}
