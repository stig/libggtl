#include <stdio.h>
#include <assert.h>
#include <ggtl/reversi.h>

int main(void)
{
  GGTL *g;
  RState *s;
  RMove *m;

  g = ggtl_new();
  s = reversi_state_new(6);
  g = reversi_init(g, s);

  reversi_state_draw(s);
  putchar('\n');

  /* flip up */
  m = reversi_move_new(1, 2);
  s = ggtl_move(g, m);
  assert(s != NULL);
  reversi_state_draw(s);
  putchar('\n');

  /* flip left */
  m = reversi_move_new(3, 1);
  s = ggtl_move(g, m);
  assert(s != NULL);
  reversi_state_draw(s);
  putchar('\n');

  /* flip down */
  m = reversi_move_new(4, 3);
  s = ggtl_move(g, m);
  assert(s != NULL);
  reversi_state_draw(s);
  putchar('\n');

  /* flip right */
  m = reversi_move_new(3, 4);
  s = ggtl_move(g, m);
  assert(s != NULL);
  reversi_state_draw(s);
  putchar('\n');

  /* now diagonals */

  /* flip down & left */
  m = reversi_move_new(4, 0);
  s = ggtl_move(g, m);
  assert(s != NULL);
  reversi_state_draw(s);
  putchar('\n');

  /* flip up & right */
  m = reversi_move_new(1, 4);
  s = ggtl_move(g, m);
  assert(s != NULL);
  reversi_state_draw(s);
  putchar('\n');

  /* flip up & right */
  m = reversi_move_new(4, 4);
  s = ggtl_move(g, m);
  assert(s != NULL);
  reversi_state_draw(s);
  putchar('\n');

  /* flip up & right */
  m = reversi_move_new(0, 1);
  s = ggtl_move(g, m);
  assert(s != NULL);
  reversi_state_draw(s);
  putchar('\n');

  ggtl_free(g);
	return 0;
}
