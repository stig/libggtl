# Variables to be populated by included files
ptests			= 
ctests			=
phelpers		= 

include $(srcdir)/t/nim/makefile.mk
include $(srcdir)/t/reversi/makefile.mk

ctests                 += t/defaults.t \
                          t/cache.t \
                          t/sortmoves.t

ptests                 += t/ttt.t

t_defaults_t_SOURCES    = t/defaults.c
t_defaults_t_LDFLAGS    = -lggtl -ltap -lm

t_cache_t_SOURCES       = t/cache.c
t_cache_t_LDFLAGS       = -lggtl -ltap

t_sortmoves_t_SOURCES   = t/sortmoves.c
t_sortmoves_t_LDFLAGS   = -lggtl -ltap

check_PROGRAMS          = $(ctests) $(phelpers)


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

EXTRA_DIST          += $(ptests)

clean-local:
	rm -rf t/.libs 
	rm -rf t/nim/.libs 
	rm -rf t/reversi/.libs 
