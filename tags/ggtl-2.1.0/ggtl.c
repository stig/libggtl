#include "ggtl/core.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <sl/sl.h>

#define max(a, b) (a) > (b) ? (a) : (b)
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

  /* optimisation for end of search */
  int saw_end;
};

/* The various AIs */
GGTL_MOVE *ai_none(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_random(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_fixed(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_iterative(GGTL *g, GGTL_MOVE *);

/* Helper functions */
int ab(GGTL *g, int alpha, int beta, int ply);
static void ai_trace(GGTL *g, int level, char *fmt, ...);
static void state_cache_free(GGTL *g);
static void move_cache_free(GGTL *g);


#if HAVE_SYS_TIME_H && HAVE_GETTIMEOFDAY
#include <sys/time.h>
typedef struct timeval ggtl_time_t;
#else
#include <time.h>
typedef time_t ggtl_time_t;
#endif

static void setstarttime(ggtl_time_t *t)
{
#if HAVE_GETTIMEOFDAY
  (void)gettimeofday(t, NULL);
#else
  *t = time(NULL);
#endif
}

static int havetimeleft(ggtl_time_t start, long max)
{
#if HAVE_GETTIMEOFDAY
  struct timeval now, elapsed, allowed;

  gettimeofday(&now, NULL);
  timersub(&now, &start, &elapsed);

  allowed.tv_sec = (time_t)max / 1000;
  allowed.tv_usec = (suseconds_t)(max % 1000) * 1000;

  return timercmp(&allowed, &elapsed, >);
#else
  return ((time_t)max / 1000) < time(NULL) - start;
#endif
}

/*

=head1 NAME

GGTL - Generic Game-Tree search Library

=head1 SYNOPSIS

  #include <ggtl/core.h>
  
  GGTL *ggtl_new(void);
  GGTL *ggtl_init(GGTL *g, void *s);
  GGTL_VTAB *ggtl_vtab(GGTL *g);
  void ggtl_free(GGTL *g);
  
  void *ggtl_move(GGTL *g, void *m);
  void *ggtl_ai_move(GGTL *g);
  void *ggtl_undo(GGTL *g);
  
  void ggtl_set(GGTL *g, int key, int value);
  int ggtl_get(GGTL *g, int key);
  
  void *ggtl_peek_state(GGTL *g);
  void *ggtl_peek_move(GGTL *g);
  
  GGTL_MOVE *ggtl_get_moves(GGTL *g);
  int ggtl_game_over(GGTL *g);
  int ggtl_eval(GGTL *g);
  
  GGTL_STATE *ggtl_sc_new(void *state);   [ deprecated ] 
  GGTL_MOVE *ggtl_mc_new(void *move);     [ deprecated ]
  GGTL_STATE *ggtl_wrap_state(GGTL *g, void *state);
  GGTL_MOVE *ggtl_wrap_move(GGTL *g, void *move);
  
  void ggtl_cache_states(GGTL *g, GGTL_STATE *list);
  void ggtl_cache_state(GGTL *g, void *state);
  void ggtl_cache_moves(GGTL *g, GGTL_MOVE *list);
  void ggtl_cache_move(GGTL *g, void *move);
  GGTL_STATE *ggtl_uncache_state(GGTL *g);
  GGTL_MOVE *ggtl_uncache_move(GGTL *g);
  void *ggtl_uncache_state_raw(GGTL *g);
  void *ggtl_uncache_move_raw(GGTL *g);
  void ggtl_cache_free(GGTL *g);
  
=head1 DESCRIPTION

GGTL exists to make it simple to create strategic games in C. The
AI provided by GGTL can play any two-player, zero-sum game with
perfect information.

GGTL is not magic, however; it must be supplied with game-secific
callback functions in order to work. More information about these
callback functions can be found in the L<CALLBACK FUNCTIONS>
section.

The functions you have to care about when starting out using GGTL
are: C<ggtl_new()>, C<ggtl_init()> and C<ggtl_vtab()> for setting
up and initialising a game; and C<ggtl_move()>, C<ggtl_ai_move()>
and C<ggtl_undo()> for use during the course of the game. 

GGTL ships with an extension for Reversi (aka Othello). This
extension provides all the callbacks necessary in order to play
Reversi (Othello). See L<ggtl-reversi(3)|ggtl-reversi> for its
manpage.

=head1 DATA TYPES

GGTL stores its internal state in a structure of type C<GGTL>.
You are not allowed to poke around inside this structure, so it
is hidden behind an opaque pointer.

Lists of states and moves are used extensively in GGTL, so states
are often wrapped in C<GGTL_STATE> containers and moves in
C<GGTL_MOVE> containers. These structures have a C<next>
member pointing to the next node in the chain, and a C<data>
member pointing to the state or move. See C<ggtl_wrap_state()> &
C<ggtl_wrap_move()>.

The L<sl(3)|sl> library is used to manage lists internally. See
its documentation for details if you are developing your own
C<get_moves()> callback function, or want to traverse the list
returned from a call to C<ggtl_get_moves()>. These are the only
cases where users of GGTL are directly exposed to linked lists. 

GGTL stores pointers to the callback functions it uses in a
C<GGTL_VTAB>. This is part of what makes GGTL flexible and
generic; as long as suitable game-dependent functions are
assigned to the entries in this structure GGTL's AI can play any
2-player zero-sum game with perfect information. See also
C<ggtl_vtab()> and L<CALLBACK FUNCTIONS> below.


=head1 FUNCTIONS

Almost all of GGTL's functions take a pointer to a C<GGTL>
as their first argument. For brevity, the type has been omitted
from the prototypes below, when they appear as the first
argument. Thus, instead of C<GGTL *g>, C<*g> is simply
used instead.

=head2 Setup and cleanup

If you are using one of GGTL's extensions, you will only need the
first two of these.

=over

=item GGTL *ggtl_new()

Returns a new ggtl structure, or NULL on failure.

=cut

*/

GGTL *ggtl_new(void)
{
  GGTL *g;

  g = malloc( sizeof *g );
  if (g) {
    g->vtab = malloc( sizeof(GGTL_VTAB) );
    if (g->vtab) {
      g->vtab->eval = NULL;
      g->vtab->move = NULL;
      g->vtab->unmove = NULL;
      g->vtab->get_moves = NULL;
      g->vtab->game_over = NULL;
      g->vtab->clone_state = NULL;

      g->vtab->free_state = &free;
      g->vtab->free_move = &free;
    }
    else {
      ggtl_free( g );
      return NULL;
    }
    g->states = g->state_cache = g->sc_cache = NULL;
    g->moves = g->move_cache = g->mc_cache = NULL;
    ggtl_set(g, CACHE, STATES | MOVES); /* cache both */

    ggtl_set(g, TYPE, ITERATIVE);   /* the fixed-depth AI */
    ggtl_set(g, PLY, 3);            /* ply 3 */
    ggtl_set(g, MSEC, 200);         /* 200 ms */
    ggtl_set(g, TRACE, 0);          /* no trace output */
  }
  
  return g;
}

/*

=item void ggtl_free( *g )

Free the memory held in a ggtl struct.

=cut

*/

void ggtl_free( GGTL *g )
{
  ggtl_cache_states(g, g->states);
  ggtl_cache_moves(g, g->moves);
  g->states = NULL;
  g->moves = NULL;

  ggtl_cache_free(g);
  free(g->vtab);
  free(g);
}


/*

=item GGTL *ggtl_init( *g, void *state )

Set the starting position to C<state>.
Returns C<g> on success, NULL on failure.

=cut

*/

GGTL *ggtl_init( GGTL *g, void *s )
{
  g->states = ggtl_wrap_state(g, s);
  return g->states ? g : NULL;
}

/*

=item GGTL_VTAB *ggtl_vtab( *g )

Returns a pointer to C<g>'s vtable. Pointers to all the callback
functions used by GGTL are stored in this structure.  See
L<CALLBACK FUNCTIONS> below for details on each entry.

=cut

*/

GGTL_VTAB *ggtl_vtab( GGTL *g )
{
  return g->vtab;
}

/*

=back

=head2 Moving and undo

=over

=item void *ggtl_move( *g, void *move )

Apply the given move to the current position. A pointer to
the resulting new state is returned on success, NULL on
failure. 

Upon success, the move is stored in a history list. If
the C<unmove()> callback is not provided, the previous state is
also stored in order to provide undo.

=cut

*/

void *ggtl_move( GGTL *g, void *m )
{
  GGTL_STATE *ns = NULL;
  GGTL_MOVE *nm = ggtl_wrap_move(g, m);

  if (nm) {
    ns = ggtl_move_internal(g, nm);
    if (!ns) {
      nm->data = NULL;  /* we don't want to free move */
      g->mc_cache = sl_push(g->mc_cache, nm);
    }
  }
    
  return ns ? ggtl_peek_state(g) : NULL;
}

/*

=begin internal

=item GGTL_STATE *ggtl_move_internal( *g, GGTL_MOVE *move )

Similar to L<ggtl_move()> but takes a move wrapped in a
C<GGTL_MOVE> container and returns a state wrapped in a C<GGTL_STATE>
container.

=end internal

=cut

*/

GGTL_STATE *ggtl_move_internal( GGTL *g, GGTL_MOVE *m )
{
  GGTL_VTAB *v = ggtl_vtab(g);
  void *s = ggtl_peek_state(g);
  
  if (v->unmove) {
    if (v->move(s, m->data, g)) {
      g->moves = sl_push(g->moves, m);
      return g->states;
    }
    return NULL;
  }

  if (v->clone_state) { 
    s = v->clone_state(s, g);
  }
  assert(s != NULL);
  
  s = v->move(s, m->data, g);
  if (s) {
    g->states = sl_push(g->states, ggtl_wrap_state(g, s));
    g->moves = sl_push(g->moves, m);
  }

  return s ? g->states : NULL;
}

/*

=item void *ggtl_ai_move( *g )

Make the GGTL AI perform a move. Returns a pointer to the new
game state, or NULL on error.

The same internal mechanisms are used as for the C<ggtl_move()>,
so the AI's moves can also be undone.

=cut

*/

void *ggtl_ai_move(GGTL *g)
{
  GGTL_MOVE *moves, *move;
  GGTL_STATE *state;

  assert(g != NULL);
  g->opts[VISITED] = 0;

  move = NULL;
  moves = ggtl_get_moves(g);
  if (1 < sl_count(moves)) {
    switch (ggtl_get(g, TYPE)) {
      case NONE:
        move = ai_none(g, moves);
        break;
      case RANDOM:
        move = ai_random(g, moves);
        break;
      case FIXED:
        move = ai_fixed(g, moves);
        break;
      case ITERATIVE:
        move = ai_iterative(g, moves);
        break;
      default:
        fputs("Illegal AI type. How the heck did you manage that?\n", stderr);
        exit(EXIT_FAILURE);
    }
  }
  else if (moves) {
    move = moves;
    moves = NULL;
    ai_trace(g, 1, "only one move possible (skipping search)");
  }

  state = NULL;
  if (move) {
    state = ggtl_move_internal(g, move);
    if (!state) {
      ggtl_cache_moves(g, move);
    }
  }

  return state ? state->data : NULL;
}

/*

=item void *ggtl_undo( *g )

Reverts the internal game state to the previous.  Returns a
pointer to the new current state, or NULL on error (e.g. if there
was no move to undo).

=cut

*/

void *ggtl_undo( GGTL *g )
{
  GGTL_MOVE *n;
  void *s = NULL;

  n = ggtl_undo_internal(g);
  if (n) {
    ggtl_cache_moves(g, n);
    s = ggtl_peek_state(g);
  }

  return s;
}

/*

=begin internal

=item GGTL_MOVE *ggtl_undo_internal( *g )

Reverts the internal game state to the previous.  Returns the
move that was undone (wrapped in a C<GGTL_MOVE> container), or NULL
on error (e.g. if there was no move to undo).

=end internal

=cut

*/

GGTL_MOVE *ggtl_undo_internal( GGTL *g )
{
  GGTL_MOVE *nm;

  nm = sl_pop(&g->moves);
  if (nm) {
    if (g->vtab->unmove) {
      /* TODO: check return value */
      g->vtab->unmove(ggtl_peek_state(g), nm->data, g);
    }
    else {
      GGTL_STATE *ns = sl_pop(&g->states);
      ggtl_cache_states(g, ns);
    }
  }

  return nm;
}


/*

=back

=head2 Run-time options 

=over

=item void ggtl_set( *g, int key, int value )

=item int ggtl_get( *g, int key )

Sets and gets the value of the given key. The following keys are
available:

=over

=cut

*/

void ggtl_set(GGTL *g, int key, int value)
{
  assert(key >= 0);
  assert(key < SET_KEYS);
  g->opts[key] = value;
}

int ggtl_get(GGTL *g, int key)
{
  assert(key >= 0);
  assert(key < GET_KEYS);
  return g->opts[key];
}

/*

=item TYPE - the type of AI to use

Takes the following values:

=over

=item NONE

Always picks the first move returned by the get_moves()
callback; may be useful for testing.

=cut

*/

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

Picks one of the available moves at random.
It is the responsibility of the user to call srand() to make sure
the random generator is seeded differently for each run.

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

int ggtl_mc_cmp(void *anode, void *bnode)
{
  GGTL_MOVE *a = anode;
  GGTL_MOVE *b = bnode;
  return b->fitness - a->fitness;
}

int ab(GGTL *g, int alpha, int beta, int plytogo)
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
    alpha = max(alpha, sc);
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
C<ggtl_set(g, MSEC, value)>.

=cut

*/

GGTL_MOVE *ai_iterative(GGTL *g, GGTL_MOVE *moves)
{
  GGTL_MOVE *best;
  int ply, saved_ply;
  ggtl_time_t start;

  assert(1 < sl_count(moves));
  saved_ply = ggtl_get(g, PLY);
  setstarttime(&start);

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

    if (!havetimeleft(start, ggtl_get(g, MSEC) / 2)) {
      break;
    }
    
    moves = ggtl_get_moves(g);
  }
  ggtl_set(g, PLY, saved_ply);

  return best;
}


/*

=back

=item MSEC

The maximum time (in milliseconds) the iterative AI is allowed to
use for a search.

=item PLY

The depth to search to for the fixed-depth AI.

=item TRACE

The level of trace information to print during search. The trace
information will be printed to standard output. Zero (the
default) turns off tracing; larger numbers give more detailed
trace output.

=item CACHE

Decide what to cache. If 0, GGTL will not cache anything. Other
valid values are combinations of the following, ORed together:

  STATES  - cache states
  MOVES   - cache moves

Example: use C<ggtl_set(g, CACHE, STATES | MOVES)> to cache both
moves and states (this is the default).

=item VISITED - ggtl_get() only

Returns the number of states visited by the last AI search, or -1
if no information is available. If no search has taken place, the
value is undefined.

=item PLY_REACHED - ggtl_get() only

Returns the effective depth of the last iterative AI search. The
value is undefined if no such search has taken place.

=back

=cut

*/


static void ai_trace(GGTL *g, int depth, char *fmt, ...)
{
  va_list ap;
  if (ggtl_get(g, TRACE) >= depth) {
    fprintf(stdout, "%*s", depth - 1, "");
    va_start(ap, fmt);
    (void)vfprintf(stdout, fmt, ap);
    va_end(ap);
    fputc('\n', stdout);
  }
}
  

/*

=back

=head2 Misc

=over

=item void *ggtl_peek_state( *g )

Returns a pointer to the current state, or NULL on error.

=cut

*/

void *ggtl_peek_state( GGTL *g )
{
  return g->states ? g->states->data : NULL;
}

/*

=item void *ggtl_peek_move( *g )

Returns a pointer to the last move performed, or NULL on error.

=cut

*/

void *ggtl_peek_move( GGTL *g )
{
  return g->moves ? g->moves->data : NULL;
}


/*

=item GGTL_MOVE *ggtl_get_moves( *g )

Returns a list of the available moves at the current state, or
NULL if there are no available moves (i.e, if the game is over).

=cut

*/

GGTL_MOVE *ggtl_get_moves(GGTL *g)
{
  GGTL_MOVE *moves;
  GGTL_VTAB *v;

  v = ggtl_vtab(g);
  if (v->game_over && v->game_over(ggtl_peek_state(g), g)) {
    moves = NULL;
  }
  else {
    moves = v->get_moves(ggtl_peek_state(g), g);
  }

  return moves;
}

/*

=item int ggtl_game_over( *g )

Returns true if the current game state is a final state, or false
if the game is still on.

=cut

*/

int ggtl_game_over(GGTL *g)
{
  GGTL_MOVE *moves;
  int retval = 0;

  moves = ggtl_get_moves(g);
  retval = moves ? 0 : 1;

  ggtl_cache_moves(g, moves);

  return retval;
}
    
/*

=item int ggtl_eval( *g )

Returns the value of the current game state.

=cut

*/

int ggtl_eval(GGTL *g)
{
  void *state = ggtl_peek_state(g);
  return ggtl_vtab(g)->eval(state, g);
}

/*

=item GGTL_STATE *ggtl_sc_new(void *state)  [deprecated]

=item GGTL_STATE *ggtl_wrap_state(*g, void *state)

Returns the state, wrapped in a C<GGTL_STATE> node,
or NULL if an error occurs.

=cut

*/

GGTL_STATE *ggtl_sc_new(void *data)
{
  GGTL_STATE *n;

  n = malloc(sizeof *n);
  if (n) {
    n->next = NULL;
    n->data = data;
  }
  
  return n;
}

GGTL_STATE *ggtl_wrap_state(GGTL *g, void *data)
{
  GGTL_STATE *n = sl_pop(&g->sc_cache);

  if (!n) {
    n = ggtl_sc_new(data);
  }
  else {
    n->data = data;
  }

  return n;
}

/*

=item GGTL_MOVE *ggtl_mc_new(void *move)  [deprecated]

=item GGTL_MOVE *ggtl_wrap_move(*g, void *move)

Returns the move wrapped in a C<GGTL_MOVE> container, 
or NULL if an error occurs.

=cut

*/

GGTL_MOVE *ggtl_mc_new(void *data)
{
  GGTL_MOVE *n;

  n = malloc(sizeof *n);
  if (n) {
    n->next = NULL;
    n->fitness = 0;
    n->data = data;
  }
  
  return n;
}

GGTL_MOVE *ggtl_wrap_move(GGTL *g, void *data)
{
  GGTL_MOVE *n = sl_pop(&g->mc_cache);

  if (!n) {
    n = ggtl_mc_new(data);
  }
  else {
    n->data = data;
  }

  return n;
}


/*

=back

=head2 Interaction with GGTL's cache of states and moves

GGTL agressively caches states and moves internally to avoid
calling L<malloc(3)|malloc> and L<free(3)|free> more than
necessary, and it exposes this cache to its user. This becomes
particularly useful if you are implementing your own
C<get_moves()> or C<move()> callback function.

See also L<CACHE> under L<Run-time options> for configuring what
to cache.

=over

=item void ggtl_cache_states( *g, GGTL_STATE *states )

=item void ggtl_cache_state( *g, void *state )

=item void ggtl_cache_moves( *g, GGTL_MOVE *moves )

=item void ggtl_cache_move( *g, void *move )

Put a list of states or moves onto their respective cache.
Remember that a list can have only one element.

Note: these functions change behaviour depending on the cache
flag (see C<ggtl_cache()>). If it is turned off for the given
type, the memory will simply be freed instead.

=cut

*/

void ggtl_cache_states(GGTL *g, GGTL_STATE *list)
{
  GGTL_STATE *item;
  while ((item = sl_pop(&list))) {
    g->state_cache = sl_push(g->state_cache, item);
  }
  if (!(ggtl_get(g, CACHE) & STATES)) {
    state_cache_free(g);
  }
}

void ggtl_cache_state(GGTL *g, void *s)
{
  ggtl_cache_states(g, ggtl_wrap_state(g, s));
}

void ggtl_cache_moves(GGTL *g, GGTL_MOVE *list)
{
  GGTL_MOVE *item;
  while ((item = sl_pop(&list))) {
    g->move_cache = sl_push(g->move_cache, item);
  }
  if (!(ggtl_get(g, CACHE) & MOVES)) {
    move_cache_free(g);
  }
}

void ggtl_cache_move(GGTL *g, void *s)
{
  ggtl_cache_moves(g, ggtl_wrap_move(g, s));
}

/*

=item GGTL_STATE *ggtl_uncache_state( *g )

=item GGTL_MOVE *ggtl_uncache_move_raw( *g )

=item void *ggtl_uncache_state_raw( *g )

=item void *ggtl_uncache_move_raw( *g )

Retrieve a state or a move from its respective cache. The
C<ggtl_uncache_*_raw> versions drops the wrapper node and returns
pointers to the actual state/move.

=cut

*/

GGTL_STATE *ggtl_uncache_state(GGTL *g)
{
  return sl_pop(&g->state_cache);
}

GGTL_MOVE *ggtl_uncache_move(GGTL *g)
{
  return sl_pop(&g->move_cache);
}

void *ggtl_uncache_state_raw(GGTL *g)
{
  void *state = NULL;
  GGTL_STATE *node = ggtl_uncache_state(g);

  if (node) {
    state = node->data;
    node->data = NULL;
    g->sc_cache = sl_push(g->sc_cache, node);
  }

  return state;
}

void *ggtl_uncache_move_raw(GGTL *g)
{
  void *move = NULL;
  GGTL_MOVE *node = ggtl_uncache_move(g);

  if (node) {
    move = node->data;
    node->data = NULL;
    g->mc_cache = sl_push(g->mc_cache, node);
  }

  return move;
}

/*

=item void ggtl_cache_free( *g )

Free all memory used by the cache. The use of this function
should normally not be necessary.

=cut

*/

void ggtl_cache_free(GGTL *g)
{
  state_cache_free(g);
  move_cache_free(g);
}

static void state_cache_free(GGTL *g)
{
  void *n;
  while ((n = ggtl_uncache_state_raw(g))) {
    g->vtab->free_state(n);
  }
  sl_free(g->sc_cache, NULL);
}

static void move_cache_free(GGTL *g)
{
  void *n;
  while ((n = ggtl_uncache_move_raw(g))) {
    g->vtab->free_move(n);
  }
  sl_free(g->mc_cache, NULL);
}

/*

=back

=head1 CALLBACK FUNCTIONS

GGTL is a generic implementation of the Alpha-Beta game-tree
search algorithm. It must be provided with game-specific callback
functions in order to perform operations such as executing a move
and evaluating the fitness of a game state.

Setting the callback functions can be done thusly:

  ggtl_vtab(g)->eval      = &my_eval;
  ggtl_vtab(g)->get_moves = &my_get_moves;

The full list of callback functions is:

=over

=item void *move(void *state, void *mv, GGTL *g)

Apply C<mv> to C<state>, producing the successor state. Return
the pointer to the new state, or NULL on failure.  If the game
allows pass moves, this function must understand them. It is has
the responsibility of updating the indication on which player's
turn it is (a property of the game state).

C<g> is passed in so that you can benefit from GGTL's caches; 
see e.g. C<ggtl_uncache_state()> and C<ggtl_uncache_state_raw()>.

B<NOTE:> The prototype of this callback changed from v2.1.0. GGTL
now expects the passed-in state to be modified. You should
additionally implement I<either> the C<unmove()> or
C<clone_state()> callbacks (for backwards compatibility you may
omit both of these and return a new wrapped state as before--this
behaviour is deprecated, however, and will likely dissapear in a
later version).



=item void *unmove(void *state, void *mv, GGTL *g)

Optional callback to revert the passed-in move from the passed-in
state (without taking a copy). By providing this callback GGTL
will not keep a copy of each state for its undo-functionality.

The callback should return a valid pointer on success and NULL on
failure.



=item void *clone_state(void *state, GGTL *g)

Optional callback to clone C<state>. This is used to copy the
current state before passing it to the C<move()> callback.  It
should return a pointer to the cloned state or NULL on failure.



=item int eval(void *state, GGTL *g)

Should be implemented to calculate and return the fitness
(MiniMax) value of C<state>. This value must be in the range
C<GGTL_FITNESS_MIN> to C<GGTL_FITNESS_MAX>.



=item GGTL_MOVE *get_moves(void *state, GGTL *g)

Should return a list of all the moves available to the current
player at the given state, or NULL if the game is over. Hence,
if passing is allowed, a special 'pass move' must be returned.

The C<ggtl_wrap_move()> function and the L<sl> manpage may come
in handy.



=item int game_over(void *state, GGTL *g)

Optional callback to check for an end-state of the game.

If specified, must return non-zero if C<state> is a final state
in the current game. For example, some games automatically end
after a certain number of moves; this can be simply represented
by this function.

Not all games need this callback; for many games it is enough to
just let C<get_moves()> return NULL to signal that the game is
over. 



=item void free_state(void *state)

=item void free_move(void *move)

Optional callbacks for freeing up the memory held by a state and
a move, respectively. This is rarely done, as GGTL caches moves
and states agressively by default. You may chose to do so,
however.

You only have to worry about these if your states or moves 
requires manual cleaning up. The standard C function
C<free(3)|free> is used if you don't specify anything.

=back



=head1 GGTL (AB)USE

GGTL can be used even if your game is not one of those that
can be solved by its AI. For example, it can be convenient to use
GGTL for keeping track of history and providing undo.

=head1 BUGS / TODO

Investigate feasibility of supporting A* search as well.

=head1 SEE ALSO

L<ggtl-reversi(3)|ggtl-reversi> E<amp> L<ggtl-nim(3)|ggtl-reversi>.

I<Generic Game Framework in Eiffel> (G2F3) is a related project,
but--as the name implies--programmed in Eiffel:

  http://freshmeat.net/projects/g2f3/

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
