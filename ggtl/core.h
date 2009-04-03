/*
 * GGTL - 2-player strategic games AI
 * Copyright (C) 2005-2006 Stig Brautaset
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ggtl__core_h
#define ggtl__core_h

#ifdef __cplusplus      /* let C++ coders use this library */
extern "C" {
#endif

/* cache flags */
#define STATES  1
#define MOVES   2

enum {          /* keys valid for ggtl_set() */
  TYPE = 0,     /* AI type */
  PLY,          /* depth for fixed-depth AI */
  MSEC,         /* milliseconds for iterative AI (deprecated in 2.1.2) */
  TRACE,        /* tracing level */
  CACHE,        /* what to cache */
  TIME,		/* seconds (double) for iterative AI */
  SET_KEYS,
};
enum {          /* additional keys valid for ggtl_get() */
  VISITED = SET_KEYS, /* number of states visited during last search */
  PLY_REACHED,  /* depth reached by last iterative search */
  GET_KEYS,
};

/* AI types */
enum { 
  NONE = 0,
  RANDOM,
  FIXED,
  ITERATIVE,
};

/* fitness limits */
#include <limits.h>
#define GGTL_FITNESS_MAX (INT_MAX-10)
#define GGTL_FITNESS_MIN (-GGTL_FITNESS_MAX)
#define FITNESS_MAX GGTL_FITNESS_MAX  /* backward compat */
#define FITNESS_MIN GGTL_FITNESS_MIN  /* backward compat */

typedef struct ggtl GGTL;

typedef struct ggtl_sc {
  struct ggtl_sc *next;
  void *data;
} GGTL_STATE;

typedef struct ggtl_mc {
  struct ggtl_mc *next;
  int fitness;
  void *data;
} GGTL_MOVE;

typedef struct ggtl_vtab {
  int (*eval)(void *, GGTL *);
  int (*game_over)(void *, GGTL *);
  GGTL_MOVE *(*get_moves)(void *, GGTL *);
  void *(*move)(void *, void *, GGTL *);
  void *(*unmove)(void *, void *, GGTL *);
  void *(*clone_state)(void *, GGTL *);
  void (*free_state)(void *);
  void (*free_move)(void *);
} GGTL_VTAB;

/* ggtl/core.c */
GGTL *ggtl_new(void);
GGTL_STATE *ggtl_sc_new(void *);
GGTL_MOVE *ggtl_mc_new(void *);
GGTL *ggtl_init(GGTL *g, void *s);
GGTL_VTAB *ggtl_vtab(GGTL *g);
void *ggtl_peek_state(GGTL *g);
void *ggtl_peek_move(GGTL *g);
void *ggtl_move(GGTL *g, void *m);
void *ggtl_ai_move(GGTL *g);
int ggtl_ai_visited(GGTL *g);
GGTL_MOVE *ggtl_get_moves(GGTL *g);
GGTL_STATE *ggtl_move_internal(GGTL *g, GGTL_MOVE *m);
GGTL_MOVE *ggtl_undo_internal(GGTL *g);
void *ggtl_undo(GGTL *g);
void ggtl_cache_states(GGTL *g, GGTL_STATE *state);
void ggtl_cache_state(GGTL *g, void *state);
void ggtl_cache_moves(GGTL *g, GGTL_MOVE *move);
void ggtl_cache_move(GGTL *g, void *move);
void ggtl_cache_free(GGTL *g);
GGTL_STATE *ggtl_wrap_state(GGTL *g, void *s);
GGTL_MOVE *ggtl_wrap_move(GGTL *g, void *m);
GGTL_STATE *ggtl_uncache_state(GGTL *g);
GGTL_MOVE *ggtl_uncache_move(GGTL *g);
void *ggtl_uncache_state_raw(GGTL *g);
void *ggtl_uncache_move_raw(GGTL *g);
int ggtl_game_over(GGTL *g);
int ggtl_eval(GGTL *g);
void ggtl_free(GGTL *g);
void ggtl_set(GGTL *g, int key, int value);
int ggtl_get(GGTL *g, int key);
void ggtl_setval(GGTL *g, int key, ...);
void ggtl_getval(GGTL *g, int key, ...);

#ifdef __cplusplus
}
#endif
#endif	/* !ggtl__core_h */

