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

SUBDIRS = theLink win performance
if USE_BRAIN
SUBDIRS += theBrain
endif
mann_MANS =   man/mann/libmsgque.n \
	      man/mann/ccmsgque.n \
	      man/mann/javamsgque.n \
	      man/mann/pymsgque.n \
	      man/mann/csmsgque.n \
	      man/mann/perlmsgque.n \
	      man/mann/tclmsgque.n \
	      man/mann/vbmsgque.n

CTAGSFLAGS = --c-kinds=+p

EXTRA_DIST = make_binary_dist.sh $(mann_MANS)

DISTCHECK_CONFIGURE_FLAGS = --enable-tcl --enable-brain --enable-cxx \
      --enable-java --enable-csharp --enable-python --enable-perl --enable-vb \
	--enable-static

# Darwin needs the DYLD_LIBRARY_PATH for "make check"
# BSD make does !not! support the 'export' command
#export DYLD_LIBRARY_PATH = @abs_top_builddir@/libmsgque/.libs

install-web:
	@echo -e "\n\n!!!!!  have you run 'make html' first ?  !!!!!\n\n"
	rsync -azv --delete html/* aotto1968@shell.berlios.de:/home/groups/nhi1/htdocs

install-files:
	ftp -u ftp://ftp.berlios.de/incoming/NHI1-$(VERSION).tar.gz NHI1-$(VERSION).tar.gz

distclean-local:
	-rm -rf html

