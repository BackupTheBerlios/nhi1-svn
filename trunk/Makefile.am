#+
#§  \file       Makefile.am
#§  \brief      \$Id: Makefile.am 512 2009-11-30 14:52:29Z dev1usr $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 512 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

AUTOMAKE_OPTIONS = check-news 

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = win performance example/c
mann_MANS = man/mann/libmsgque.n
if USE_BRAIN
SUBDIRS += theBrain
endif
if USE_CXX
SUBDIRS += example/cc
mann_MANS += man/mann/ccmsgque.n
endif
if USE_JAVA
SUBDIRS += example/java
mann_MANS += man/mann/javamsgque.n
endif
if USE_PYTHON
SUBDIRS += example/python
mann_MANS += man/mann/pymsgque.n
endif
if USE_CSHARP
SUBDIRS += example/csharp
mann_MANS += man/mann/csmsgque.n
endif
if USE_PHP
SUBDIRS += example/php
mann_MANS += man/mann/phpmsgque.n
endif
if USE_PERL
SUBDIRS += example/perl
mann_MANS += man/mann/perlmsgque.n
endif
if USE_TCL
SUBDIRS += example/tcl
SUBDIRS += tests
mann_MANS += man/mann/tclmsgque.n
endif
if USE_VB
SUBDIRS += example/vb
mann_MANS += man/mann/vbmsgque.n
endif
SUBDIRS += acmds docs

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

