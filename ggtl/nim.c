/*

=head1 NAME

GGTL-Nim - GGTL extension for a simple Nim variant

=head1 SYNOPSIS

  #include <ggtl/nim.h>
  
  GGTL *g = ggtl_new_nim();
  struct nim_state *s = ...;
  nim_init(g, s);

=head1 DESCRIPTION

This is a minimal GGTL extension used mainly in GGTL's test suite.

=cut

*/

#include <sl/sl.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "nim.h"

/*

=head1 FUNCTIONS

=over

=item GGTL *nim_init(GGTL *g, void *state)

Initialises a C<GGTL>'s vtable with functions for playing
Nim, and set the provided state to be the starting state of the
game.

=cut

*/

GGTL *nim_init(GGTL *g, void *s)
{
  ggtl_vtab(g)->move = &nim_move;
  ggtl_vtab(g)->unmove = &nim_unmove;
  ggtl_vtab(g)->get_moves = &nim_get_moves;
  ggtl_vtab(g)->eval = &nim_eval;
  
  return ggtl_init(g, s);
}


/*

=item struct nim_state *nim_state_new( int player, int val )

Return a new state with player and value set to the given arguments, or NULL on error.

=cut

*/

struct nim_state *nim_state_new(int player, int val)
{
  struct nim_state *state;
  state = malloc( sizeof *state );
  if (state) {
    state->player = player;
    state->value = val;
  }
  return state;
}

/*

=item nim_move_new( int val )

Return a new nim move, or NULL on error.

=cut

*/

struct nim_move *nim_move_new( int val )
{
  struct nim_move *m;
  m = malloc( sizeof *m );
  if (m) m->value = val; 
  return m;
}


/*

=back

=head1 CALLBACK FUNCTIONS

=over

=item int nim_eval( void *state, GGTL *g )

Returns the fitness of a gamestate from the view of the current
player. A non-ending state has a fitness of 0, a finishing state
has a fitness of 1. It is a winning state for that player, as the
previous player was forced to take the last piece.

=cut

*/

int nim_eval( void *state, GGTL *g )
{
  struct nim_state *s = state;
  (void)g;

  return s->value ? 0 : 1;
}
    

/*

=item void *nim_move( void *state, void *move, GGTL *g )

Returns the state resulting from applying C<move> to C<state>, or
NULL on failure.

=cut

*/

void *nim_move( void *state, void *move, GGTL *g )
{
  struct nim_state *s = state;
  struct nim_move *m = move;

  (void)g;
  s->player = 3 - s->player;
  s->value = s->value - m->value;

  return s;
}

/*

=item void *nim_unmove( void *state, void *move, GGTL *g )

Returns the state resulting from reversing C<move> on C<state>, or
NULL on failure.

=cut

*/

void *nim_unmove( void *state, void *move, GGTL *g )
{
  struct nim_state *s = state;
  struct nim_move *m = move;
    
  (void)g;
  s->player = 3 - s->player;
  s->value = s->value + m->value;

  return s;
}


/*

=item GGTL_MOVE *nim_get_moves( void *state, GGTL *g )

Returns a list of the available moves at the given position, or NULL if no
moves could be found.

=cut

*/

GGTL_MOVE *nim_get_moves( void *state, GGTL *g )
{
  struct nim_state *s = state;
  GGTL_MOVE *moves;
  int i;

  moves = NULL;
  for (i = 1; i < 4 && i <= s->value; i++) {
    struct nim_move *m = ggtl_uncache_move_raw(g);
    GGTL_MOVE *n = ggtl_wrap_move(g, m);
    assert(n != NULL);

    if (!m) {
      n->data = m = malloc( sizeof *m );
    }
    assert(m != NULL);

    m->value = i;
    moves = sl_push(moves, n);
  }

  return moves;
}


/*

=back

=head1 SEE ALSO

L<ggtl(3)|ggtl>, L<reversi(3)|reversi>

=head1 AUTHOR

Stig Brautaset <stig@brautaset.org>

=head1 COPYRIGHT

Copyright (C) 2005-2006 Stig Brautaset

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

=cut

*/
