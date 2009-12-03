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
SUBDIRS += tests
endif

bin_PROGRAMS = brain

CLEANFILES =

brain_SOURCES = brain.c 
#brabrainDD = -lm ../theLink//libmsgque/libmsgque.la
brain_CFLAGS = -I../theLink/libmsgque -I./tokyocabinet -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES
brain_LDADD = -lz -lbz2 -lrt -lm -lc ../theLink/libmsgque/*.lo ./tokyocabinet/libtokyocabinet.a
#brain_LDFLAGS = -static

EXTRA_DIST = brain 

