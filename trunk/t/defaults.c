#include <math.h>

#include <tap.h>
#include <ggtl/core.h>

int main(void)
{
  GGTL *g;
  double d;

  plan_tests(11);
  
  g = ggtl_new();
  ok( g, "setup ok" );

  ok1( 6 == SET_KEYS );
  ok1( ITERATIVE == ggtl_get(g, TYPE) );
  ok1( 3 == ggtl_get(g, PLY) );
  ok1( 200 == ggtl_get(g, MSEC) );
  
  ggtl_getval(g, TIME, &d);
  ok1( fabs(d) - 0.2 <= 0.0000001 );
    
  ggtl_set(g, MSEC, 350);
  ggtl_getval(g, TIME, &d);
  ok1( fabs(d) - 0.35 <= 0.0000001 );
  ok1( 350 == ggtl_get(g, MSEC) );
  
  ok1( 0 == ggtl_get(g, TRACE) );
  ok1( (STATES | MOVES) == ggtl_get(g, CACHE) );

  ok1( 2 == GET_KEYS - SET_KEYS);

  ggtl_free(g);
  return exit_status();
}
