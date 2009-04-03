#include <tap.h>
#include <stdlib.h>
#include <stddef.h>
#include <sl/sl.h>
#include <ggtl/nim.h>

int main(void)
{
  GGTL *g;
  struct nim_state *state, *s;
  struct nim_move *m, *m2;

  plan_tests(45);

  state = nim_state_new(1, 5);

  ok1( g = ggtl_new() );
  ok1( nim_init(g, state) );

  { /* limit scope of v */
    GGTL_VTAB *v;
    ok( v = ggtl_vtab(g), "got vtable" );
    ok( v->move, "vtable has move()" );
    ok( v->eval, "vtable has eval()" );
    ok( v->get_moves, "vtable has get_moves()" );
  }

  ok( s = ggtl_peek_state(g), "can peek at current state" );
  ok1( state == s );
  ok( NULL == ggtl_peek_move(g), "no move on stack yet" );

  { /* limit scope of n */
    GGTL_STATE *sc;
    GGTL_MOVE *mc;
    ok1( mc = ggtl_mc_new(nim_move_new(2)) );
    ok( sc = ggtl_move_internal(g, mc), "got state node back" );
    ok( s = sc->data, "got state" );
  }

  ok( s != state, "state is not same as before" );
  ok( s->player == 2, "player changed" );
  ok( s->value == 3, "move applied" );
  ok( m = ggtl_peek_move(g), "can peek at move now" );

  m2 = nim_move_new(1);
  ok( s = ggtl_move(g, m2), "got state back" );
  ok( s->player == 1, "player changed" );
  ok( s->value == 2, "move applied" );

  { /* limit scope of moves & movenode */
    GGTL_MOVE *moves, *movenode;
    int i;
    moves = ggtl_get_moves(g);
    ok( 2 == sl_count(moves), "only two possible moves now" );

    i = 2;
    while ((movenode = sl_pop(&moves))) {
      struct nim_move *move = movenode->data;
      ok( move->value == i, "move->value == %d", i );
      i--;
      free(move);
      free(movenode);
    }
  }

  { /* limit scope of n */
    GGTL_MOVE *n;

    ok( n = ggtl_undo_internal(g), "undo: got move node" );
    ok( n->data == m2, "move is previous current" );
    ok( m != m2, "current and previous are different" );
    ok( m == ggtl_peek_move(g), "first move is now current" );

    free(n->data);
    free(n);
  }

  ok( s = ggtl_peek_state(g), "peeked at state" );
  ok( s->player == 2, "player undone" );
  ok( s->value == 3, "move undone" );

  ok( s = ggtl_undo(g), "undo: got state" );
  ok( s == state, "..and it's the initial state" );
  ok( s->player == 1, "player undone" );
  ok( s->value == 5, "move undone" );

  ok( !ggtl_game_over(g), "game is still on" );

  ok( m = nim_move_new(5), "got move" );
  ok( s = ggtl_move(g, m), "performed move" );
  ok1( s->value == 0 );

  ok( ggtl_game_over(g), "game is over now" );
  ok1( ggtl_undo(g) );


  /* test basics of ai operations */
  ggtl_set(g, TYPE, NONE);
  ok( state = ggtl_ai_move(g), "computer move" );
  ok( state->player == 2, "changed player" );
  ok( state->value == 2, "changed value" );

  ggtl_set(g, TYPE, RANDOM);
  ok( state = ggtl_ai_move(g), "random move" );
  ok( state->player == 1, "changed player" );
  ok( state->value != 2, "changed value" );

  ggtl_free(g);
	return exit_status();
}
