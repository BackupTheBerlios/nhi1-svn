#+
#§  \file       theLink/example/go/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

if USE_GO

GOMAKEFILE  =	src/Makefile
MAKEARGS    =	$(AM_MAKEFLAGS)

all-local:
	cd src && $(MAKE) $(MAKEARGS)

clean-local: $(GOMAKEFILE)
	cd src && $(MAKE) $(MAKEARGS) clean

install-exec-local:
	cd src && $(MAKE) $(MAKEARGS) DESTDIR="$(DESTDIR)" install

distclean-local:
	-test -f src/.distclean && rm -fr src

endif

EXTRA_DIST =

