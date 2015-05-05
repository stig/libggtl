

# Generic Game-Tree search Library #

## Synopsis ##

```
  #include <ggtl/core.h>
  
  GGTL *ggtl_new(void);
  GGTL *ggtl_init(GGTL *g, void *s);
  GGTL_VTAB *ggtl_vtab(GGTL *g);
  void ggtl_free(GGTL *g);
  
  void *ggtl_move(GGTL *g, void *m);
  void *ggtl_ai_move(GGTL *g);
  void *ggtl_undo(GGTL *g);
  
  void ggtl_set(GGTL *g, int key, int value);
  int ggtl_get(GGTL *g, int key);
  void ggtl_set_float(GGTL *g, int key, float value);
  float ggtl_get_float(GGTL *g, int key);
  
  void *ggtl_peek_state(GGTL *g);
  void *ggtl_peek_move(GGTL *g);
  
  GGTL_MOVE *ggtl_get_moves(GGTL *g);
  int ggtl_game_over(GGTL *g);
  int ggtl_eval(GGTL *g);
  
  GGTL_STATE *ggtl_wrap_state(GGTL *g, void *state);
  GGTL_MOVE *ggtl_wrap_move(GGTL *g, void *move);
  
  void ggtl_cache_states(GGTL *g, GGTL_STATE *list);
  void ggtl_cache_state(GGTL *g, void *state);
  void ggtl_cache_moves(GGTL *g, GGTL_MOVE *list);
  void ggtl_cache_move(GGTL *g, void *move);
  GGTL_STATE *ggtl_uncache_state(GGTL *g);
  GGTL_MOVE *ggtl_uncache_move(GGTL *g);
  void *ggtl_uncache_state_raw(GGTL *g);
  void *ggtl_uncache_move_raw(GGTL *g);
  void ggtl_cache_free(GGTL *g);
```

## Description ##

GGTL exists to make it simple to create strategic games in C. The AI provided by GGTL can play any two-player, zero-sum game with perfect information.
GGTL is not magic, however; it must be supplied with game-secific callback functions in order to work. More information about these callback functions can be found in the ggtlcb(3) manpage.
The functions you have to care about when starting out using GGTL are: ggtl\_new(), ggtl\_init() and ggtl\_vtab() for setting up and initialising a game; and ggtl\_move(), ggtl\_ai\_move() and ggtl\_undo() for use during the course of the game.
GGTL ships with an extension for Reversi (aka Othello). This extension provides all the callbacks necessary in order to play Reversi (Othello). See reversi(3) for its manpage.


## Data types ##

GGTL stores its internal state in a structure of type GGTL. You are not allowed to poke around inside this structure, so it is hidden behind an opaque pointer.
Lists of states and moves are used extensively in GGTL, so states are often wrapped in GGTL\_STATE containers and moves in GGTL\_MOVE containers. These structures have a next member pointing to the next node in the chain, and a data member pointing to the state or move. See ggtl\_wrap\_state() & ggtl\_wrap\_move().
The sl(3) library is used to manage lists internally. See its documentation for details if you are developing your own get\_moves() callback function, or want to traverse the list returned from a call to ggtl\_get\_moves(). These are the only cases where users of GGTL are directly exposed to linked lists.
GGTL stores pointers to the callback functions it uses in a GGTL\_VTAB. This is part of what makes GGTL flexible and generic; as long as suitable game-dependent functions are assigned to the entries in this structure GGTL's AI can play any 2-player zero-sum game with perfect information. See also ggtl\_vtab() below and the ggtlcb(3) manpage.


## Functions ##

Almost all of GGTL's functions take a pointer to a GGTL as their first argument. For brevity, the type has been omitted from the prototypes below, when they appear as the first argument. Thus, instead of GGTL **g,**g is simply used instead.

### Setup and cleanup ###

If you are using one of GGTL's extensions, you will only need the first two of these.

#### `GGTL *ggtl_new()` ####
Returns a new ggtl structure, or NULL on failure.

#### `void ggtl_free( *g )` ####

Free the memory held in a ggtl struct.

#### `GGTL *ggtl_init( *g, void *state )` ####

Set the starting position to state. Returns g on success, NULL on failure.

#### `GGTL_VTAB *ggtl_vtab( *g )` ####

Returns a pointer to g's vtable. Pointers to all the callback functions used by GGTL are stored in this structure. See the ggtlcb(3) manpage for details on each entry.

### Moving and undo ###

#### `void *ggtl_move( *g, void *move )` ####

Apply the given move to the current position. A pointer to the resulting new state is returned on success, NULL on failure.
Upon success, the move is stored in a history list. If the unmove() callback is not provided, the previous state is also stored in order to provide undo.

#### `void *ggtl_ai_move( *g )` ####

Make the GGTL AI perform a move. Returns a pointer to the new game state, or NULL on error.
The same internal mechanisms are used as for the ggtl\_move(), so the AI's moves can also be undone.

#### `void *ggtl_undo( *g )` ####

Reverts the internal game state to the previous. Returns a pointer to the new current state, or NULL on error (e.g. if there was no move to undo).

### Run-time options ###

#### `void ggtl_set( *g, int key, int value )` ####
#### `int ggtl_get( *g, int key )` ####

Simple way to set/get the value for a given key. Some key/value pairs are not available through the use of these functions; for those you need ggtl\_set\_float() and ggtl\_get\_float().

#### `void ggtl_set_float( *g, int key, float value )` ####

#### `float ggtl_get_float( *g, int key )` ####

Get/set the values of the given keys. In contrast to ggtl\_set() and ggtl\_get(), these functions can be used to set and retrieve the TIME parameter, which is a floating point number. Take care, however, to provide values of the correct type (by casting them if necessary), lest bad things will happen.
The following keys are available:

  * TYPE (int) - the type of AI to use
> > See ggtlai(3) for a description of the various types supported.
  * MSEC [deprecated](deprecated.md)
> > The maximum time (in milliseconds) the iterative AI is allowed to use for a search. This option is deprecated. Use `TIME` instead.
  * TIME (float)
> > The time to search in seconds. This is a floating-point value so you can get sub-millisecond granularity. This option is only available through the use of the new ggtl\_set\_float() and ggtl\_get\_float() functions.
  * PLY (int)
> > The depth to search to for the fixed-depth AI.
  * TRACE (int)
> > The level of trace information to print during search. The trace information will be printed to standard output. Zero (the default) turns off tracing; larger numbers give more detailed trace output.
  * CACHE (int)
> > Decide what to cache. If 0, GGTL will not cache anything. Other valid values are combinations of the following, ORed together:
      * STATES  - cache states
      * MOVES   - cache moves
Example: use `ggtl_set(g, CACHE, STATES | MOVES)` to cache both moves and states (this is the default).
  * VISITED (int) - (getting only)
> > Returns the number of states visited by the last AI search, or -1 if no information is available. If no search has taken place, the value is undefined.
  * PLY\_REACHED (int) - (getting only)
> > Returns the effective depth of the last iterative AI search. The value is undefined if no such search has taken place.

### Misc ###
#### `void *ggtl_peek_state( *g )` ####

Returns a pointer to the current state, or NULL on error.

#### `void *ggtl_peek_move( *g )` ####
Returns a pointer to the last move performed, or NULL on error.

#### `GGTL_MOVE *ggtl_get_moves( *g )` ####

Returns a list of the available moves at the current state, or NULL if there are no available moves (i.e, if the game is over).

#### `int ggtl_game_over( *g )` ####
Returns true if the current game state is a final state, or false if the game is still on.

#### `int ggtl_eval( *g )` ####
Returns the value of the current game state.

#### `GGTL_STATE *ggtl_wrap_state(*g, void *state)` ####
Returns the state, wrapped in a GGTL\_STATE node, or NULL if an error occurs.

#### `GGTL_MOVE *ggtl_wrap_move(*g, void *move)` ####
Returns the move wrapped in a GGTL\_MOVE container, or NULL if an error occurs.

### Interaction with GGTL's cache of states and moves ###
GGTL agressively caches states and moves internally to avoid calling malloc(3) and free(3) more than necessary, and it exposes this cache to its user. This becomes particularly useful if you are implementing your own get\_moves() or move() callback function.
See also CACHE under Run-time options for configuring what to cache.

#### `void ggtl_cache_states( *g, GGTL_STATE *states )` ####
#### `void ggtl_cache_state( *g, void *state )` ####
#### `void ggtl_cache_moves( *g, GGTL_MOVE *moves )` ####
#### `void ggtl_cache_move( *g, void *move )` ####
Put states and moves onto their respective caches. The pluralised versions take a list of nodes, the singular ones takes a pointer to a bare state or move and caches that. Remember that a list can have one or more elements.
Note: these functions change behaviour depending on the cache flag (see ggtl\_cache()). If it is turned off for the given type, the memory will simply be freed instead.

#### `GGTL_STATE *ggtl_uncache_state( *g )` ####
#### `GGTL_MOVE *ggtl_uncache_move_raw( *g )` ####
#### `void *ggtl_uncache_state_raw( *g )` ####
#### `void *ggtl_uncache_move_raw( *g )` ####
Retrieve a state or a move from its respective cache. The ggtl\_uncache**raw versions drops the wrapper node and returns pointers to the actual state/move.**

#### `void ggtl_cache_free( *g )` ####
Free all memory used by the cache. The use of this function should normally not be necessary.


## (AB)USE ##
GGTL can be used even if your game is not one of those that can be solved by its AI. For example, it can be convenient to use GGTL for keeping track of history and providing undo.



## See also ##

ggtltut(3) shows how to implement a simple Tic-Tac-Toe game using GGTL.

See ggtlai(3) for the various AIs supported by ggtl\_ai\_move().

ggtlcb(3) documents the callback functions required by GGTL to support game-tree search for a whole range of games.

reversi(3) & nim(3) documents extensions to GGTL providing all the callbacks necessary to implement Reversi and Nim.

Generic Game Framework in Eiffel (G2F3) is a related project, but--as the name implies--programmed in Eiffel:

> http://freshmeat.net/projects/g2f3/


## Author ##
Stig Brautaset <stig@brautaset.org>


## Thanks ##
Thanks to Steven Goodwin for constructive comments.


## Copyright ##
Copyright (C) 2005-2006 Stig Brautaset

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.