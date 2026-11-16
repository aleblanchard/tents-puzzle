// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"

#include <SDL.h>
#include <SDL_image.h>	// required to load transparent texture from PNG
#include <SDL_ttf.h>	// required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

#define FONT "res/DejaVuSans.ttf"
#define FONTSIZE 36
#define SIZE_MARGE 35
#define TXT_SIZE 99	 // grand nombre
/* **************************************************************** */

struct Env_t {
	game g;
	SDL_Texture *background;
	SDL_Texture *tree;
	SDL_Texture *tent;
	SDL_Texture *grass;
	SDL_Texture *bigtree;
	TTF_Font *font;
	bool detection_erreurs;
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
	Env *env = malloc(sizeof(struct Env_t));
	if (argc > 1) {
		env->g = game_load(argv[1]);  // grille chargée depuis fichier
	} else {
		env->g = game_default();  // sinon grille par défaut
	}
	env->background = IMG_LoadTexture(ren, "res/background.png");
	env->tree = IMG_LoadTexture(ren, "res/tree.png");
	env->tent = IMG_LoadTexture(ren, "res/tent.png");
	env->grass = IMG_LoadTexture(ren, "res/grass.png");
	env->bigtree = IMG_LoadTexture(ren, "res/bigtree.png");
	env->detection_erreurs = false;

	env->font = TTF_OpenFont(FONT, FONTSIZE);
	if (!env->font) ERROR("TTF_OpenFont: %s\n", FONT);
	return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
	int w, h;
	SDL_GetWindowSize(win, &w, &h);	 // recupère la taille de la fenetre (demo.c)

	SDL_RenderCopy(ren, env->background, NULL, NULL);  // texture du background


	uint nrows = game_nb_rows(env->g);
	uint ncols = game_nb_cols(env->g);

	int casew = (w - SIZE_MARGE) / ncols;
	int caseh = (h - SIZE_MARGE) / nrows;  // taille d'une case, laisse 40 pixels de SIZE_MARGE pour les compteurs

	// grille
	for (int x = 0; x < ncols * casew; x = x + casew) {	  // saute de case en case en largeur
		SDL_RenderDrawLine(ren, x, 0, x, nrows * caseh);  // dessine les lignes verticale
	}

	for (int y = 0; y < nrows * caseh; y = y + caseh) {	  // saute de case en case en hauteur
		SDL_RenderDrawLine(ren, 0, y, ncols * casew, y);  // dessine les lignes verticales
	}

	SDL_Rect rect;

	for (uint i = 0; i < nrows; i++) {	// cases
		for (uint j = 0; j < ncols; j++) {
			square c = game_get_square(env->g, i, j);
			rect.x = j * casew;
			rect.y = i * caseh;
			rect.w = casew;
			rect.h = caseh;

			if (env->detection_erreurs) {
				if (game_get_current_nb_tents_row(env->g, i) > game_get_expected_nb_tents_row(env->g, i) || game_get_current_nb_tents_col(env->g, j) > game_get_expected_nb_tents_col(env->g, j)) {
					SDL_SetRenderDrawColor(ren, 255, 0, 0, 0.8);
					SDL_RenderDrawLine(ren, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
				}
			}

			if (c == TREE) {
				SDL_RenderCopy(ren, env->tree, NULL, &rect);
			} else if (c == GRASS) {
				SDL_RenderCopy(ren, env->grass, NULL, &rect);
			} else if (c == TENT) {
				SDL_RenderCopy(ren, env->tent, NULL, &rect);
			} else if (c == BIGTREE) {
				SDL_RenderCopy(ren, env->bigtree, NULL, &rect);
			}
		}
	}
	for (uint i = 0; i < nrows; i++) {	// compteurs par ligne
		char txt[TXT_SIZE];
		sprintf(txt, "%u", game_get_expected_nb_tents_row(env->g, i));
		SDL_Color color = {238, 236, 78, 0.8};
		SDL_Surface *surf = TTF_RenderText_Blended(env->font, txt, color);
		SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);

		SDL_QueryTexture(tex, NULL, NULL, &rect.w, &rect.h);
		rect.x = ncols * casew + 1;
		rect.y = i * caseh + caseh / 2 - rect.h / 2;
		SDL_RenderCopy(ren, tex, NULL, &rect);
	}
	for (uint i = 0; i < ncols; i++) {	// compteurs par ligne
		char txt[TXT_SIZE];
		sprintf(txt, "%u", game_get_expected_nb_tents_col(env->g, i));
		SDL_Color color = {238, 236, 78, 0.8};
		SDL_Surface *surf = TTF_RenderText_Blended(env->font, txt, color);
		SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);

		SDL_QueryTexture(tex, NULL, NULL, &rect.w, &rect.h);
		rect.y = nrows * caseh + 1;
		rect.x = i * casew + casew / 2 - rect.w / 2;
		SDL_RenderCopy(ren, tex, NULL, &rect);
	}

	if (game_won(env->g)) {
		char txt[] = "Victoire !";
		SDL_Color color = {238, 236, 78, 0.8};
		SDL_Surface *surf = TTF_RenderText_Blended(env->font, txt, color);
		SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
		SDL_QueryTexture(tex, NULL, NULL, &rect.w, &rect.h);
		rect.x = w / 2 - rect.w / 2;  // centre horizontal
		rect.y = h / 2 - rect.h / 2;  // centre vertical
		SDL_RenderCopy(ren, tex, NULL, &rect);
	}
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
	if (e->type == SDL_QUIT) {	// quitter
		return true;
	}

	// raccourcis clavier
	if (e->type == SDL_KEYDOWN) {
		if (e->key.keysym.sym == SDLK_q) {
			return true;  // quitter (touche q)
		}
		if (e->key.keysym.sym == SDLK_r) {
			game_restart(env->g);  // restart (touche r)
		}
		if (e->key.keysym.sym == SDLK_z) {
			game_undo(env->g);	// undo (touche z)
		}
		if (e->key.keysym.sym == SDLK_y) {
			game_redo(env->g);	// redo (touche y)
		}
		if (e->key.keysym.sym == SDLK_a) {
			game_solve(env->g);	 // solver (touche a)
		}
		if (e->key.keysym.sym == SDLK_f) {
			game_fill_grass(env->g);  // fill grass (touche f)
		}
		if (e->key.keysym.sym == SDLK_s) {	// sauvegarder la partie (touche s)
			game_save(env->g, "saved_game.txt");
		}
		if (e->key.keysym.sym == SDLK_d) {	// detection erreur (touche d)
			if (env->detection_erreurs == true) {
				env->detection_erreurs = false;
			} else {
				env->detection_erreurs = true;
			}
		}
	}

	// souris
	if (e->type == SDL_MOUSEBUTTONDOWN) {
		SDL_Point mouse;  // recuperer la pos du clic
		SDL_GetMouseState(&mouse.x, &mouse.y);

		int w, h;
		SDL_GetWindowSize(win, &w, &h);	 // taille de la fenetre
		int casew = (w - SIZE_MARGE) / game_nb_cols(env->g);
		int caseh = (h - SIZE_MARGE) / game_nb_rows(env->g);  // gérer le décalage souris avec la SIZE_MARGE par rapport a la fenetre
		int j = mouse.x / casew;
		int i = mouse.y / caseh;

		square c = game_get_square(env->g, i, j);


		if (c != TREE && c != BIGTREE) {				// pas touche aux arbres
			if (e->button.button == SDL_BUTTON_LEFT) {	// clic gauche, pose une tente ou supprime si quelque chose est deja posé
				if (c == EMPTY) {
					game_play_move(env->g, i, j, TENT);
				}  // tente
				else {
					game_play_move(env->g, i, j, EMPTY);  // deja un objet, met vide
				}
			} else if (e->button.button == SDL_BUTTON_RIGHT) {	// clic droit, pose herbe ou supp
				if (c == EMPTY) {
					game_play_move(env->g, i, j, GRASS);  // vide, tente
				} else {
					game_play_move(env->g, i, j, EMPTY);  // deja un objet, supprime
				}
			}
			if (game_won(env->g)) {
				printf("Victoire!");
			}
		}
	}
	return false;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
	SDL_DestroyTexture(env->background);
	SDL_DestroyTexture(env->tree);
	SDL_DestroyTexture(env->tent);
	SDL_DestroyTexture(env->grass);
	SDL_DestroyTexture(env->bigtree);
	TTF_CloseFont(env->font);
	game_delete(env->g);
	free(env);
}

/* **************************************************************** */