#include <tap.h>
#include <stdlib.h>
#include <sl/sl.h>
#include <ggtl/reversi.h>

int main(void)
{
  GGTL *g;
  RState *s;
  RMove *m;
  GGTL_MOVE *list;

  plan_tests(16);

  g = ggtl_new();
  s = reversi_state_new(6);
  g = reversi_init(g, s);

  ok( list = ggtl_get_moves(g), "got list of moves" );
  ok( 4 == sl_count(list), "4 moves available" );
  sl_free(list, free);

  ok( m = reversi_move_new(1, 2), "got move" );
  ok( s = ggtl_move(g, m), "performed move" );

  ok( list = ggtl_get_moves(g), "got list of moves" );
  ok( 3 == sl_count(list), "3 moves available now" );
  sl_free(list, free);

  ggtl_free(g);

  /* now cheat and check some weird positions
    ....
    xo..
    ....
    .... - o to move
  */
  s = reversi_state_new(4);
  {
    int i, j;
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        s->board[i][j] = 0;
      }
    }
    s->board[1][0] = 2;
    s->board[1][1] = 1;
  }  

  g = ggtl_new();
  g = reversi_init(g, s);

  ok( list = ggtl_get_moves(g), "got list of moves" );
  ok( 1 == sl_count(list), "only one move" );

  m = list->data;
  ok( -1 == m->x && -1 == m->y, "..it's the pass move" );

  ok( ggtl_move_internal(g, list), "pass" );

  ok( list = ggtl_get_moves(g), "got list of moves" );
  ok( 1 == sl_count(list), "only one move" );

  m = list->data;
  ok( -1 != m->x && -1 != m->y, "..it's _not_ the pass move" );
  ok( ggtl_move_internal(g, list), "take it" );

  ok( !ggtl_get_moves(g), "no more moves possible" );
  ok( ggtl_game_over(g), "game is over" );

  ggtl_free(g);
	return 0;
}
