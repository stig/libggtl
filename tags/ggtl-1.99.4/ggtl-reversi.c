/*

=head1 NAME

GGTL-Reversi- GGTL extension for playing Reversi (aka Othello)

=head1 SYNOPSIS

  #include <ggtl/reversi.h>
  
  GGTL *reversi_init(GGTL *g, void *s);
  GGTL_STATE *reversi_state_clone(RState *s, GGTL *g);
  RMove *reversi_move_new(int x, int y, GGTL *g);
  RState *reversi_state_new(int size);
  void reversi_state_draw(RState *s);
  RStateCount reversi_state_count(RState *s);
  
  # callback functions used by ggtl core:
  int reversi_eval(void *state, GGTL *g);
  void reversi_state_free(void *state);
  GGTL_MOVE *reversi_get_moves(void *s, GGTL *g);
  GGTL_STATE *reversi_move(void *s, void *m, GGTL *g);

See L<reversi-demo(3)|reversi-demo> for a complete example of a
self-playing Reversi game using this extension.

=head1 DESCRIPTION

GGTL-Reversi is a GGTL extension for playing Reversi (aka
Othello). It provides all the callback functions and
datastructures GGTL needs to provide a Reversi AI, leaving you
free to worry about other aspects of the game. 

All even board sizes greater than 4x4 are supported and can be
specified at run time.

=head1 DATA STRUCTURES

Three data structures are used by this GGTL extension. They are:

=over

=item RState

  typedef struct reversi_state {
    int player;
    int size;
    int **board;
  } RState;

=item RMove

  typedef struct reversi_move {
    int x;
    int y;
  } RMove;

=item RStateCount

  typedef struct reversi_counts {
    int c[3];
  } RStateCount;

=back

=cut

*/

#include "ggtl/reversi.h"
#include "config.h"

#include <sl/sl.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static int move_internal(RState *s, int x, int y);
static int valid_move(RState *s, int me, int x, int y);

/*

=head1 FUNCTIONS

=over

=item RState *reversi_state_new( int size )

Returns a reversi state with a board of the desired size, or NULL
on failure. The board is set up for the beginning of a game and
player 1 is set to start.

=cut

*/

RState *reversi_state_new(int size)
{
  RState *state;

  if (size < 4 || size % 2) {
    return NULL;
  }

  state = malloc(sizeof *state);
  if (state) {
    state->board = malloc(size * (sizeof(int*)));
    if (state->board) {
      state->board[0] = malloc(size * size * (sizeof(int)));
      if (state->board[0]) {
        int i, j;
        for (i = 1; i < size; i++) {
          state->board[i] = state->board[0] + i * size;
        }
        for (i = 0; i < size; i++) {
          for (j = 0; j < size; j++) {
            state->board[i][j] = 0;
          }
        }
      }
    }
  }

  if (state && state->board && state->board[0]) {
    state->size = size;
    state->player = 1;
    state->board[size/2-1][size/2] = 1;
    state->board[size/2][size/2-1] = 1;
    state->board[size/2-1][size/2-1] = 2;
    state->board[size/2][size/2] = 2;
  }
  else if (state && state->board) {
    free(state->board);
    free(state);
    state = NULL;
  }
  else {
    free(state);
    state = NULL;
  }

  return state;
}

/*

=item GGTL_STATE *reversi_state_clone( RState *s, GGTL *g )

Clone C<s>, using a cached state from C<g> if available. Return
the cloned state wrapped in a C<GGTL_STATE>, or NULL on
error. 

It is assumed that cached states are the same size as the one
being cloned.

=cut

*/

GGTL_STATE *reversi_state_clone( RState *s, GGTL *g )
{
  RState *clone;
  GGTL_STATE *node;

  node = ggtl_uncache_state(g);
  if (!node) {
    clone = reversi_state_new(s->size);
    node = clone ? ggtl_sc_new(clone) : NULL;
    if (!node) {
      reversi_state_free(clone);
    }
  }

  if (node) {
    int i, j;
    clone = node->data;
    clone->player = s->player;
    for (i = 0; i < s->size; i++) {
      for (j = 0; j < s->size; j++) {
        clone->board[i][j] = s->board[i][j];
      }
    }
  }

  return node;
}


/*

=item void reversi_state_draw( RState *s )

Print a plain-text representation of a state to standard out.

=cut

*/

void reversi_state_draw(RState *s)
{
  char p[] = ".xo";
  int i, j;

  for (i = 0; i < s->size; i++) {
    for (j = 0; j < s->size; j++) {
      putchar((int) p[ s->board[i][j] ]);
    }
    if (i == (s->size - 1)) {
      printf(" - %c to move", p[s->player]);
    }
    putchar('\n');
  }
}

/*

=item RMove *reversi_move_new( int x, int y )

Returns a new move, or NULL on failure. 

=cut

*/

RMove *reversi_move_new(int x, int y)
{
  RMove *m;

  m = malloc(sizeof *m);
  if (m) {
    m->x = x;
    m->y = y;
  }

  return m;
}


/*

=item GGTL_MOVE *reversi_move_new_wrapped( int x, int y, *g )

Returns a new move, wrapped in a C<ggtl_mc> container, or NULL
on failure. If g is non-NULL it will be asked for a cached move;
otherwise, a new move will be allocated.

=cut

*/

GGTL_MOVE *reversi_move_new_wrapped(int x, int y, GGTL *g)
{
  GGTL_MOVE *n;

  n = g ? ggtl_uncache_move(g) : NULL;
  n = n ? n : ggtl_mc_new(NULL);
  if (n) {
    RMove *m;
    n->data = n->data ? n->data : malloc(sizeof *m);
    if (n->data) {
      m = n->data;
      m->x = x;
      m->y = y;
    }
    else {
      free(n);
      n = NULL;
    }
  }

  return n;
}

/*

=item GGTL *reversi_init(GGTL *g, void *state)

Initialises a GGTL structure's vtable with functions for playing
Reversi and set the provided state to be the starting state of the
game.

=cut

*/

GGTL *reversi_init(GGTL *g, void *s)
{
  ggtl_vtab(g)->move = &reversi_move;
  ggtl_vtab(g)->get_moves = &reversi_get_moves;
  ggtl_vtab(g)->eval = &reversi_eval;
  ggtl_vtab(g)->free_state = &reversi_state_free;
  
  return ggtl_init(g, s);
}

/*

=item RStateCount reversi_state_count(RState *s)

Returns a structure containing the counts of empty, white & black
squares in the given state.

=cut

*/

RStateCount reversi_state_count(RState *s)
{
  int i, j;
  RStateCount count = {{0}};

  for (i = 0; i < s->size; i++) {
    for (j = 0; j < s->size; j++) {
      count.c[ s->board[i][j] ]++;
    }
  }

  return count;
}



/*

=back

=head1 CALLBACK FUNCTIONS

These functions are used internally by GGTL; it is unlikely that
you should have to worry about them. However, you I<are> allowed
to override these should you wish. See
L<ggtl_vtab()|ggtl/ggtl_vtab>.

=over

=item int reversi_eval( void *state, GGTL *g )

Evaluate a reversi state and return its fitness.

=cut

*/

int reversi_eval( void *state, GGTL *g )
{
  RState *s = state;
  GGTL_MOVE *moves;
  int mine, diff, me, you;
  struct reversi_counts counts;

  me = s->player;
  you = 3 - me;

  moves = ggtl_get_moves(g);
  if (!moves) {
    counts = reversi_state_count(s);
    mine = counts.c[me] - counts.c[you];
    return mine > 0 ? FITNESS_MAX : 
           mine < 0 ? FITNESS_MIN : 0;
  }

  mine = sl_count(moves);
  ggtl_cache_moves(g, moves);

  s->player = 3 - s->player;
  moves = ggtl_get_moves(g);
  s->player = 3 - s->player;

  diff = mine - sl_count(moves);
  ggtl_cache_moves(g, moves);

  counts = reversi_state_count(s);
  mine = counts.c[me] - counts.c[you];

  return diff + mine;
}

/*

=item GGTL_STATE *reversi_move( void *state, void *move, GGTL *g )

Returns the state resulting from applying C<move> to C<state>, or
NULL on failure.

=cut

*/

GGTL_STATE *reversi_move( void *state, void *move, GGTL *g )
{
  GGTL_STATE *node;

  node = reversi_state_clone(state, g);
  if (node) {
    RMove *m = move;

    if (!move_internal(node->data, m->x, m->y)) {
      ggtl_cache_states(g, node);
      node = NULL;
    }
  }

  return node;
}

static int move_internal(RState *s, int x, int y)
{
  int me = s->player;
  int not_me = 3 - me;
  int tx, ty, flipped = 0;

  s->player = not_me;

  if (x == -1 && y == -1) {
    return 1;
  }
  else if (x < 0 || x > (s->size-1) || y < 0 || y > (s->size-1)) {
    return 0;
  } 
  else if (s->board[x][y] != 0) {
    return 0;
  }

  /* left */
  for (tx = x - 1; tx >= 0 && s->board[tx][y] == not_me; tx--)
    ;
  if (tx >= 0 && tx != x - 1 && s->board[tx][y] == me) {
    tx = x - 1;
    while (tx >= 0 && s->board[tx][y] == not_me) {
      s->board[tx][y] = me;
      tx--;
    }
    flipped++;
  }

  /* right */
  for (tx = x + 1; tx < s->size && s->board[tx][y] == not_me; tx++)
    ;
  if (tx < s->size && tx != x + 1 && s->board[tx][y] == me) {
    tx = x + 1;
    while (tx < s->size && s->board[tx][y] == not_me) {
      s->board[tx][y] = me;
      tx++;
    }
    flipped++;
  }

  /* up */
  for (ty = y - 1; ty >= 0 && s->board[x][ty] == not_me; ty--)
    ;
  if (ty >= 0 && ty != y - 1 && s->board[x][ty] == me) {
    ty = y - 1;
    while (ty >= 0 && s->board[x][ty] == not_me) {
      s->board[x][ty] = me;
      ty--;
    }
    flipped++;
  }
  
  /* down */
  for (ty = y + 1; ty < s->size && s->board[x][ty] == not_me; ty++)
    ;
  if (ty < s->size && ty != y + 1 && s->board[x][ty] == me) {
    ty = y + 1;
    while (ty < s->size && s->board[x][ty] == not_me) {
      s->board[x][ty] = me;
      ty++;
    }
    flipped++;
  }
  
  /* up/left */
  tx = x - 1;
  ty = y - 1; 
  while (tx >= 0 && ty >= 0 && s->board[tx][ty] == not_me) {
    tx--; ty--;
  }
  if (tx >= 0 && ty >= 0 && tx != x - 1 && ty != y - 1 && 
      s->board[tx][ty] == me) {
    tx = x - 1;
    ty = y - 1;
    while (tx >= 0 && ty >= 0 && s->board[tx][ty] == not_me) {
      s->board[tx][ty] = me;
      tx--; ty--;
    }
    flipped++;
  }

  /* up/right */
  tx = x - 1;
  ty = y + 1; 
  while (tx >= 0 && ty < s->size && s->board[tx][ty] == not_me) {
    tx--; ty++;
  }
  if (tx >= 0 && ty < s->size && tx != x - 1 && ty != y + 1 && 
      s->board[tx][ty] == me) {
    tx = x - 1;
    ty = y + 1;
    while (tx >= 0 && ty < s->size && s->board[tx][ty] == not_me) {
      s->board[tx][ty] = me;
      tx--; ty++;
    }
    flipped++;
  }
  
  /* down/right */
  tx = x + 1;
  ty = y + 1; 
  while (tx < s->size && ty < s->size && s->board[tx][ty] == not_me) {
    tx++; ty++;
  }
  if (tx < s->size && ty < s->size && tx != x + 1 && ty != y + 1 && 
      s->board[tx][ty] == me) {
    tx = x + 1;
    ty = y + 1;
    while (tx < s->size && ty < s->size && s->board[tx][ty] == not_me) {
      s->board[tx][ty] = me;
      tx++; ty++;
    }
    flipped++;
  }

  /* down/left */
  tx = x + 1;
  ty = y - 1;
  while (tx < s->size && ty >= 0 && s->board[tx][ty] == not_me) {
    tx++; ty--;
  }
  if (tx < s->size && ty >= 0 && tx != x + 1 && ty != y - 1 && 
      s->board[tx][ty] == me) {
    tx = x + 1;
    ty = y - 1;
    while (tx < s->size && ty >= 0 && s->board[tx][ty] == not_me) {
      s->board[tx][ty] = me;
      tx++; ty--;
    }
    flipped++;
  }

  if (flipped) {
    s->board[x][y] = me;
  }
  return flipped;
}


/*

=item GGTL_MOVE *reversi_get_moves( void *state, GGTL *g )

Returns a list of the available moves at the given position, or
NULL if no moves could be found.

=cut

*/

GGTL_MOVE *reversi_get_moves( void *state, GGTL *g )
{
  RState *s = state;
  GGTL_MOVE *moves;
  int me, i, j;

  me = s->player;
  moves = NULL;
again:
  for (i = 0; i < s->size; i++) {
    for (j = 0; j < s->size; j++) {
      if (valid_move(s, me, i, j)) {
        GGTL_MOVE *n = reversi_move_new_wrapped(i, j, g); 
        assert(n != NULL);
        moves = sl_push(moves, n);
      }
    }
  }

  if (!moves) {
    if (me == s->player) {
      me = 3 - me;
      goto again;
    }
  }
  else if (me != s->player) {
    GGTL_MOVE *n = sl_pop(&moves);
    RMove *m = n->data;
    ggtl_cache_moves(g, moves);
    moves = n;
    m->x = m->y = -1;
  }

  return moves;
}

static int valid_move(RState *s, int me, int x, int y)
{
  int tx, ty;
  int not_me = 3 - me;

  /* slot must not already be occupied */
  if (s->board[x][y] != 0)
    return 0;

  /* left */
  for (tx = x - 1; tx >= 0 && s->board[tx][y] == not_me; tx--)
    ;
  if (tx >= 0 && tx != x - 1 && s->board[tx][y] == me) 
    return 1;

  /* right */
  for (tx = x + 1; tx < s->size && s->board[tx][y] == not_me; tx++)
    ;
  if (tx < s->size && tx != x + 1 && s->board[tx][y] == me)
    return 1;

  /* up */
  for (ty = y - 1; ty >= 0 && s->board[x][ty] == not_me; ty--)
    ;
  if (ty >= 0 && ty != y - 1 && s->board[x][ty] == me) 
    return 1;
  
  /* down */
  for (ty = y + 1; ty < s->size && s->board[x][ty] == not_me; ty++)
    ;
  if (ty < s->size && ty != y + 1 && s->board[x][ty] == me) 
    return 1;
  
  /* up/left */
  tx = x - 1;
  ty = y - 1; 
  while (tx >= 0 && ty >= 0 && s->board[tx][ty] == not_me) {
    tx--; ty--;
  }
  if (tx >= 0 && ty >= 0 && tx != x - 1 && ty != y - 1 && 
      s->board[tx][ty] == me)
    return 1;

  /* up/right */
  tx = x - 1;
  ty = y + 1; 
  while (tx >= 0 && ty < s->size && s->board[tx][ty] == not_me) {
    tx--; ty++;
  }
  if (tx >= 0 && ty < s->size && tx != x - 1 && ty != y + 1 && 
      s->board[tx][ty] == me)
    return 1;
  
  /* down/right */
  tx = x + 1;
  ty = y + 1; 
  while (tx < s->size && ty < s->size && s->board[tx][ty] == not_me) {
    tx++; ty++;
  }
  if (tx < s->size && ty < s->size && tx != x + 1 && ty != y + 1 && 
      s->board[tx][ty] == me) 
    return 1;

  /* down/left */
  tx = x + 1;
  ty = y - 1;
  while (tx < s->size && ty >= 0 && s->board[tx][ty] == not_me) {
    tx++; ty--;
  }
  if (tx < s->size && ty >= 0 && tx != x + 1 && ty != y - 1 && 
      s->board[tx][ty] == me)
    return 1;

  return 0;
}

/*

=item void reversi_state_free( void *state )

Free up the memory held up by a state. This overrides the default
free_state() callback supplied by GGTL.

=cut

*/

void reversi_state_free(void *state)
{
  RState *s = state;
  free(s->board[0]);
  free(s->board);
  free(s);
}


/*

=back

=head1 SEE ALSO

L<ggtl(3)|ggtl>

=head1 THANKS

The code for performing a move was inspired by code found in
Gnome Iagno by Ian Peters.

=head1 AUTHOR

Stig Brautaset <stig@brautaset.org>

=head1 COPYRIGHT

Copyright (C) 2005 Stig Brautaset

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

=cut

*/
