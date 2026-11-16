# Tents — jeu de logique en C (SDL2 + WebAssembly)

[![CI](https://github.com/aleblanchard/tents-puzzle/actions/workflows/ci.yml/badge.svg)](https://github.com/aleblanchard/tents-puzzle/actions/workflows/ci.yml)
![C99](https://img.shields.io/badge/C-C99-A8B9CC?logo=c&logoColor=white)
![SDL2](https://img.shields.io/badge/SDL2-interface%20graphique-blue)
![WebAssembly](https://img.shields.io/badge/WebAssembly-Emscripten-654FF0?logo=webassembly&logoColor=white)

**▶ [Jouer dans le navigateur](https://aleblanchard.github.io/tents-puzzle/)**

Implémentation du casse-tête **Tents** réalisée en équipe de quatre (équipe B33) dans le cadre d'un projet technologique à l'Université de Bordeaux : une bibliothèque de jeu en C, un solveur, un générateur de grilles, une version texte, une interface graphique SDL2 et une version web compilée en WebAssembly.

## Règles du jeu

Placer des tentes sur la grille de façon à ce que :

- chaque arbre ait une tente adjacente (un **gros arbre** en demande deux) ;
- aucune tente n'en touche une autre, même en diagonale ;
- le nombre de tentes de chaque ligne et de chaque colonne corresponde aux indications en marge.

## Fonctionnalités

- Bibliothèque `game` (API publique dans `game.h`, `game_aux.h`, `game_ext.h`)
- Grilles de taille variable, option *wrapping* (bords qui se rejoignent), gros arbres
- Annuler / refaire, sauvegarde et chargement de parties
- Solveur (`game_solve`) et générateur aléatoire (`game_random`)
- Interface graphique SDL2 avec détection des mauvais coups
- Version web : la bibliothèque C compilée avec Emscripten, interface en HTML/CSS/JS
- 66 tests unitaires exécutés avec CTest

## Compilation

Prérequis : un compilateur C, CMake ≥ 3.5, SDL2, SDL2_image et SDL2_ttf.

```bash
# Debian / Ubuntu
sudo apt install build-essential cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

mkdir build && cd build
cmake ..
make
ctest            # lance les tests unitaires
```

## Utilisation

```bash
./game_sdl [partie.txt]     # interface graphique
./game_text [partie.txt]    # version terminal
./game_solve -s|-c <entrée> [<sortie>]   # -s : résoudre, -c : compter les solutions
./game_random <lignes> <colonnes> <wrapping> <nb_arbres> <nb_gros_arbres> <solution> [<fichier>]
```

### Commandes de l'interface graphique

| Action | Effet |
| --- | --- |
| Clic gauche | Pose une tente (ou efface la case) |
| Clic droit | Pose de l'herbe (ou efface la case) |
| `Z` / `Y` | Annuler / refaire |
| `R` | Recommencer |
| `A` | Afficher la solution |
| `F` | Remplir les cases vides d'herbe |
| `S` | Sauvegarder la partie |
| `D` | Activer la détection des mauvais coups |
| `Q` | Quitter |

### Version web en local

```bash
python3 -m http.server
# puis ouvrir http://localhost:8000/web/game_web.html
```

## Structure

```
.
├── game*.c / game*.h     # bibliothèque du jeu
├── game_text.c           # interface texte
├── game_solve.c          # solveur
├── game_random.c         # générateur
├── main.c, model.c/.h    # interface SDL2
├── game_test_*.c         # tests unitaires (un fichier par membre)
├── res/                  # images et police
└── web/                  # version WebAssembly
```

## Équipe

- **Alexis Blanchard**
- **Gauthier Bongrain**
- **Félix Claus**
- **Thomas Laurans**

## Crédits

- Base de l'interface SDL2 issue de la démo fournie par l'équipe pédagogique (A. Esnard).
- Images générées avec Nano Banana 2.
- Police [DejaVu Sans](https://dejavu-fonts.github.io/) (licence libre).
