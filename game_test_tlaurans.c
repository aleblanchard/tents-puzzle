#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"  // creation du game g qui pointe sur la game par default

int test_dummy() { return EXIT_SUCCESS; }  // test de dummy qui doit renvoyer toujours EXIT_success

int test_default_solution() {  // test de default_solution

	game g = game_default_solution();  // on cree une game avec la solution
	assert(g != NULL);

	// pour que test_default_solution soit bon il faut que toute les case soit replie
	for (uint i = 0; i < DEFAULT_SIZE; ++i) {	   // test de tout les ligne
		for (uint j = 0; j < DEFAULT_SIZE; ++j) {  // test de toute les collone

			square s = game_get_square(g, i, j);  // on recupere la valeur de la case

			if (s == EMPTY) {  // si elle est vide
				fprintf(stderr, "test_default_solution EMPTY en %u,%u ca ne peu pas etre la solution\n", i,
						j);			  // donne la case vide
				game_delete(g);		  // on s'arrete
				return EXIT_FAILURE;  // erreur test
			}
		}
	}

	// pour que test_default_solution soit bon il faut aussi que ca donne la bonne reponse
	if (!game_won(g)) {	 // si ca n'est pas une game_won
		fprintf(stderr,
				"test_default_solution game_default_solution n'est pas bon\n");	 // verifie que la game est bien win
		game_delete(g);															 // on s'arrete
		return EXIT_FAILURE;													 // erreur test
	}

	// si tout est bon
	game_delete(g);		  // on s'arrete
	return EXIT_SUCCESS;  // test reussi
}

int test_game_is_neigh() {	// test de game_is_neigh

	game g = game_default_solution();  // on cree une game avec deja de case de remplie
	assert(g != NULL);

	// on fait un test si la case 0 1 na pas d'arbre autour et  si c'est faut
	if (!game_is_neigh(g, 0, 1, TREE, false)) {
		fprintf(stderr, "test_game_is_neigh Echec: (0,1) doit avoir un voisin TREE orthogonal\n");	// message d'erreur
		game_delete(g);																				// on s'arrete
		return EXIT_FAILURE;																		// erreur test
	}

	// on fait un test si la case 1 1 na pas d'arbre autour et  si c'est vrais
	if (game_is_neigh(g, 1, 1, TREE, false)) {
		fprintf(stderr,
				"test_game_is_neigh Echec: (1,1) ne doit pas avoir de voisin TREE orthogonal\n");  // message d'erreur
		game_delete(g);																			   // on s'arrete
		return EXIT_FAILURE;																	   // erreur test
	}

	// on fait le test si la case  1 1 a un arbre autour et si c'est vrais
	if (!game_is_neigh(g, 1, 1, TREE, true)) {
		fprintf(stderr, "test_game_is_neigh Echec: (1,1) doit avoir un voisin TREE en diagonal\n");	 // message d'erreur
		game_delete(g);																				 // on s'arrete
		return EXIT_FAILURE;																		 // erreur test
	}

	// tout les test sont passer donc
	game_delete(g);		  // on s'arrete
	return EXIT_SUCCESS;  // test reussi
}

int test_game_get_adjacent_square() {  // test de game_get_adjacent_square

	game g = game_default_solution();  // on cree une game avec deja de case de remplie
	assert(g != NULL);

	uint nx, ny;  // creation de variable nx et ny

	// ici on test si la case a droite de 0 0 existe et qu'elle a bien la case 0 1 sinon:
	if (!game_get_adjacent_square(g, 0, 0, RIGHT, &nx, &ny) || nx != 0 || ny != 1) {
		fprintf(stderr, "test_game_get_adjacent_square Echec: adjacent RIGHT de (0,0) attendu (0,1) obtenu %u,%u\n", nx,
				ny);		  // message d'erreur
		game_delete(g);		  // on s'arrete
		return EXIT_FAILURE;  // erreur test
	}

	// ici on test si la case au dessus de la case 0 0 existe pas sinon:
	if (game_get_adjacent_square(g, 0, 0, UP, &nx, &ny)) {
		fprintf(stderr,
				"test_game_get_adjacent_square Echec: adjacent UP de (0,0) doit etre hors grille\n");  // message d'erreur
		game_delete(g);																				   // on s'arrete
		return EXIT_FAILURE;																		   // erreur test
	}

	// ici on test si la case en bas a droite de 0 0 existe et qu'elle a bien la case 1 1 sinon:
	if (!game_get_adjacent_square(g, 0, 0, DOWN_RIGHT, &nx, &ny) || nx != 1 || ny != 1) {
		fprintf(stderr, "Echec: adjacent DOWN_RIGHT de (0,0) attendu (1,1) obtenu %u,%u\n", nx,
				ny);		  // message d'erreur
		game_delete(g);		  // on s'arrete
		return EXIT_FAILURE;  // erreur test
	}

	// ici on test si la case en haut a gauche de 1 1 existe et qu'elle a bien la case 0 0 sinon:
	if (!game_get_adjacent_square(g, 1, 1, UP_LEFT, &nx, &ny) || nx != 0 || ny != 0) {
		fprintf(stderr, "Echec: adjacent UP_LEFT de (1,1) attendu (0,0) obtenu %u,%u\n", nx, ny);  // message d'erreur
		game_delete(g);																			   // on s'arrete
		return EXIT_FAILURE;																	   // erreur test
	}

	// ici on test si la case a gauche de 7 7 existe et qu'elle a bien la case 7 6 sinon
	if (!game_get_adjacent_square(g, 7, 7, LEFT, &nx, &ny) || nx != 7 || ny != 6) {
		fprintf(stderr, "Echec: adjacent LEFT de (7,7) attendu (7,6) obtenu %u,%u\n", nx, ny);	// message d'erreur
		game_delete(g);																			// on s'arrete
		return EXIT_FAILURE;																	// erreur test
	}

	// ici on test si la case en haut de 7 7 existe et qu'elle a bien la valeur 6 7 sinon
	if (!game_get_adjacent_square(g, 7, 7, UP, &nx, &ny) || nx != 6 || ny != 7) {
		fprintf(stderr, "Echec: adjacent UP de (7,7) attendu (6,7) obtenu %u,%u\n", nx, ny);  // message d'erreur
		game_delete(g);																		  // on s'arrete
		return EXIT_FAILURE;																  // erreur test
	}

	// tout les test sont passer donc
	game_delete(g);		  // on s'arrete
	return EXIT_SUCCESS;  // test reussi
}

int test_game_default() {  // test de game_default

	game g = game_default();  // creation du game avec les paramettre game default
	assert(g != NULL);

	int tree = 0;  // creation d'un compteur tree initialiser a 0

	for (uint i = 0; i < DEFAULT_SIZE; ++i) {	   // boucle pour parcourire tout les ligne
		for (uint j = 0; j < DEFAULT_SIZE; ++j) {  // boucle pour parcourire tout les case

			square s = game_get_square(g, i, j);  // initialisation d'une variable s avec la valeur de la case i j

			if (s == TENT || s == GRASS) {	// si la case s et une tent ou de l'herbe (ce qui ne doit pas etre le cas
											// puisque c'est une game vierge)
				fprintf(stderr, "test_game_default comporte une tent ou de l'herbe en %d %d\n", i,
						j);			  // message d'erreur
				game_delete(g);		  // on s'arrete
				return EXIT_FAILURE;  // erreur test
			}

			if (s == TREE) {  // si s est un arbre
				tree++;		  // on ajoute 1 au compte d'arbre sur le board
			}
		}
	}

	if (tree < 12)	// on compare le nombre d'arbre 12 parce que le jeu pose toujours 12 arbre a chaque creation de game
	{
		fprintf(stderr, "test_game_default erreur il n'y a pas asser d'arbre\n");  // message d'erreur
		game_delete(g);															   // on s'arrete
		return EXIT_FAILURE;													   // erreur test
	}

	if (game_won(g)) {															  // si la game est won (ce qui ne doit pas etre le cas puisque la game est vierge)
		fprintf(stderr, "test_game_default erreur la game est gagner d'avance");  // message d'erreur
		game_delete(g);															  // on s'arrete
		return EXIT_FAILURE;													  // erreur test
	}

	// si tout les test sont passer
	game_delete(g);		  // on s'arrete
	return EXIT_SUCCESS;  // test reussi
}

int test_game_print() {	 // test game_print (je l'ai test sans vraiment pouvoir verifier)

	game g1 = game_new_empty();	 // creation d'une variable game g1 a qui on affecte une game vide
	assert(g1 != NULL);

	game_set_square(g1, 0, 1, TREE);  // on met un arbre en 0 1
	game_set_square(g1, 2, 3, TREE);  // on met un arbre en 2 3
	game_print(g1);					  // on affiche la board
	game_delete(g1);				  // on supprime g1

	game g2 = game_default();  // creation d'une variable game g2 a qui on affecte une game par default
	assert(g2 != NULL);

	game_set_square(g2, 0, 1, TENT);  // on met une tent en 0 1
	game_set_square(g2, 2, 3, TENT);  // on met une tent en 2 3
	game_print(g2);					  // on affiche la board
	game_delete(g2);				  // on supprime g2

	// Tout a marcher(on le suppose puisqu'on ne peu pas vraiment le test autrement que visuelment)
	return EXIT_SUCCESS;  // test reussi
}

int test_restart_game() {  // test restart_game

	game g1 = game_default();  // creation d'une variable game g1 a qui on affecte une game par default
	assert(g1 != NULL);

	game g2 = game_copy(g1);  // creation d'une variable game g2 a qui on affecte la copi de g1
	assert(g2 != NULL);

	game_set_square(g1, 1, 1, TENT);   // on place une tent en 1 1
	game_set_square(g1, 2, 2, GRASS);  // on place de l'herbe en 2 2

	game_restart(g1);  // on restart pour revenir au condition de base

	if (!game_equal(g1, g2)) {																   // si g1 n'est pas egal a g2(donc qu'il y a toujours l'herbe ou la tent)
		fprintf(stderr, "test_game_restart erreur: restart ne rend pas la game identique\n");  // message d'erreur
		game_delete(g1);																	   // on s'arrete
		game_delete(g2);																	   // on s'arrete
		return EXIT_FAILURE;																   // erreur test
	}

	// si tout les test sont reussi
	game_delete(g1);	  // message d'erreur
	game_delete(g2);	  // message d'erreur
	return EXIT_SUCCESS;  // test reussi
}

int test_undo_redo() {
	game g = game_new_empty_ext(8, 8, false);  // creation du game g qui pointe sur la game
	assert(g != NULL);

	game g1 = game_copy(g);	 // creation du game g1 qui pointe sur la game par default
	assert(g1 != NULL);

	// on effectue des changements sur la board
	// on ajoute une tent en 0 1
	game_set_square(g, 0, 1, TENT);

	game g2 = game_copy(g);	 // pour test le redo apres
	assert(g2 != NULL);

	game_undo(g);

	if (!game_equal(g, g1)) {					 // si la game n'est pas redevenue comme avant
		fprintf(stderr, "ERREUR :  test undo");	 // message d'erreur
		game_delete(g);							 // on supprime les games
		game_delete(g1);
		game_delete(g2);
		return EXIT_FAILURE;  // erreur test
	}

	game_redo(g);

	if (!game_equal(g, g2)) {					 // si la game n'est pas redevenue comme avant
		fprintf(stderr, "ERREUR :  test undo");	 // message d'erreur
		game_delete(g);							 // on supprime les games
		game_delete(g1);
		game_delete(g2);
		return EXIT_FAILURE;  // erreur test
	}

	game_delete(g);	 // on supprime les games
	game_delete(g1);
	game_delete(g2);
	return EXIT_SUCCESS;  // test reussi
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "parametre insuffisance\n");  // message d'erreur
		return EXIT_FAILURE;						  // erreur parametre
	}

	if (strcmp("dummy", argv[1]) == 0) {
		return test_dummy();
	}

	if (strcmp("default_solution", argv[1]) == 0) {
		return test_default_solution();
	}

	if (strcmp("game_is_neigh", argv[1]) == 0) {
		return test_game_is_neigh();
	}

	if (strcmp("game_get_adjacent_square", argv[1]) == 0) {
		return test_game_get_adjacent_square();
	}

	if (strcmp("game_default", argv[1]) == 0) {
		return test_game_default();
	}

	if (strcmp("game_print", argv[1]) == 0) {
		return test_game_print();
	}

	if (strcmp("game_restart", argv[1]) == 0) {
		return test_restart_game();
	}

	if (strcmp("undo_redo", argv[1]) == 0) {
		return test_restart_game();
	}

	fprintf(stderr, "test inconnu\n");	// message d'erreurk
	return EXIT_FAILURE;				// erreur test
}
