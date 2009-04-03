#include <tap.h>
#include <assert.h>
#include <ggtl/core.h>

int main(void)
{
  GGTL *g;

  plan_tests(8);
  
  g = ggtl_new();
  ok( g, "setup ok" );

  ok1( 5 == SET_KEYS );
  ok1( ITERATIVE == ggtl_get(g, TYPE) );
  ok1( 3 == ggtl_get(g, PLY) );
  ok1( 200 == ggtl_get(g, MSEC) );
  ok1( 0 == ggtl_get(g, TRACE) );
  ok1( (STATES | MOVES) == ggtl_get(g, CACHE) );

  ok1( 2 == GET_KEYS - SET_KEYS);

  ggtl_free(g);
  return 0;
}
