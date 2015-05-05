# Nim Extension #

## SYNOPSIS ##

```
  #include <ggtl/nim.h>
==    ==
  GGTL *g = ggtl_new_nim();
  struct nim_state *s = ...;
  nim_init(g, s);
```

## DESCRIPTION ##
This is a minimal GGTL extension used mainly in GGTL's test suite.


## FUNCTIONS ##
### `GGTL *nim_init(GGTL *g, void *state)` ###
Initialises a GGTL's vtable with functions for playing Nim, and set the provided state to be the starting state of the game.
### `struct nim_state *nim_state_new( int player, int val )` ###
Return a new state with player and value set to the given arguments, or NULL on error.
### `nim_move_new( int val )` ###
Return a new nim move, or NULL on error.


## CALLBACK FUNCTIONS ##
### `int nim_eval( void *state, GGTL *g )` ###
Returns the fitness of a gamestate from the view of the current player. A non-ending state has a fitness of 0, a finishing state has a fitness of 1. It is a winning state for that player, as the previous player was forced to take the last piece.
### `void *nim_move( void *state, void *move, GGTL *g )` ###
Returns the state resulting from applying move to state, or NULL on failure.
### `void *nim_unmove( void *state, void *move, GGTL *g )` ###
Returns the state resulting from reversing move on state, or NULL on failure.
### `GGTL_MOVE *nim_get_moves( void *state, GGTL *g )` ###
Returns a list of the available moves at the given position, or NULL if no moves could be found.


## SEE ALSO ##

  * FunctionReference
  * ReversiExtension