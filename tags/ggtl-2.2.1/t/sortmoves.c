#include <tap.h>
#include <stddef.h>
#include <ggtl/core.h>
#include <ggtl/private.h> /* cheat */
#include <sl/sl.h>


int main(void)
{
  GGTL_MOVE *a, *b, *c, *d;

  plan_tests(4);

  a = ggtl_mc_new(NULL);
  a->fitness = 0;

  b = ggtl_mc_new(NULL);
  b->fitness = -10;

  c = ggtl_mc_new(NULL);
  c->fitness = 10;
  
  d = ggtl_mc_new(NULL);
  d->fitness = 1;

  a->next = b;
  b->next = c;
  c->next = d;

  a = sl_mergesort(a, fitness_cmp);
  b = a->next;
  c = b->next;
  d = c->next;

  ok1( a->fitness == 10 );
  ok1( b->fitness == 1 );
  ok1( c->fitness == 0 );
  ok1( d->fitness == -10 );
  
  return exit_status();
}
