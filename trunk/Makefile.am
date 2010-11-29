#+
#§  \file       Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

SHELL = /bin/bash

AUTOMAKE_OPTIONS = check-news 

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = theLink win performance bin
if USE_BRAIN
SUBDIRS += theBrain
endif
if USE_GUARD
SUBDIRS += theGuard
endif
mann_MANS =  $(srcdir)/man/mann/*.n

CTAGSFLAGS = --c-kinds=+p

EXTRA_DIST = $(mann_MANS)

install-data-local:
	echo $$(pwd)
	rm -fr $(DESTDIR)$(datarootdir)/html/theLink
	mkdir -p $(DESTDIR)$(datarootdir)/html
	cp -pr $(srcdir)/html/theLink $(DESTDIR)$(datarootdir)/html/theLink

uninstall-local:
	chmod -R u+rwx $(DESTDIR)$(datarootdir)/html/theLink
	rm -fr $(DESTDIR)$(datarootdir)/html/theLink

dist-hook:
	rm -fr $(distdir)/html
	cp -pr $(srcdir)/html $(distdir)

DISTCHECK_CONFIGURE_FLAGS = --enable-static --enable-tcl --enable-cxx \
      --enable-python --enable-perl --enable-guard --enable-brain \
	--enable-csharp --enable-java --enable-ruby --enable-php \
	  --enable-go --enable-threads

#DISTCHECK_CONFIGURE_FLAGS = --enable-static --enable-tcl \
#	--enable-go --enable-threads

# Darwin needs the DYLD_LIBRARY_PATH for "make check"
# BSD make does !not! support the 'export' command
#export DYLD_LIBRARY_PATH = @abs_top_builddir@/libmsgque/.libs

install-web:
	@echo -e "\n\n!!!!!  have you run 'make html' first ?  !!!!!\n\n"
	rsync -azv --delete html/* aotto1968@shell.berlios.de:/home/groups/nhi1/htdocs

#install-web:
#	@echo -e "\n\n!!!!!  have you run 'make html' first ?  !!!!!\n\n"
#	rsync -azv --delete html/ aotto1968,libmsgque@web.sourceforge.net:htdocs/

install-files:
	for F in NHI1-$(VERSION)* binary-dist/NHI1-$(VERSION)* ; do \
	  ftp -u ftp://ftp.berlios.de/incoming/$$(basename $$F) $$F; \
	done

distclean-local:
	-rm -rf html

