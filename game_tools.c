

#include "game_tools.h"

#include <assert.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

EMSCRIPTEN_KEEPALIVE
game game_load(char* filename) {
	uint nb_rows, nb_cols, w;
	bool wrapping;

	FILE* file = fopen(filename, "r");
	assert(file != NULL);

	int r1 = fscanf(file, "%u %u %u", &nb_rows, &nb_cols, &w);
	(void)r1;
	assert(nb_rows > 0 && nb_cols > 0 && (w == 1 || w == 0));
	if (w == 1)
		wrapping = true;
	else
		wrapping = false;

	game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
	assert(g != NULL);

	uint nb;
	for (uint i = 0; i < game_nb_rows(g); i++) {
		int r2 = fscanf(file, "%u", &nb);
		(void)r2;
		game_set_expected_nb_tents_row(g, i, nb);
	}

	for (uint i = 0; i < game_nb_cols(g); i++) {
		int r3 = fscanf(file, "%u", &nb);
		(void)r3;
		game_set_expected_nb_tents_col(g, i, nb);
	}

	char c;
	square s;
	for (uint row = 0; row < game_nb_rows(g); row++) {
		for (uint col = 0; col < game_nb_cols(g); col++) {
			int r4 = fscanf(file, " %c", &c);
			(void)r4;
			if (c == 'E')
				s = EMPTY;
			else if (c == 'X')
				s = TREE;
			else if (c == 'T')
				s = TENT;
			else if (c == 'G')
				s = GRASS;
			else
				s = BIGTREE;
			game_set_square(g, row, col, s);
		}
	}
	fclose(file);
	return g;
}

EMSCRIPTEN_KEEPALIVE
void game_save(cgame g, char* filename) {
	FILE* file = fopen(filename, "w");
	assert(file != NULL);

	uint w;
	if (game_is_wrapping(g)) {
		w = 1;
	} else {
		w = 0;
	}

	fprintf(file, "%u %u %u\n", game_nb_rows(g), game_nb_cols(g), w);  //<nb_rows> <nb_cols> <wrapping> \n ligne 1;

	for (uint i = 0; i < game_nb_rows(g); i++) {  // <nb_expected_tents_row[0]> <nb_expected_tents_row[1]> ... <nb_expected_tents_row[nb_rows-1]> \n
		fprintf(file, "%u ", game_get_expected_nb_tents_row(g, i));
	}
	fprintf(file, "\n");

	for (uint i = 0; i < game_nb_cols(g); i++) {  // <nb_expected_tents_col[0]> <nb_expected_tents_col[1]> ... <nb_expected_tents_col[nb_cols-1]> \n
		fprintf(file, "%u ", game_get_expected_nb_tents_col(g, i));
	}
	fprintf(file, "\n");

	char c;
	for (uint row = 0; row < game_nb_rows(g); row++) { /* <squares[0][0]> <squares[0][1]> ... <squares[0][nb_cols-1]> \n
														  <squares[1][0]> <squares[1][1]> ... <squares[1][nb_cols-1]> \n
														  <squares[2][0]> <squares[2][1]> ... <squares[2][nb_cols-1]> \n
														  ...
														  <squares[nb_rows-1][0]> <squares[nb_rows-1][1]> ... <squares[nb_rows-1][nb_cols-1]> \n*/
		for (uint col = 0; col < game_nb_cols(g); col++) {
			square s = game_get_square(g, row, col);
			if (s == EMPTY)
				c = 'E';
			else if (s == TREE)
				c = 'X';
			else if (s == TENT)
				c = 'T';
			else if (s == GRASS)
				c = 'G';
			else
				c = 'B';

			fprintf(file, "%c ", c);
		}
		fprintf(file, "\n");
	}
	fclose(file);
	printf("Partie sauvegardé\n");
}

void get_random_position(uint nb_rows, uint nb_cols, uint* i, uint* j) {  // genere position aleatoire sur le board
	*i = (((double)rand() * nb_rows) / RAND_MAX);						  // selon l'annexe
	*j = (((double)rand() * nb_cols) / RAND_MAX);
}


bool can_place_tent(game g, uint i, uint j) {
	if (game_get_square(g, i, j) != EMPTY) return false;	   // case doit être vide
	if (game_nb_neigh(g, i, j, TENT, true) > 0) return false;  // vérifier pas de tentes adjacentes (ortho et diag); true pour les diags
	return true;
}



bool check_place_tree(game g, uint i, uint j) {
	if (game_get_square(g, i, j) != EMPTY) return false;  // la case doit etre vide
	uint xi, xj;
	direction direction[] = {UP, DOWN, LEFT, RIGHT};
	for (int f = 0; f <= 3; f++) {	// verifier qu'il existe au moins 1 voisin orthogonal ou on peu placer une tente (règles)
		if (game_get_adjacent_square(g, i, j, direction[f], &xi, &xj)) {
			if (can_place_tent(g, xi, xj)) {
				return true;  // trouvé un emplacement vite pour la tent
			}
		}
	}
	return false;  // aucun emplacement qui fonctionne trouvé
}



bool check_place_bigtree(game g, uint i, uint j) {
	if (game_get_square(g, i, j) != EMPTY) return false;  // la case doit etre vide
	uint xi, xj;
	direction direction[] = {UP, DOWN, LEFT, RIGHT};  // vérifier 2 voisins ortho ou can place tent marche et qui ne sont pas adjancentes entre elles
	int cpt = 0;									  // compteur de postions valides trouvées
	uint itent[4], jtent[4];						  // stockage des coord des positions trouvées
	for (int f = 0; f <= 3; f++) {					  // on regarde les 4 voisins ortho
		if (game_get_adjacent_square(g, i, j, direction[f], &xi, &xj)) {
			if (can_place_tent(g, xi, xj)) {
				itent[cpt] = xi;  // sauvegarde de la pos
				jtent[cpt] = xj;
				cpt++;
			}
		}
	}
	if (cpt < 2) return false;	// on veut 2 pos valides pour le bigtree

	for (int p = 0; p < cpt; p++) {	 // vérifier que les 2 positions ne sont pas adjacentes
		for (int k = p + 1; k < cpt; k++) {
			int disti = itent[p] - itent[k];  // calculer la distance entre les deux
			int distj = jtent[p] - jtent[k];
			if (disti < 0) disti = -disti;
			if (distj < 0) distj = -distj;	// on met toutes les valeurs en positif
			if (disti > 1 || distj > 1) {	// si distance >1 dans une dimension, les positions ne sont pas adjacentes (c'est bon)
				return true;				// solution trouvée!
			}
		}
	}
	return false;  // aucune solution trouvée..... :(
}

void set_tree_w_tent(game g, uint i, uint j) {
	game_set_square(g, i, j, TREE);	 // place l'arbre

	uint xi, xj;  // coor voisin
	direction direction[] = {UP, DOWN, LEFT, RIGHT};
	for (int f = 0; f <= 3; f++) {
		if (game_get_adjacent_square(g, i, j, direction[f], &xi, &xj)) {  // tester en haut, bas, gauche, droite
			if (can_place_tent(g, xi, xj)) {							  // verifier si poser la tente est legal ou non
				game_set_square(g, xi, xj, TENT);
				return;	 // sortir si on trouve, évite de placer plusieurs tents
			}
		}
	}
}

void set_bigtree_w_tent(game g, uint i, uint j) {  //REFONTE (causait un bug et faisait des jeux impossibles)
	game_set_square(g, i, j, BIGTREE);		//place bigtree sur la case

	uint upi, upj, downi, downj, lefti, leftj, righti, rightj;		//coordonnées des 4 voisins potentiels
	
	//test vertical (les tentes peuvent pas etre a côté en diag, donc forcément haut + bas ou droite + gauche)
	if (game_get_adjacent_square(g,i,j,UP,&upi,&upj) && can_place_tent(g,upi,upj) && game_get_adjacent_square(g,i,j,DOWN,&downi,&downj) && can_place_tent(g,downi,downj)){
		game_set_square(g,upi,upj,TENT); game_set_square(g,downi,downj,TENT);	//positions valides : on place les deux tentes
		return;
	}
	//test horizontal
	if (game_get_adjacent_square(g,i,j,LEFT,&lefti,&leftj) && can_place_tent(g,lefti,leftj) && game_get_adjacent_square(g,i,j,RIGHT,&righti,&rightj) && can_place_tent(g,righti,rightj)){
		game_set_square(g,lefti,leftj,TENT); game_set_square(g,righti,rightj,TENT);	//positions valides : on place les deux tentes
		return;
	}
}

EMSCRIPTEN_KEEPALIVE
game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_trees, uint nb_bigtrees, bool solution) {
	game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
	assert(g != NULL);

	// PLACER LES BIGTREE
	uint bigtree_placed = 0;
	uint cpt = 0;
	uint lim_cpt = nb_rows * nb_cols;						 // limite d'essais
	while (bigtree_placed < nb_bigtrees && cpt < lim_cpt) {	 // tant qu'on a pas mis tousl es bigtree et que cpt<limite
		uint i, j;
		get_random_position(nb_rows, nb_cols, &i, &j);	// pos aleatoire
		if (check_place_bigtree(g, i, j)) {				// verification si on peut poser
			set_bigtree_w_tent(g, i, j);				// on place bigtree et sa tente si solution est true
			bigtree_placed++;
		}
		cpt++;
	}
	// PLACER LES TREES
	uint trees_placed = 0;
	uint cpt_trees = 0;
	while (trees_placed < nb_trees && cpt_trees < lim_cpt) {
		uint i, j;
		get_random_position(nb_rows, nb_cols, &i, &j);
		if (check_place_tree(g, i, j)) {
			set_tree_w_tent(g, i, j);
			trees_placed++;
		}
		cpt_trees++;
	}

	// COMPTEUR DE TENTS EXPECTED
	for (uint i = 0; i < nb_rows; i++) {  // parcours chaque ligne
		uint cpt = 0;
		for (uint j = 0; j < nb_cols; j++) {
			square s = game_get_square(g, i, j);
			if (s == TENT) cpt++;
		}
		game_set_expected_nb_tents_row(g, i, cpt);
	}
	for (uint i = 0; i < nb_cols; i++) {  // parcours chaque col
		uint cpt = 0;
		for (uint j = 0; j < nb_rows; j++) {
			square s = game_get_square(g, j, i);
			if (s == TENT) cpt++;
		}
		game_set_expected_nb_tents_col(g, i, cpt);
	}

	if (solution) {	 // remplit le board de grass si solution est true
		game_fill_grass(g);
	} else {
		game_restart(g);  // affiche la game sans la solution
	}

	return g;
}

bool game_ratio(cgame g, uint i, uint j) {	// verifie si le ratio entre tent posé et tent attendu et toujour bon
	if (g == NULL || j >= game_nb_cols(g) || i >= game_nb_rows(g)) return false;
	if (game_get_current_nb_tents_col(g, j) <= game_get_expected_nb_tents_col(g, j) && game_get_current_nb_tents_row(g, i) <= game_get_expected_nb_tents_row(g, i)) {
		return true;
	}
	return false;
}


// fonction ici pour code + propre
void solve_rec(game g, uint* count, bool* fin) {  // comme binword_rec
	int a = 0;
	int b = 0;	// chercher une case empty
	bool found = false;
	for (uint i = 0; i < game_nb_rows(g) && !found; i++) {	// on sort de la boucle des qu'on trouve empty
		for (uint j = 0; j < game_nb_cols(g) && !found; j++) {
			if (game_get_square(g, i, j) == EMPTY) {
				a = i;
				b = j;
				found = true;
			}
		}
	}
	if (!found) {			// si plus de case empty/board complet (copier la partie pos==len ds binwordrec)
		if (game_won(g)) {	// si gamewon est true, alors c'est bon
			(*count)++;
			if (fin) *fin = true;  // fin sert a savoir si la fonction qui l'utilise a besoin de la premiere solution ou si elle veut savoir le nombre de solution posible
		}
		return;	 // pas de solution donc on repars
	}
	square GT[] = {GRASS, TENT};
	for (int j = 0; j < 2; j++) {
		game_set_square(g, a, b, GT[j]);
		bool ok = game_ratio(g, a, b);				   // on test uniquement si il n'y a pas deja trop de tent sur la ligne et colonne comparer a se qui est attendu
		if (ok && GT[j] == TENT) {					   // on test que si il n'y a pas de tent adjacentes
			if (game_is_neigh(g, a, b, TENT, true)) {  // verifie si il y a des tents adjacentes
				ok = false;
			}
			if (!game_is_neigh(g, a, b, TREE, false) && (!game_is_neigh(g, a, b, BIGTREE, false))) {  // verifie si la tent est placé a coté d'un TREE
				ok = false;
			}
		}
		if (ok) {
			solve_rec(g, count, fin);
			if (fin && *fin) return;
		}
	}
	game_set_square(g, a, b, EMPTY);
	// remet empty
	return;
}

EMSCRIPTEN_KEEPALIVE
bool game_solve(game g) {  // fonction entiere a faire
	game tmp = game_copy(g);
	if (tmp == NULL) return false;
	uint count = 0;
	bool fin = false;
	solve_rec(tmp, &count, &fin);
	if (fin) {
		for (uint i = 0; i < game_nb_rows(g); i++) {
			for (uint j = 0; j < game_nb_cols(g); j++) {
				game_set_square(g, i, j, game_get_square(tmp, i, j));
			}
		}
		game_delete(tmp);
		return true;
	}
	game_delete(tmp);
	return false;
}

uint game_nb_solutions(cgame g) {  // fonction entiere a faire
	uint count = 0;
	game g2 = game_copy(g);		  // on ne veut pas que g soit modifier
	solve_rec(g2, &count, NULL);  /// on veut que la fonction continue jusqu'au bout
	game_delete(g2);
	return count;
}