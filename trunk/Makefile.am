#+
#:  \file       Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

SHELL = /bin/bash

AUTOMAKE_OPTIONS = check-news 

PATH = $(path)
LD_LIBRARY_PATH = $(ld_library_path)
export PATH
export LD_LIBRARY_PATH

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = bin sbin etc theLink performance tests lxc
if USE_BRAIN
SUBDIRS += theBrain
endif
if USE_GUARD
SUBDIRS += theGuard
endif

CTAGSFLAGS = --c-kinds=+p
EXTRA_DIST = README_windows

install-data-local:
	echo $$(pwd)
	rm -fr $(DESTDIR)$(datarootdir)/html/theLink
	mkdir -p $(DESTDIR)$(datarootdir)/html
	cp -pr $(srcdir)/html/theLink $(DESTDIR)$(datarootdir)/html

uninstall-local:
	chmod -R u+rwx $(DESTDIR)$(datarootdir)/html/theLink
	rm -fr $(DESTDIR)$(datarootdir)/html/theLink

dist-hook:
	rm -fr $(distdir)/html
	cp -pr $(srcdir)/html $(distdir)

AM_DISTCHECK_CONFIGURE_FLAGS = --with-tool-root=$(NHI1_TOOL_ROOT) --enable-cache --with-tcl

# Darwin needs the DYLD_LIBRARY_PATH for "make check"
# BSD make does !not! support the 'export' command
#export DYLD_LIBRARY_PATH = @abs_top_builddir@/libmsgque/.libs

install-web:
	@echo -e "\n\n!!!!!  have you run 'make html' first ?  !!!!!\n\n"
	rsync -azv --delete $(srcdir)/html/* aotto1968@shell.berlios.de:/home/groups/nhi1/htdocs

#install-web:
#	@echo -e "\n\n!!!!!  have you run 'make html' first ?  !!!!!\n\n"
#	rsync -azv --delete html/ aotto1968,libmsgque@web.sourceforge.net:htdocs/

install-src:
	ftp -u ftp://ftp.berlios.de/incoming/ NHI1-*.tar.bz2 || true

install-files:
	for F in NHI1-$(VERSION)* binary-dist/NHI1-$(VERSION)* ; do \
	  ftp -u ftp://ftp.berlios.de/incoming/$$(basename $$F) $$F || true; \
	done

#distclean-local:
#	rm -rf html

all-local: $(patsubst $(srcdir)/%.in, %, $(wildcard $(srcdir)/*.in))

