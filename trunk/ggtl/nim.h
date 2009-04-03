/*
GGTL-extension for a simple nim clone.
Copyright (C) 2005-2006 Stig Brautaset. All rights reserved.

This file is part of GGTL.

GGTL is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GGTL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GGTL; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/
#ifndef ggtl__min_h
#define ggtl__min_h

#include <ggtl/core.h>

#ifdef __cplusplus      /* let C++ coders use this library */
extern "C" {
#endif

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
void *nim_move(void *s, void *m, GGTL *g);
void *nim_unmove(void *s, void *m, GGTL *g);
GGTL_MOVE *nim_get_moves(void *s, GGTL *g);
int nim_eval(void *state, GGTL *g);

#ifdef __cplusplus
}
#endif
#endif	/* !ggtl__nim_h */

