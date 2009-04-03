#include <tap.h>
#include <sl/sl.h>
#include <ggtl/reversi.h>

int main(void)
{
  GGTL *g;
  RState *s;
  RMove *m;

  plan_tests(14);

  g = ggtl_new();
  s = reversi_state_new(6);
  g = reversi_init(g, s);

  ok( 0 == ggtl_eval(g), "starting state is neutral" );

  ok( m = reversi_move_new(1, 2), "got move" );
  ok( s = ggtl_move(g, m), "performed move" );

  ok( -3 == ggtl_eval(g), "2nd state has disadvantage" );

  ok( m = reversi_move_new(3, 1), "got move" );
  ok( s = ggtl_move(g, m), "performed move" );

  ok( 1 == ggtl_eval(g), "3rd state has advantage" );

  /* ok, cheating a bit to test end-game evaluation... */
  ok( ggtl_undo(g), "undo" );
  ok( m = reversi_move_new(-1, -1), "got pass move" );
  ok( s = ggtl_move(g, m), "passed" );

  ok( m = reversi_move_new(4, 3), "winning move" );
  ok( s = ggtl_move(g, m), "passed" );

  ok( FITNESS_MIN == ggtl_eval(g), "oops. game lost" );
  ok( ggtl_game_over(g), "yep, game is over" );

  ggtl_free(g);
  return 0;
}
