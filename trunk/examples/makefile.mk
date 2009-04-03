bin_PROGRAMS          = reversi-demo ttt-demo ttt-demo2

reversi_demo_SOURCES  = examples/reversi-demo.c
reversi_demo_LDFLAGS  = -L$(builddir) -lreversi

ttt_demo_SOURCES      = examples/ttt-demo.c
ttt_demo_LDFLAGS      = -L$(builddir) -lggtl

examples/ttt-demo.c: examples/ggtltut.pod
	-mkdir -p $@ && rmdir $@
	sed 's/^=for //' $< > $@

ttt_demo2_SOURCES      = examples/ttt-demo2.c
ttt_demo2_LDFLAGS      = -L$(builddir) -lggtl

examples/ttt-demo2.c: examples/ggtltut2.pod
	-mkdir -p $@ && rmdir $@
	sed 's/^=for //' $< > $@

examples/reversi-demo.c: examples/reversi-demo.pod
	-mkdir -p $@ && rmdir $@
	sed 's/^=for //' $< > $@

man3_MANS            += examples/reversi-demo.man \
                        examples/ggtltut2.man \
                        examples/ggtltut.man

EXTRA_DIST           += examples/reversi-demo.pod \
                        examples/ggtltut2.pod \
                        examples/ggtltut.pod

CLEANFILES           += $(reversi_demo_SOURCES) $(ttt_demo_SOURCES) $(ttt_demo2_SOURCES)
