#+
#§  \file       theLink/gomsgque/Makefile.am
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

#####################################################################################
## GO files

GOMAKEFILE  =	src/Makefile
MAKEARGS    =	$(AM_MAKEFLAGS)

setup:
	-test ! -d src && cp -pr $(srcdir)/src . && chmod -R u+w src && touch src/.distclean

all-local: setup
	-cd src && $(MAKE) $(MAKEARGS)
	cd src && $(MAKE) $(MAKEARGS)

clean-local: setup
	cd src && $(MAKE) $(MAKEARGS) clean

install-exec-local:
	cd src && $(MAKE) $(MAKEARGS) DESTDIR="$(DESTDIR)" pkglibdir="$(pkglibdir)" goinstall

uninstall-local:
	cd src && $(MAKE) $(MAKEARGS) DESTDIR="$(DESTDIR)" pkglibdir="$(pkglibdir)" gouninstall

distclean-local:
	-test -f src/.distclean && rm -fr src

endif

EXTRA_DIST = \
$(srcdir)/src/MqS.go \
$(srcdir)/src/MqSException.go \
$(srcdir)/src/MqBufferS.go \
$(srcdir)/src/MqBinary.go \
$(srcdir)/src/send.go \
$(srcdir)/src/read.go \
$(srcdir)/src/link.go \
$(srcdir)/src/config.go \
$(srcdir)/src/service.go\
$(srcdir)/src/slave.go\
$(srcdir)/src/MqFactoryS.go\
$(srcdir)/src/gomsgque.h \
$(srcdir)/src/gomsgque.c \
$(srcdir)/src/Makefile \
MakeGoMsgque.inc.in

