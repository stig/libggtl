# Reversi Demo #

## SYNOPSIS ##

```
  reversi-demo 
  reversi-demo SIZE 
  reversi-demo SIZE PLY 
  reversi-demo SIZE PLY TRACEPLY
```

## DESCRIPTION ##

This shows how simple it is to create a self-playing Reversi (aka Othello) program with the GGTL reversi(3) extension. If you have GGTL installed, a program called reversi-demo should have been installed with it.
If invoked without arguments (or with only some of the arguments given), the defaults for SIZE, PLY and TRACEPLY are 6, 3, and -1.


## SOURCE ##
The full source for reversi-demo is:

```
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
      state = ggtl_ai_move(g);
    } while (state);
  
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
```
