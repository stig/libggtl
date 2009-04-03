ctests                  = 09defaults.t \
			  10cache.t \
                          11basic.t \
                          12sortmoves.t \
                          20nim_ai.t \
                          31reversi_get_moves.t \
                          32reversi_eval.t \
                          34reversi_iterative.t \
                          35reversi_time.t

ptests                  = $(srcdir)/21nim_trace.t \
                          $(srcdir)/30reversi_move.t \
                          $(srcdir)/33reversi_trace.t

phelpers                = 21nim_trace \
                          30reversi_move

check_PROGRAMS          = $(ctests) $(phelpers)
CFLAGS                  = -I$(top_srcdir) -W -Wall
LDFLAGS                 = -ltap
LDADD                   = $(top_builddir)/libggtl.la

# c tests
09defaults_t_SOURCES          = 09defaults.c
10cache_t_SOURCES             = 10cache.c
11basic_t_SOURCES             = 11basic.c
12sortmoves_t_SOURCES         = 12sortmoves.c
20nim_ai_t_SOURCES            = 20nim_ai.c
31reversi_get_moves_t_SOURCES = 31reversi_get_moves.c
32reversi_eval_t_SOURCES      = 32reversi_eval.c
34reversi_iterative_t_SOURCES = 34reversi_iterative.c
35reversi_time_t_SOURCES      = 35reversi_time.c

# helpers
21nim_trace_SOURCES           = 21nim_trace.c
30reversi_move_SOURCES        = 30reversi_move.c

if HAVE_PROVE
check-local: 
	prove -s $(ctests) $(ptests)
else
TESTS = $(ctests)
check-local:
	@echo "prove not installed; check test output manually"
	for f in $(ptests) ; do perl $$f ; done
	@echo "prove not installed; check test output manually"
endif

EXTRA_DIST = $(ptests)
