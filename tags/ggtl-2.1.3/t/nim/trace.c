#include <stdio.h>
#include <assert.h>
#include <ggtl/nim.h>

int main(void)
{
  GGTL *g;

  g = nim_init(ggtl_new(), nim_state_new(1, 5));
  assert(g != NULL);

  ggtl_set(g, TRACE, 20);
  ggtl_set(g, TYPE, FIXED);
  ggtl_set(g, PLY, 3);

  while (ggtl_ai_move(g)) {
    putc('\n', stdout);
  }

  ggtl_free(g);
	return 0;
}
