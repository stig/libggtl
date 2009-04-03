#ifndef _ggtl_private_h
#define _ggtl_private_h

#define GGTL_ERR (GGTL_FITNESS_MAX+1)

struct ggtl {
  GGTL_VTAB *vtab;

  GGTL_STATE *state_cache;
  GGTL_MOVE *move_cache;
  GGTL_STATE *states;
  GGTL_MOVE *moves;
  GGTL_STATE *sc_cache;
  GGTL_MOVE *mc_cache;

  /* run-time options */
  int opts[GET_KEYS];
  float time_to_search;

  /* optimisation for end of search */
  int saw_end;
};

/* The various AIs */
GGTL_MOVE *ai_none(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_random(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_fixed(GGTL *g, GGTL_MOVE *);
GGTL_MOVE *ai_iterative(GGTL *g, GGTL_MOVE *);


/* Helper functions */
void ai_trace(GGTL *g, int level, char *fmt, ...);
int fitness_cmp(void *anode, void *bnode);

#endif /* !_ggtl_private_h */
