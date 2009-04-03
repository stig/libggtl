#include <tap.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <ggtl/core.h>

int main(void)
{
  GGTL *g;

  plan_tests(16);
  
  g = ggtl_new();
  assert(g != NULL);

  ok1( !ggtl_uncache_state_raw(g) );
  ok1( !ggtl_uncache_move_raw(g) );

  {
    int m, m1, s, s1;
    GGTL_STATE *sc = ggtl_sc_new(&s);
    GGTL_MOVE *mc = ggtl_mc_new(&m);

    ggtl_cache_states(g, sc);
    ggtl_cache_state(g, &s1);
    ok1( sc = ggtl_uncache_state(g) );
    ok( sc->data == &s1, "is same state" );
    ok1( &s == ggtl_uncache_state_raw(g) );

    ggtl_cache_moves(g, mc);
    ggtl_cache_move(g, &m1);
    ok1( mc = ggtl_uncache_move(g) );
    ok( mc->data == &m1, "is same move" );
    ok1( &m == ggtl_uncache_move_raw(g) );

    ok1( sc = ggtl_wrap_state(g, &s) );
    ok1( mc = ggtl_wrap_move(g, &m) );
    ggtl_cache_states(g, sc);
    ggtl_cache_moves(g, mc);

    ok1( &m == ggtl_uncache_move_raw(g) );
    ok1( &s == ggtl_uncache_state_raw(g) );
  }

  ok1( !ggtl_uncache_state_raw(g) );
  ok1( !ggtl_uncache_move_raw(g) );
  ok1( !ggtl_uncache_state(g) );
  ok1( !ggtl_uncache_move(g) );

  ggtl_free(g);

  return exit_status();
}
