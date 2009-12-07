#+
#§  \file       theBrain/tests/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

TESTS_ENVIRONMENT = $(TCLSH)
TESTS = $(srcdir)/all.tcl

EXTRA_DIST = $(srcdir)/*.tcl $(srcdir)/*.test $(srcdir)/*.bash $(srcdir)/*.env

if HAS_STATIC
LIBMSGQUE = ../../theLink/libmsgque/*.o
else
LIBMSGQUE = ../../theLink/libmsgque/*.lo
endif

noinst_PROGRAMS = client$(EXEEXT)

client_SOURCES = client.c stat.c stat.h
client_CFLAGS = -I$(srcdir)/../../theLink/libmsgque -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES
client_LDADD = -lm $(LIBMSGQUE)


