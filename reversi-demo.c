/*

=head1 NAME

reversi-demo - a self-playing reversi demo using GGTL 

=head1 SYNOPSIS

  reversi-demo 
  reversi-demo SIZE 
  reversi-demo SIZE PLY 
  reversi-demo SIZE PLY TRACEPLY

=head1 DESCRIPTION

L<reversi-demo(3)> is a tiny program using L<ggtl-reversi(3)> to
create a self-playing Reversi (aka Othello) game.

If invoked without arguments (or with only some of the arguments
given), the defaults for SIZE, PLY and TRACEPLY are 6, 3, and
-1.

=head1 SOURCE

The full source for reversi-demo(3) is:

=pod */

  #include <stdio.h>
  #include <stdlib.h>
  #include <ggtl/reversi.h>
  
  int main(int argc, char **argv)
  {
    RState *state;
    GGTL *g;
    int size;
    int ply;
    int trace;
  
    size = argc > 1 ? atoi(argv[1]) : 6;
    ply = argc > 2 ? atoi(argv[2]) : 3;
    trace = argc > 3 ? atoi(argv[3]) : 1;
    if (size % 2) {
      size++;
    }
  
    state = reversi_state_new(size);
    g = reversi_init(ggtl_new(), state);
    ggtl_set(g, TYPE, FIXED);
  
    if (!g && !state) {
      puts("cannot allocate enough memory");
      return EXIT_FAILURE;
    }
  
    ggtl_set(g, PLY, ply);
    ggtl_set(g, TRACE, trace);
    do {
      reversi_state_draw(state);
      putchar('\n');
    } while (state = ggtl_ai_move(g));
  
    if (0 == ggtl_eval(g)) {
      puts("game ended in a draw");
    }
    else {
      state = ggtl_peek_state(g);
      printf("player %d %s\n", state->player, 
        0 < ggtl_eval(g) ? "won" : "lost");
    }
  
    ggtl_free(g);
    return 0;
  }
