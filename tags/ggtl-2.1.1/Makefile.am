# Always be strict about errors and warnings
CFLAGS			= -W -Wall -Werror

# Variables to be populated by included files
man3_MANS		= 
EXTRA_DIST		=
CLEANFILES		= 

include $(srcdir)/ggtl/makefile.mk
include $(srcdir)/examples/makefile.mk

if HAVE_LIBTAP
include $(srcdir)/t/makefile.mk
else
check-local:
	@echo "****************************************"
	@echo "libtap not installed; not running tests!"
	@echo "****************************************"
endif

docs: $(man3_MANS)

%.man: %.pod
	-mkdir -p $@ && rmdir $@
	pod2man -r "$(PACKAGE_STRING)" -s 3 -c "GGTL Documentation" -n GGTL $< $@

%.3: %.c
	-mkdir -p $@ && rmdir $@
	pod2man -r "$(PACKAGE_STRING)" -s 3 -c "GGTL Reference" -n GGTL $< $@

FWOBJS		= ggtl.o ggtlai.o reversi.o
FWHDRS		= ggtl/core.h ggtl/reversi.h
FWROOT		= $(PACKAGE_NAME).framework
FWDIR		= $(FWROOT)/Versions/$(PACKAGE_VERSION)
FWHDRDIR	= $(FWDIR)/Headers
FWLIB		= $(FWDIR)/$(PACKAGE_NAME)

fw: $(libreversi_la_OBJECTS)
	-rm -rf $(FWROOT) && mkdir -p $(FWHDRDIR)
	cp $(FWHDRS) $(FWHDRDIR)
	gcc -dynamiclib -o $(FWLIB) $(FWOBJS) \
		-install_name "@executable_path/../Frameworks/$(FWLIB)" \
		-framework sl
	cd $(FWDIR)/.. && ln -sf $(PACKAGE_VERSION) Current 
	cd $(FWROOT) && ln -sf Versions/Current/$(PACKAGE_NAME) .


EXTRA_DIST          += README ChangeLog

CLEANFILES          += $(man3_MANS)
