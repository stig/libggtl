# Callback Reference #

## SYNOPSIS ##

```
  #include <ggtl/core.h>
  ggtl_vtab(g)->eval      = &my_eval;
  ggtl_vtab(g)->get_moves = &my_get_moves;
```


## DESCRIPTION ##


GGTL is a generic implementation of the Alpha-Beta game-tree search algorithm. It must be provided with game-specific callback functions in order to perform operations such as executing a move and evaluating the fitness of a game state.

The full list of callback functions is:

### `void *move(void *state, void *mv, GGTL *g)` ###
Apply mv to state, producing the successor state. Return the pointer to the new state, or NULL on failure. If the game allows pass moves, this function must understand them. It is has the responsibility of updating the indication on which player's turn it is (a property of the game state).
g is passed in so that you can benefit from GGTL's caches; see e.g. ggtl\_uncache\_state() and ggtl\_uncache\_state\_raw().
NOTE: The prototype of this callback changed from v2.1.0. GGTL now expects the passed-in state to be modified. You should additionally implement either the unmove() or clone\_state() callbacks (for backwards compatibility you may omit both of these and return a new wrapped state as before--this behaviour is deprecated, however, and will likely dissapear in a later version).

### `void *unmove(void *state, void *mv, GGTL *g)` ###
Optional callback to revert the passed-in move from the passed-in state (without taking a copy). By providing this callback GGTL will not keep a copy of each state for its undo-functionality.

The callback should return a valid pointer on success and NULL on failure.

### `void *clone_state(void *state, GGTL *g)` ###
Optional callback to clone state. This is used to copy the current state before passing it to the move() callback. It should return a pointer to the cloned state or NULL on failure.

### `int eval(void *state, GGTL *g)` ###
Should be implemented to calculate and return the fitness (MiniMax) value of state. This value must be in the range GGTL\_FITNESS\_MIN to GGTL\_FITNESS\_MAX.

### `GGTL_MOVE *get_moves(void *state, GGTL *g)` ###
Should return a list of all the moves available to the current player at the given state, or NULL if the game is over. Hence, if passing is allowed, a special 'pass move' must be returned.
The ggtl\_wrap\_move() function and the sl manpage may come in handy.

### `int game_over(void *state, GGTL *g)` ###
Optional callback to check for an end-state of the game.
If specified, must return non-zero if state is a final state in the current game. For example, some games automatically end after a certain number of moves; this can be simply represented by this function.
Not all games need this callback; for many games it is enough to just let get\_moves() return NULL to signal that the game is over.

### `void free_state(void *state)` ###
### `void free_move(void *move)` ###
Optional callbacks for freeing up the memory held by a state and a move, respectively. This is rarely done, as GGTL caches moves and states agressively by default. You may chose to do so, however.
You only have to worry about these if your states or moves requires manual cleaning up. The standard C function free(3)|free is used if you don't specify anything.