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
mann_MANS = man/mann/libmsgque.n
if USE_BRAIN
SUBDIRS += theBrain
endif
if USE_CXX
mann_MANS += man/mann/ccmsgque.n
endif
if USE_JAVA
mann_MANS += man/mann/javamsgque.n
endif
if USE_PYTHON
mann_MANS += man/mann/pymsgque.n
endif
if USE_CSHARP
mann_MANS += man/mann/csmsgque.n
endif
if USE_PHP
mann_MANS += man/mann/phpmsgque.n
endif
if USE_PERL
mann_MANS += man/mann/perlmsgque.n
endif
if USE_TCL
mann_MANS += man/mann/tclmsgque.n
endif
if USE_VB
mann_MANS += man/mann/vbmsgque.n
endif

CTAGSFLAGS = --c-kinds=+p

EXTRA_DIST = make_binary_dist.sh $(mann_MANS)

DISTCHECK_CONFIGURE_FLAGS = --enable-tcl

# Darwin needs the DYLD_LIBRARY_PATH for "make check"
# BSD make does !not! support the 'export' command
#export DYLD_LIBRARY_PATH = @abs_top_builddir@/libmsgque/.libs

# install SourceForge (SF) data

install-web:
	@echo -e "\n\n!!!!!  have you run 'make html' first ?  !!!!!\n\n"
	rsync -azv --delete html/* aotto1968@shell.berlios.de:/home/groups/nhi1/htdocs

distclean-local:
	-rm -rf html

