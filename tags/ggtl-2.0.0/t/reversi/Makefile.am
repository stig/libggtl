ctests                  = get_moves.t \
                          eval.t \
                          iterative.t \
                          time.t

ptests                  = $(srcdir)/move.t \
                          $(srcdir)/trace.t

phelpers                = move

check_PROGRAMS          = $(ctests) $(phelpers)
CFLAGS                  = -I$(top_srcdir) -W -Wall -Werror
LDFLAGS                 = -ltap
LDADD                   = $(top_builddir)/libggtl.la

# c tests
get_moves_t_SOURCES	= get_moves.c
eval_t_SOURCES		= eval.c
iterative_t_SOURCES	= iterative.c
time_t_SOURCES		= time.c

# helpers
move_SOURCES		= move.c

EXTRA_DIST = $(ptests)
