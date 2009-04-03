#include "ggtl/core.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <sl/sl.h>

#define max(a, b) (a) > (b) ? (a) : (b)
#define GGTL_ERR (FITNESS_MAX+1)

struct ggtl {
  GGTL_VTAB *vtab;

  GGTL_STATE *state_cache;
  GGTL_MOVE *move_cache;
  GGTL_STATE *states;
  GGTL_MOVE *moves;

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
int ggtl_mc_cmp(void *anode, void *bnode);
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
  
  GGTL_STATE *ggtl_sc_new(void *data);
  GGTL_MOVE *ggtl_mc_new(void *data);
  
  void ggtl_cache_states(GGTL *g, GGTL_STATE *list);
  void ggtl_cache_moves(GGTL *g, GGTL_MOVE *list);
  GGTL_STATE *ggtl_uncache_state(GGTL *g);
  GGTL_MOVE *ggtl_uncache_move(GGTL *g);
  void ggtl_cache_free(GGTL *g);
  
=head1 DESCRIPTION

GGTL exists to make it simple to create strategic games in C. The
AI provided by GGTL can play any two-player, zero-sum game with
perfect information.

GGTL is not magic, however; it must be supplied with game-secific
callback functions in order to work. More information about these
callback functions can be found in the L<CALLBACK FUNCTIONS>
section. A Reversi extension to GGTL is available; this extension
provides all the callbacks necessary in order to play Reversi
(Othello). See L<ggtl-reversi(3)|ggtl-reversi> for its manpage.

Assuming you are not using one of GGTL's extensions, the most
important functions are: C<ggtl_new()>, C<ggtl_init()> and
C<ggtl_vtab()> for setting up and initialising a game; and
C<ggtl_move()>, C<ggtl_ai_move()> and C<ggtl_undo()> for use
during the course of the game. If you use the Reversi extension,
for example, you would instead use its C<reversi_init()> function
in place of C<ggtl_init()> and C<ggtl_vtab()>. This sets up all
the necessary callbacks for you.

GGTL stores its internal state in a structure of type C<GGTL>.
You are not allowed to poke around inside this structure, so it
is hidden behind an opaque pointer.

Lists of states and moves are used extensively in GGTL, so states
are often wrapped in C<GGTL_STATE> containers and moves in
C<GGTL_MOVE> containers. These structures have a C<next>
member pointing to the next node in the chain, and a C<data>
member pointing to the state or move. See C<ggtl_sc_new()>
and C<ggtl_mc_new()>. 

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
      g->vtab->get_moves = NULL;
      g->vtab->game_over = NULL;

      g->vtab->free_state = &free;
      g->vtab->free_move = &free;
    }
    else {
      ggtl_free( g );
      return NULL;
    }
    g->states = g->state_cache = NULL;
    g->moves = g->move_cache = NULL;
    ggtl_set(g, CACHE, STATES | MOVES); /* cache both */

    ggtl_set(g, TYPE, ITERATIVE);   /* the fixed-depth AI */
    ggtl_set(g, PLY, 3);            /* ply 3 */
    ggtl_set(g, MSEC, 200);         /* 200 ms */
    ggtl_set(g, TRACE, 0);          /* no trace output */
  }
  
  return g;
}

/*

=item ggtl_free( *g )

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
  g->states = ggtl_sc_new(s);
  return g->states ? g : NULL;
}

/*

=item GGTL_VTAB *ggtl_vtab( *g )

Returns a pointer to C<g>'s vtable. Pointers to all the callback
functions used by GGTL are stored in this structure. It looks
like this (see L<CALLBACK FUNCTIONS> below for details on each
entry):

  typedef struct ggtl_vtab {
    int (*eval)(void *, GGTL *);
    int (*game_over)(void *, GGTL *);
    GGTL_MOVE *(*get_moves)(void *, GGTL *);
    GGTL_STATE *(*move)(void *, void *, GGTL *);
    void (*free_state)(void *);
    void (*free_move)(void *);
  } GGTL_VTAB;

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

=cut

*/

void *ggtl_move( GGTL *g, void *m )
{
  GGTL_STATE *ns;
  GGTL_MOVE *nm;

  ns = NULL;
  nm = ggtl_mc_new(m);
  if (nm) {
    ns = ggtl_move_internal(g, nm);
    if (!ns) {
      nm->data = NULL;  /* we don't want to free move */
      sl_free(nm, NULL);
    }
  }
    
  return ns ? ggtl_peek_state(g) : NULL;
}

/*

=begin internal

=item GGTL_STATE *ggtl_move_internal( *g, GGTL_MOVE *move )

Similar to L<ggtl_move()> but takes a move wrapped in a
C<ggtl_mc> container and returns a state wrapped in a C<ggtl_sc>
container.

=end internal

=cut

*/

GGTL_STATE *ggtl_move_internal( GGTL *g, GGTL_MOVE *m )
{
  GGTL_VTAB *v = ggtl_vtab(g);
  GGTL_STATE *state;

  state = v->move(ggtl_peek_state(g), m->data, g);
  if (state) {
    g->states = sl_push(g->states, state);
    g->moves = sl_push(g->moves, m);
  }

  return state;
}

/*

=item void *ggtl_ai_move( *g )

Make the GGTL AI perform a move. Returns a pointer to the new
game state, or NULL on error.

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
        fputs("Illegal AI type\n", stderr);
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
move that was undone (wrapped in a ggtl_mc container), or NULL
on error (e.g. if there was no move to undo).

=end internal

=cut

*/

GGTL_MOVE *ggtl_undo_internal( GGTL *g )
{
  GGTL_MOVE *nm;

  nm = sl_pop(&g->moves);
  if (nm) {
    GGTL_STATE *ns = sl_pop(&g->states);
    ggtl_cache_states(g, ns);
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

  alpha = FITNESS_MIN-1;  /* loss should be better than this */
  beta = FITNESS_MAX;

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

=item GGTL_STATE *ggtl_sc_new(void *state)

Returns the state, wrapped in a C<ggtl_sc> node,
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
/*

=item GGTL_MOVE *ggtl_mc_new(void *move)

Returns the move wrapped in a C<ggtl_mc> node, 
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

=item void ggtl_cache_state( *g, GGTL_STATE *states )

=item void ggtl_cache_move( *g, GGTL_MOVE *moves )

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

/*

=item GGTL_STATE *ggtl_uncache_state( *g )

=item GGTL_MOVE *ggtl_uncache_move( *g )

Retrieve a state or a move from its respective cache.

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
  GGTL_STATE *n;
  while ((n = ggtl_uncache_state(g))) {
    g->vtab->free_state(n->data);
    free(n);
  }
}

static void move_cache_free(GGTL *g)
{
  GGTL_MOVE *n;
  while ((n = ggtl_uncache_move(g))) {
    g->vtab->free_move(n->data);
    free(n);
  }
}

/*

=back

=head1 CALLBACK FUNCTIONS

GGTL, as mentioned, requires game-specific callback functions to
be able to work with any game.

=over


=item int eval(void *state, GGTL *g)

Responsible for evaluating C<state> and return its estimated
value for the current player.

The returned value must be in the range C<FITNESS_MIN> to
C<FITNESS_MAX>, defined in F<ggtl/core.h>.

=item GGTL_MOVE *get_moves(void *state, GGTL *g)

Should return a list of all the moves available to the current
player at the given state.

If NULL is returned, GGTL assumes the game is over. Hence, if
simply letting the turn pass to the next player is allowed, a
special pass move must be returned.

=item GGTL_STATE *move(void *state, void *mv, GGTL *g)

This function should return a state wrapped in a C<struct
ggtl_sc> container node. This state should be the result of
applying the move C<mv> to C<state> (don't modify C<state>
though; take a copy).  

If the game allows pass moves, this function must understand
them. It is has the responsibility of updating the indication on
which player's turn it is (a property of the game state).

C<g> is passed in so that you can benefit from GGTL's cache of
states; see C<ggtl_uncache_state()>.

=item int game_over(void *state, GGTL *g)

If specified, must return non-zero if C<state> is a final state
in the current game. For example, some games automatically end
after a certain number of moves; this can be simply represented
by this function.

Not all games need this function; for many games it is simpler to
just let C<get_moves()> return NULL to signal that the game is
over. 

=item void free_state(void *state)

=item void free_move(void *)

These are responsible for freeing up the memory held by a state
and a move, respectively. It is entirely possible that you do not
need to worry about these; L<free(3)|free> is the used as the default.

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
