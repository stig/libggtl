bin_PROGRAMS          = reversi-demo ttt-demo 

reversi_demo_SOURCES  = examples/reversi-demo.c
reversi_demo_LDFLAGS  = -L$(builddir) -lreversi

ttt_demo_SOURCES      = examples/ttt-demo.c
ttt_demo_LDFLAGS      = -L$(builddir) -lggtl

examples/ttt-demo.c: examples/ggtltut.pod
	-mkdir -p $@ && rmdir $@
	sed 's/^=for //' $< > $@

examples/reversi-demo.c: examples/reversi-demo.pod
	-mkdir -p $@ && rmdir $@
	sed 's/^=for //' $< > $@

man3_MANS            += examples/reversi-demo.man \
                        examples/ggtltut.man

EXTRA_DIST           += examples/reversi-demo.pod \
                        examples/ggtltut.pod

CLEANFILES           += $(reversi_demo_SOURCES) $(ttt_demo_SOURCES) 
