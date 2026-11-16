#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H
#include <assert.h>
#include <stdlib.h>

#include "game.h"
struct history {
	square** board;
	struct history* next;
};

typedef struct history* lhistory;


struct game_s {
	square** board;
	uint* rows;
	uint* cols;
	uint nrows;
	uint ncols;
	bool wrapping;

	// undo
	lhistory undo;
	lhistory redo;
};

#endif
