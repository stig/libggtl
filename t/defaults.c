#include <math.h>
#include <stdlib.h>

#include <tap.h>
#include <ggtl/core.h>

int main(void)
{
  GGTL *g;

  plan_tests(11);
  
  g = ggtl_new();
  ok( g, "setup ok" );

  ok1( 6 == SET_KEYS );
  ok1( ITERATIVE == ggtl_get(g, TYPE) );
  ok1( 3 == ggtl_get(g, PLY) );
  ok1( abs(200 - ggtl_get(g, MSEC)) <= 1 );
  ok1( fabs(ggtl_get_float(g, TIME) - 0.2) <= 0.0000001 );
    
  ggtl_set(g, MSEC, 350);
  ok1( fabs(ggtl_get_float(g, TIME) - 0.35) <= 0.0000001 );
  ok1( abs(350 - ggtl_get(g, MSEC)) <= 1 );
  
  ok1( 0 == ggtl_get(g, TRACE) );
  ok1( (STATES | MOVES) == ggtl_get(g, CACHE) );

  ok1( 2 == GET_KEYS - SET_KEYS);

  ggtl_free(g);
  return exit_status();
}
