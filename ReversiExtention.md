# Reversi Extension #

## SYNOPSIS ##

```
  #include <ggtl/reversi.h>
  
  GGTL *reversi_init(GGTL *g, void *state);
  RMove *reversi_move_new(int x, int y, GGTL *g);
  RState *reversi_state_new(int size);
  void reversi_state_draw(RState *state);
  RStateCount reversi_state_count(RState *state);
  
  /* callback functions used by ggtl core */
  void *reversi_state_clone(void *state, GGTL *g);
  int reversi_eval(void *state, GGTL *g);
  void reversi_state_free(void *state);
  GGTL_MOVE *reversi_get_moves(void *state, GGTL *g);
  void *reversi_move(void *s, void *mv, GGTL *g);
```

See reversi-demo(3) for a complete example of a self-playing Reversi game using this extension.


## DESCRIPTION ##

GGTL-Reversi is a GGTL extension for playing Reversi (aka Othello). It provides all the callback functions and datastructures GGTL needs to provide a Reversi AI, leaving you free to worry about other aspects of the game.
All even board sizes greater than 4x4 are supported and can be specified at run time.


## DATA STRUCTURES ##
Three data structures are used by this GGTL extension. They are:

```
  typedef struct reversi_state {
    int player;
    int size;
    int **board;
  } RState;

  typedef struct reversi_move {
    int x;
    int y;
  } RMove;

  typedef struct reversi_counts {
    int c[3];
  } RStateCount;
```

## FUNCTIONS ##

### `RState *reversi_state_new( int size )` ###
Returns a reversi state with a board of the desired size, or NULL on failure. The board is set up for the beginning of a game and player 1 is set to start.

### `void *reversi_state_clone( void *s, GGTL *g )` ###
Clone the state s (using a cached state from g if available). Return the cloned state, or NULL on error.
It is assumed that cached states are the same size as the one being cloned.

### `void reversi_state_draw( RState *s )` ###
Print a plain-text representation of a state to standard out.

### `RMove *reversi_move_new( int x, int y )` ###
Returns a new move, or NULL on failure.

### `GGTL_MOVE *reversi_move_new_wrapped( int x, int y, *g )` ###
Returns a new move, wrapped in a ggtl\_mc container, or NULL on failure. If g is non-NULL it will be asked for a cached move; otherwise, a new move will be allocated.

### `GGTL *reversi_init(GGTL *g, void *state)` ###
Initialises a GGTL structure's vtable with functions for playing Reversi and set the provided state to be the starting state of the game.

### `RStateCount reversi_state_count(RState *s)` ###
Returns a structure containing the counts of empty, white & black squares in the given state.


## CALLBACK FUNCTIONS ##
These functions are used internally by GGTL; it is unlikely that you should have to worry about them. However, you are allowed to override these should you wish. See ggtl\_vtab().

### `int reversi_eval( void *state, GGTL *g )` ###
Evaluate a reversi state and return its fitness.

### `void *reversi_move( void *state, void *move, GGTL *g )` ###
Returns the state resulting from applying move to state, or NULL on failure.

### `GGTL_MOVE *reversi_get_moves( void *state, GGTL *g )` ###
Returns a list of the available moves at the given position, or NULL if no moves could be found.

### `void reversi_state_free( void *state )` ###
Free up the memory held up by a state. This overrides the default free\_state() callback supplied by GGTL.


## SEE ALSO ##

FunctionReference

## THANKS ##
The code for performing a move was inspired by code found in Gnome Iagno by Ian Peters.