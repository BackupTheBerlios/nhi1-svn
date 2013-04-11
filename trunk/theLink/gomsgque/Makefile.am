#+
#:  \file       theLink/gomsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

EXTRA_DIST = $(srcdir)/src/gomsgque/*.go $(srcdir)/src/gomsgque/*.h $(srcdir)/src/gomsgque/*.c

#####################################################################################
## GO files

GOPATH = $(abs_builddir)
export GOPATH

GOBIN = $(abs_builddir)
export GOBIN

#####################################################################################
## GO files

$(abs_builddir)/src/gomsgque:
	mkdir $@

$(abs_builddir)/src/gomsgque/%: $(abs_srcdir)/src/gomsgque/% | $(abs_builddir)/src/gomsgque
	cp $< $@

all-local: $(subst $(abs_srcdir), $(abs_builddir), $(wildcard $(abs_srcdir)/src/gomsgque/*))
	$(GO) install gomsgque

clean-local: 
	$(GO) clean
	rm -fr pkg

distclean-local:
	if test "$(abs_builddir)" != "$(abs_srcdir)" ; then rm -fr src; else true; fi

