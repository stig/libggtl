lib_LTLIBRARIES         = libggtl.la libnim.la libreversi.la

libggtl_la_SOURCES      = ggtl/ggtl.c ggtl/ggtlai.c ggtl/private.h
libggtl_la_LDFLAGS      = -no-undefined -version-info 0:0:0

libnim_la_SOURCES       = ggtl/nim.c
libnim_la_LIBADD        = libggtl.la
libnim_la_LDFLAGS       = -no-undefined -version-info 0:0:0

libreversi_la_SOURCES   = ggtl/reversi.c
libreversi_la_LIBADD    = libggtl.la
libreversi_la_LDFLAGS   = -no-undefined -version-info 0:0:0

nobase_include_HEADERS  = ggtl/core.h \
                          ggtl/nim.h \
                          ggtl/reversi.h

man3_MANS              += ggtl/ggtl.3 \
                          ggtl/ggtlai.3 \
                          ggtl/ggtlcb.man \
                          ggtl/nim.3 \
                          ggtl/reversi.3

EXTRA_DIST             += ggtl/ggtlcb.pod
