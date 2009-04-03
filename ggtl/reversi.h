/*
 * Copyright (C) 2005-2006 Stig Brautaset
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ggtl__reversi_h
#define ggtl__reversi_h

#include <ggtl/core.h>

#ifdef __cplusplus      /* let C++ coders use this library */
extern "C" {
#endif

typedef struct reversi_state {
  int player;
  int size;
  int **board;
} RState;

typedef struct reversi_move {
  int x;
  int y;
} RMove;

typedef struct reversi_counts {
  int c[3];
} RStateCount;

/* ggtl/reversi.h */
GGTL *reversi_init(GGTL *g, void *s);
void *reversi_move(void *s, void *m, GGTL *g);
GGTL_MOVE *reversi_get_moves(void *s, GGTL *g);
int reversi_eval(void *state, GGTL *g);
RState *reversi_state_new(int size);
void *reversi_state_clone(void *s, GGTL *g);
RMove *reversi_move_new(int x, int y);
GGTL_MOVE *reversi_move_new_wrapped(int x, int y, GGTL *g);
void reversi_state_free(void *state);
void reversi_state_draw(RState *s);
RStateCount reversi_state_count(RState *s);


#ifdef __cplusplus
}
#endif
#endif	/* !ggtl__reversi_h */

