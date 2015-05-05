# Artificial Intelligence Types #

## SYNOPSIS ##

```
  #include <ggtl/core.h>
  
  /* set the current AI type */
  ggtl_set(g, TYPE, X);
  /* get the current AI */
  ggtl_get(g, TYPE);
  /* force a move */
  ggtl_move(g, m);
  /* make the current AI perform a move */
  ggtl_ai_move(g);
```

## DESCRIPTION ##
GGTL supports a few different modes for the AI player. The various AIs can be chosen by calling ggtl\_set(g, TYPE, 

&lt;type&gt;

). The accepted types are:
  * NONE
> > Not really an AI. Always picks the first move returned by the get\_moves() callback. Can be useful for testing.
  * RANDOM
> > Not really an AI either. Picks one of the available moves at random. It is the responsibility of the user to call srand() to make sure the random generator is seeded differently for each run.
  * FIXED
> > Performs a fixed-depth Alpha-Beta search to find the best move at a given ply (depth in the game tree). The depth to be searched can be set with ggtl\_set(g, PLY, depth).
  * ITERATIVE
> > This AI performs an iterative deepening Alpha-Beta search to find the best possible move at the greatest depth that can be searched in a given time. The time allowed for a search can be set with ggtl\_set(); use ggtl\_set(g, TIME, 0.350) to set the allowed time to 350 milliseconds.


## SEE ALSO ##

  * FunctionReference