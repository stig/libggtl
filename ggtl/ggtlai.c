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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sl/sl.h>

#include "core.h"
#include "private.h"

static int ab(GGTL *g, int alpha, int beta, int ply);

#if HAVE_GETTIMEOFDAY

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

static double setstarttime() {
  struct timeval t;
  (void)gettimeofday(&t, NULL);
  return t.tv_sec + (t.tv_usec / 1000000.0);
}

#else

#include <time.h>
static double setstarttime() {
  return clock() / (double)CLOCKS_PER_SEC;
}

#endif

static int havetimeleft(double start, double max)
{
  double elapsed = setstarttime() - start;
  return elapsed < max;
}

#if 0 /* C comments don't nest */

=head1 NAME

ggtlai - the various AIs supported by GGTL

=head1 SYNOPSIS

  #include <ggtl/core.h>
  
  /* set the current AI type */
  ggtl_set(g, TYPE, X);

  /* get the current AI */
  ggtl_get(g, TYPE);

  /* force a move */
  ggtl_move(g, m);

  /* make the current AI perform a move */
  ggtl_ai_move(g);

=head1 DESCRIPTION

GGTL supports a few different modes for the AI player.
The various AIs can be chosen by calling C<< ggtl_set(g, TYPE,
<type>) >>. The accepted C<type>s are:

=over

=item NONE

Not really an AI. Always picks the first move returned by the
get_moves() callback. Can be useful for testing.

=cut

#endif

GGTL_MOVE *ai_none(GGTL *g, GGTL_MOVE *moves)
{
  GGTL_MOVE *move;

  assert(1 < sl_count(moves));

  move = sl_pop(&moves);
  ggtl_cache_moves(g, moves);

  return move;
}

/*

=item RANDOM

Not really an AI either. Picks one of the available moves at
random. It is the responsibility of the user to call srand() to
make sure the random generator is seeded differently for each
run.

=cut

*/

GGTL_MOVE *ai_random(GGTL *g, GGTL_MOVE *moves)
{
  GGTL_MOVE *move;
  int idx;
  
  assert(1 < sl_count(moves));

  idx = rand() % sl_count(moves);
  while (idx--) {
    ggtl_cache_moves(g, sl_pop(&moves));
  }
  move = sl_pop(&moves);
  ggtl_cache_moves(g, moves);

  assert(move != NULL);

  return move;
}

/*

=item FIXED 

Performs a fixed-depth Alpha-Beta search to find the best move at
a given ply (depth in the game tree). The depth to be searched
can be set with C<ggtl_set(g, PLY, depth)>.

=cut

*/

int fitness_cmp(void *anode, void *bnode)
{
  GGTL_MOVE *a = anode;
  GGTL_MOVE *b = bnode;
  return b->fitness - a->fitness;
}

static int ab(GGTL *g, int alpha, int beta, int plytogo)
{
  GGTL_MOVE *moves, *m;
  int tracelevel = ggtl_get(g, PLY) - plytogo + 2;

  g->opts[VISITED]++;
  
  moves = ggtl_get_moves(g);
  if (!moves || plytogo <= 0) {
    int fitness;
    if (!moves) { g->saw_end = 1; }
    else { ggtl_cache_moves(g, moves); }
    fitness = ggtl_vtab(g)->eval(ggtl_peek_state(g), g);
    ai_trace(g, tracelevel, "%s: %d", 
      moves ? "ply limit" : "leaf state", fitness);
    return fitness;
  }
  
  while (alpha < beta && (m = sl_pop(&moves))) {
    int sc;
    void *state;

    if (!ggtl_move_internal(g, m)) {
      ggtl_cache_moves(g, m);
      alpha = GGTL_ERR;
      break;
    }

    sc = -ab(g, -beta, -alpha, plytogo - 1);
    if (sc > alpha) {
      alpha = sc; 
    }
    state = ggtl_undo(g);
    assert(state != NULL);
  }

  {
    char skipped[50] = {0};
    if (moves) {
      int count = sl_count(moves);
      char *plural = count == 1 ? "" : "es";
      sprintf(skipped, " (%d branch%s skipped)", count, plural);
    }
    ai_trace(g, tracelevel, "a/b: %d/%d%s", alpha, beta, skipped);
  }

  /* if we broke out of the loop early, 
     cache the rest of the moves */
  ggtl_cache_moves(g, moves);

  return alpha;
}

GGTL_MOVE *ai_fixed(GGTL *g, GGTL_MOVE *moves)
{
  GGTL_STATE *state;
  GGTL_MOVE *m, *best;
  int alpha, beta;

  assert(1 < sl_count(moves));

  alpha = GGTL_FITNESS_MIN-1;  /* loss should be better than this */
  beta = GGTL_FITNESS_MAX;

  state = NULL;
  best = NULL;
  while ((m = sl_pop(&moves))) {
    if (ggtl_move_internal(g, m)) {
      int sc = -ab(g, -beta, -alpha, ggtl_get(g, PLY) - 1);
      ai_trace(g, 2, "a/b: %d/%d (visited: %d)", sc, beta,
        ggtl_get(g, VISITED));
      m = ggtl_undo_internal(g); 

      /* It is important that the _first_ move to be found with
       * the given alpha-beta value is picked, as latter ones
       * might be worse (since they might be victims of cutoffs).  */
      if (sc > alpha) {
        GGTL_MOVE *tmp = best;
        best = m;
        m = tmp;
        alpha = sc;
      }
      ggtl_cache_moves(g, m);
    }
    else {
      ggtl_cache_moves(g, m);
      ggtl_cache_moves(g, moves);
      ggtl_cache_moves(g, best);
      return NULL;
    }
      
    assert(alpha != GGTL_ERR);
  }
  assert(best != NULL);
  m = best;

  ggtl_cache_moves(g, moves);
  
  ai_trace(g, 1, 
    "best branch: %d (ply %d search; %d states visited)",
    alpha, ggtl_get(g, PLY), ggtl_get(g, VISITED));

  return m;
}


/*

=item ITERATIVE

This AI performs an iterative deepening Alpha-Beta search to find
the best possible move at the greatest depth that can be searched
in a given time. The time allowed for a search can be set with
C<ggtl_set()>; use C<ggtl_set(g, TIME, 0.350)> to set the allowed
time to 350 milliseconds.

=cut

*/

GGTL_MOVE *ai_iterative(GGTL *g, GGTL_MOVE *moves)
{
  GGTL_MOVE *best;
  int ply, saved_ply;
  double start;

  assert(1 < sl_count(moves));
  saved_ply = ggtl_get(g, PLY);
  start = setstarttime();

  best = NULL;
  for (ply = 1;; ply++) { 
    GGTL_MOVE *m;

    ggtl_set(g, PLY, ply);
    m = ai_fixed(g, moves);
    if (m) {
      ggtl_cache_moves(g, best);
      best = m;
      g->opts[PLY_REACHED] = ply;
    }

    if (!havetimeleft(start, g->time_to_search / 2.0)) {
      break;
    }
    
    moves = ggtl_get_moves(g);
  }
  ggtl_set(g, PLY, saved_ply);

  return best;
}

/*

=back

=head1 SEE ALSO

L<ggtl(3)|ggtl>

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

