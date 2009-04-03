SUBDIRS                 = nim reversi .

ctests                  = defaults.t \
			  cache.t \
                          sortmoves.t

ptests                  = 
phelpers                = 

check_PROGRAMS          = $(ctests) $(phelpers)
CFLAGS                  = -I$(top_srcdir) -W -Wall -Werror
LDFLAGS                 = -ltap
LDADD                   = $(top_builddir)/libggtl.la

# c tests
defaults_t_SOURCES      = defaults.c
cache_t_SOURCES         = cache.c
sortmoves_t_SOURCES     = sortmoves.c

if HAVE_PROVE
check-local: 
	@if test $(srcdir) != "." ; then prove -s $(srcdir) $(srcdir)/nim $(srcdir)/reversi ; else true ; fi
	prove -s . nim reversi 

else
TESTS = $(ctests)
check-local:
	@echo "prove not installed; check test output manually"
	for f in $(ptests) ; do perl $$f ; done
	@echo "prove not installed; check test output manually"
endif

EXTRA_DIST = $(ptests)
