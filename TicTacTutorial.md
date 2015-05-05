

# TIC TAC TUTORIAL #

## SYNOPSIS ##

This article describes the steps involved in creating a self-playing Tic-Tac-Toe game using an AI provided by GGTL.

The ggtlcb(3) manpage describes the various callback functions GGTL supports. Which ones you have to provide when implementing a game depends on the game in question, so the first thing to do is chose one. I've chosen Tic-Tac-Toe, as it is relatively simple but illustrates what is going on quite well.

### Header files ###
We obviously need to include the GGTL header, ggtl/core.h. The sl/sl.h header is for the sl(3) linked-list library. You're not required to use it, but as it is used internally by GGTL, doing so incurs no extra cost on your memory usage.

```
  #include <ggtl/core.h>
  #include <sl/sl.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <assert.h>
  #include <string.h>
```

### Chosing suitable data structures ###

We first have to define a structure for our states. One of the things that might be a bit hard to get used to at first is that GGTL makes it the responsibility of the states to know which player's turn it is to move. The main reason for this is that it lets the programmer chose how to represent the players. Want to use a and b for the players? 1 and 2? x and y? Fine. They're all supported, and there's no extra hoops to jump through.

We will use x and o for our players. For the board, we'll use a 3-by-3 grid of integers, where a zero means an empty location.

```
typedef struct {
    int player;
    int grid[3][3];
} ttt_state_t;
```

Moves are simpler. They consist of simply an x and an y coordinate.

```
typedef struct {
    int x, y;
} ttt_move_t;
```

### Making moves ###

Performing a move simply means putting the current player in the grid location specified by the move, then advance the current player. We do that like so:

```
  void *ttt_move(void *st, void *mv, GGTL *g)
  {
    ttt_state_t *s = st;
    ttt_move_t *m = mv;
    s->grid[ m->x ][ m->y ] = s->player;
    s->player = s->player == 'x' ? 'o' : 'x';
    return s;
  }
```

### Never do anything you can't undo ###

GGTL's AI figures out which move to pick by recursively trying out every possible move at the current position and seing where they lead. Needless to say it wouldn't get very far doing this if it didn't know how to undo a move, so let's teach it how to do this.

Depending on personal preference (or attributes of the game you're implementing) you may chose to implement either the unmove() or clone\_state() callback. For Tic-Tac-Toe either one will do, but we will implement both for illustration purposes. First we have the unmove() callback. It is very similar to move():

```
  void *ttt_unmove(void *st, void *mv, GGTL *g)
  {
    ttt_state_t *s = st;
    ttt_move_t *m = mv;
    s->grid[ m->x ][ m->y ] = 0;
    s->player = s->player == 'x' ? 'o' : 'x';
    return s;
  }
```

Next up we have the clone\_state() callback:

```
void *ttt_clone(void *st, GGTL *g)
  {
    ttt_state_t *copy = malloc(sizeof *copy);
    assert(copy != NULL);
    
    (void)memmove(copy, st, sizeof *copy);
        
    return copy;
  }
```

We now have everything needed for using GGTL simply as a history manager, keeping track of the current position whilst providing unlimited undo.

### Working out your options ###

GGTL's AI needs help finding out what moves are available at each state. We do this by giving it a callback function that returns a list of available moves for the current player. Any grid location not currently occupied is a legal move, so this is really quite simple. We can use ggtl's cache to avoid gratuitous calls to malloc().

Note that in the interest of brevity and simplicity I've taken the liberty to exclude error checking from the below code. You should really check the return value of malloc().

```
  GGTL_MOVE *ttt_getmoves(void *st, GGTL *g)
  {
    ttt_state_t *s = st;
    int x, y;
    GGTL_MOVE *moves = NULL;
    for (x = 0; x < 3; x++) {
      for (y = 0; y < 3; y++) {
        if (!s->grid[x][y]) {
          ttt_move_t *m = ggtl_uncache_move_raw(g);
          if (!m) {
            m = malloc(sizeof *m);
          }
          m->x = x; 
          m->y = y;
          moves = sl_push(moves, ggtl_wrap_move(g, m));
        }
      }
    }
    return moves;
  }
```

### Evaluating your options ###

When searching for the best move, GGTL's AI needs to know which states are better than others. We will give it a simple function that returns 0 in the case of a draw or an unfinished game, and +1 or -1 if the current state is a win or a loss for the current player.

A much better evaluation function would not only indicate wins and losses but return a fitness value for intermediate states too. This would allow GGTL to make better decisions even if it doesn't have time to search through all the positions to the end of the game. This is left as an exercise for the reader (as it happens, Tic-Tac-Toe has such a small search space that searching through all positions is not a problem; hence, this doesn't matter if we let GGTL's AI look far enough ahead).

```
  int ttt_eval(void *st, GGTL *g)
  {
    ttt_state_t *s = st;
    int i, player;
  
    for (i = 0; i < 3; i++) {
      player = s->grid[i][0];
      if (player &&
          player == s->grid[i][1] &&
          player == s->grid[i][2]) {
        goto have_winner;
      }
  
      player = s->grid[0][i];
      if (player &&
          player == s->grid[1][i] &&
          player == s->grid[2][i]) {
        goto have_winner;
      }
    }
  
    player = s->grid[0][0];
    if (player &&
        player == s->grid[1][1] &&
        player == s->grid[2][2]) {
      goto have_winner;
    }
   
    player = s->grid[0][2];
    if (player &&
        player == s->grid[1][1] &&
        player == s->grid[2][0]) {
      goto have_winner;
    }
  
    return 0;
  
  have_winner:
    return player == s->player ? 1 : -1;
  }
```

### An exercise in visualisation ###

We now have all the components GGTL needs to provide an AI player for Tic-Tac-Toe. However, to enable us to visualize the game, we'll create a function that prints out the game state. Let's keep it simple though.

```
  void ttt_draw(ttt_state_t *s)
  {
    int x, y;
    for (x = 0; x < 3; x++) {
      for (y = 0; y < 3; y++) {
        int c = s->grid[x][y];
        putchar(c ? c : '.');
      }
      if (x < 2) putchar('\n');
    }
    printf(" - %c to move\n\n", s->player);
  }
```

### All together now ###

We're now ready to create our main() function. It is fairly standard and consists of setting up the inital state of our game, initializing GGTL, playing through the game, reporting who won, and cleaning up the mess (freeing memory etc).

Note that, once again, for brevity and clarity I've omitted error checking.

```
  int main(int argc, char **argv)
  {
    GGTL *g = ggtl_new();
    ttt_state_t *s = calloc(1, sizeof *s);
    s->player = 'x';
  
    ggtl_init(g, s);
  
    /* set the callback functions */
    ggtl_vtab(g)->move = ttt_move;
    ggtl_vtab(g)->get_moves = ttt_getmoves;
    ggtl_vtab(g)->eval = ttt_eval;
    if (argc > 1 && argv[1]) {
        puts("Using unmove()\n");
        ggtl_vtab(g)->unmove = ttt_unmove;
    
    } else {
        puts("Using clone_state()\n");
        ggtl_vtab(g)->clone_state = ttt_clone;
    }
    
    ggtl_set(g, TYPE, FIXED); /* fixed-depth alpha-beta search */
    ggtl_set(g, PLY, 9);      /* nine-move look-ahead */
  
    do {
      ttt_draw(s);
      s = ggtl_ai_move(g);
    } while (s);
  
    s = ggtl_peek_state(g);
    if (!ttt_eval(s, NULL)) {
      puts("The only winning move is not to play.");
    }
    else {
      printf("Player %d %s. This should never happen.\n", 
        s->player, 
        ttt_eval(s, NULL) > 0 ? "won" : "lost");
    }
    ggtl_free(g);
  
    return 0;
  }
```

And that's all it takes. We now have a self-playing Tic-Tac-Toe game. This program is bundled with GGTL and should be installed as ttt-demo if you have GGTL installed.


## AUTHOR ##
Stig Brautaset <stig@brautaset.org>


## THANKS ##

Kari Pahula for providing a documentation patch.