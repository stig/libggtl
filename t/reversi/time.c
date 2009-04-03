#include <tap.h>
#include <stdio.h>
#include <time.h>
#include <sl/sl.h>
#include <ggtl/reversi.h>

int main(void)
{
  GGTL *g;
  int ply, size = 6;

  plan_tests(9);
  
  /* now test that a slew of moves take roughly the right time */
  for (ply = 1; ply < 4; ply++) {
    time_t start;
    int duration, moves = size * size - 4; 
    float upper, lower;

    ok( g = reversi_init(ggtl_new(), reversi_state_new(size)), "setup ok" );
    ggtl_set(g, TYPE, ITERATIVE);
    ggtl_set(g, MSEC, ply * 100);

    start = time(NULL);
    while (ggtl_ai_move(g))
      ;
    duration = time(NULL) - start;

    upper = moves * ply / 10.0;
    lower = (moves - moves / 10) * ply / 20.0;

    ok( duration < upper, 
      "duration (%d) below upper bound (%f)", duration, upper );
    ok( duration > lower, 
      "duration (%d) above lower bound (%f)", duration, lower );

    ggtl_free(g);
  }

  return exit_status();
}
