#+
#§  \file       theBrain/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

if USE_TCL
SUBDIRS = tests
endif

bin_PROGRAMS = brain

CLEANFILES =

brain_SOURCES = brain.c 
brain_CFLAGS = -I$(srcdir)/../theLink/libmsgque $(BRAIN_CFLAGS) -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES

if HAS_STATIC
LIBMSGQUE = ../theLink/libmsgque/*.o
else
LIBMSGQUE = ../theLink/libmsgque/*.lo
endif

brain_LDADD = $(BRAIN_LDADD) $(LIBMSGQUE)
#brain_LDFLAGS = -static

