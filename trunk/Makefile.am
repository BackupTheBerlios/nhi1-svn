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

dist-hook:
	mkdir $(distdir)/html
	cp -pr $(srcdir)/html $(distdir)/html

DISTCHECK_CONFIGURE_FLAGS = --enable-tcl --enable-cxx \
      --enable-java --enable-csharp --enable-python --enable-perl \
	--enable-static --enable-guard --enable-brain

#--enable-vb --enable-guard --enable-brain

# Darwin needs the DYLD_LIBRARY_PATH for "make check"
# BSD make does !not! support the 'export' command
#export DYLD_LIBRARY_PATH = @abs_top_builddir@/libmsgque/.libs

#install-web:
#	@echo -e "\n\n!!!!!  have you run 'make html' first ?  !!!!!\n\n"
#	rsync -azv --delete html/* aotto1968@shell.berlios.de:/home/groups/nhi1/htdocs

install-web:
	@echo -e "\n\n!!!!!  have you run 'make html' first ?  !!!!!\n\n"
	rsync -azv --delete html/ aotto1968,libmsgque@web.sourceforge.net:htdocs/

install-files:
	for F in NHI1-$(VERSION)* binary-dist/NHI1-$(VERSION)* ; do \
	  ftp -u ftp://ftp.berlios.de/incoming/$$(basename $$F) $$F; \
	done

distclean-local:
	-rm -rf html

