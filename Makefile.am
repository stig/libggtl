if HAVE_LIBTAP
SUBDIRS = . t
else
SUBDIRS = .
check-local:
	@echo "****************************************"
	@echo "libtap not installed; not running tests!"
	@echo "****************************************"
endif

lib_LTLIBRARIES         = libggtl.la 

nobase_include_HEADERS  = ggtl/core.h \
                          ggtl/nim.h \
                          ggtl/reversi.h

libggtl_la_CFLAGS       = -W -Wall
libggtl_la_LDFLAGS      = -no-undefined \
                          -release $(PACKAGE_VERSION)

libggtl_la_SOURCES      = $(nobase_include_HEADERS) \
                          ggtl.c \
                          ggtl-nim.c \
                          ggtl-reversi.c

bin_PROGRAMS            = reversi-demo
reversi_demo_SOURCES    = reversi-demo.c
reversi_demo_LDADD      = libggtl.la



# Documentation
man_MANS                = ggtl.3 \
                          ggtl-nim.3 \
                          ggtl-reversi.3 \
                          reversi-demo.3


%.3: %.c
	pod2man -r "$(PACKAGE_STRING)" -s 3 -c "GGTL Reference" -n GGTL $< > $@


# Distribution

EXTRA_DIST          = README \
                      ChangeLog

CLEANFILES          = $(man_MANS) 

