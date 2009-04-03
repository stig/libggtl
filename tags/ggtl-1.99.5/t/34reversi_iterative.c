#include <tap.h>
#include <stdio.h>
#include <time.h>
#include <sl/sl.h>
#include <ggtl/reversi.h>

int main(void)
{
  GGTL *g;
  int ply, size = 6;

  plan_no_plan();

  g = reversi_init(ggtl_new(), reversi_state_new(size));
  ok( g, "setup okay" );

  do {
    RState *s;
    RMove *m, *m2;
    GGTL_MOVE *mc;

    ggtl_set(g, TYPE, ITERATIVE);
    ok1( 200 == ggtl_get(g, MSEC) );
    ok1( ITERATIVE == ggtl_get(g, TYPE) );

    ok( s = ggtl_ai_move(g), "performed iterative search" );
    ply = ggtl_get(g, PLY_REACHED);
    ok( mc = ggtl_undo_internal(g), "save move we found" );
    ok( m = mc->data, "got real move" );

    ggtl_set(g, TYPE, FIXED);
    ggtl_set(g, PLY, ply);
    ok1( FIXED == ggtl_get(g, TYPE) );
    ok1( ply == ggtl_get(g, PLY) );

    ok( s = ggtl_ai_move(g), "fixed search to reached depth" );
    ok( m2 = ggtl_peek_move(g), "got move again" );
    ok( m->x == m2->x && m->y == m2->y, "the moves are the same" );
  
    ggtl_cache_moves(g, mc);
  } while (!ggtl_game_over(g));

  ggtl_free(g);
  return exit_status();
}
