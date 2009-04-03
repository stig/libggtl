#include <tap.h>
#include <stdlib.h>
#include <ggtl/nim.h>

int main(void)
{
  GGTL *g;
  struct nim_state *s;

  plan_tests(12);

  ok1( s = nim_state_new(1, 5) );
  ok1( g = ggtl_new() );
  ok1( g = nim_init(g, s) );

  ggtl_set(g, TRACE, -2);
  ggtl_set(g, TYPE, FIXED);
  ggtl_set(g, PLY, 3);

  ok1( s = ggtl_ai_move(g) );
  ok( 2 == s->value, "expected 2, got: %d", s->value );
  ok1( 11 == ggtl_get(g, VISITED) );

  ok1( s = ggtl_ai_move(g) );
  ok( 1 == s->value, "expected 1, got: %d", s->value );
  ok1( 3 == ggtl_get(g, VISITED) );

  ok1( s = ggtl_ai_move(g) );
  ok( 0 == s->value, "expected 0, got: %d", s->value ); 
  ok( 0 == ggtl_get(g, VISITED), "expected 0, got: %d", ggtl_get(g, VISITED));

  ggtl_free(g);
	return exit_status();
}
