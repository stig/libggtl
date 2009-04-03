/*
 * Copyright (C) 2005 Stig Brautaset
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ggtl__min_h
#define ggtl__min_h

#include <ggtl/core.h>

struct nim_state {
  int player;
  int value;
};

struct nim_move {
  int value;
};

/* ggtl/nim.h */
GGTL *nim_init(GGTL *g, void *s);
struct nim_state *nim_state_new(int player, int val);
struct nim_move *nim_move_new( int val );
GGTL_STATE *nim_move(void *s, void *m, GGTL *g);
GGTL_MOVE *nim_get_moves(void *s, GGTL *g);
int nim_eval(void *state, GGTL *g);



#endif	/* !ggtl__nim_h */

