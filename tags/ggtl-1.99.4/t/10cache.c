#include <tap.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <ggtl/core.h>

int main(void)
{
  GGTL *g;

  plan_tests(6);
  
  g = ggtl_new();
  assert(g != NULL);

  ok1( !ggtl_uncache_state(g) );
  ok1( !ggtl_uncache_move(g) );

  {
    GGTL_STATE *sc = ggtl_sc_new(NULL);
    GGTL_MOVE *mc = ggtl_mc_new(NULL);

    ggtl_cache_states(g, sc);
    ok1( sc = ggtl_uncache_state(g) );
    free(sc);

    ggtl_cache_moves(g, mc);
    ok1( mc = ggtl_uncache_move(g) );
    free(mc);
  }

  ok1( !ggtl_uncache_state(g) );
  ok1( !ggtl_uncache_move(g) );
  
  ggtl_free(g);

  return 0;
}
