ctests                  = ai.t \
                          basic.t

ptests                  = $(srcdir)/trace.t
phelpers                = trace 

check_PROGRAMS          = $(ctests) $(phelpers)
CFLAGS                  = -I$(top_srcdir) -W -Wall -Werror
LDFLAGS                 = -ltap
LDADD                   = $(top_builddir)/libggtl.la

# c tests
ai_t_SOURCES            = ai.c
basic_t_SOURCES         = basic.c

# helpers
trace_SOURCES           = trace.c

EXTRA_DIST = $(ptests)
