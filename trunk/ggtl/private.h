/*
GGTL - 2-player strategic games AI.
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

#ifndef _ggtl_private_h
#define _ggtl_private_h

#define GGTL_ERR (GGTL_FITNESS_MAX+1)

struct ggtl {
  GGTL_VTAB *vtab;

  GGTL_STATE *state_cache;
  GGTL_MOVE *move_cache;
  GGTL_STATE *states;
  GGTL_MOVE *moves;
  GGTL_STATE *sc_cache;
  GGTL_MOVE *mc_cache;

  /* run-time options */
  int opts[GET_KEYS];
  float time_to_search;

  /* optimisation for end of search */
  int saw_end;
};

/* The various AIs */
GGTL_MOVE *ai_none(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_random(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_fixed(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_iterative(GGTL *g, GGTL_MOVE *);


/* Helper functions */
void ai_trace(GGTL *g, int level, char *fmt, ...);
int fitness_cmp(void *anode, void *bnode);

#endif /* !_ggtl_private_h */
