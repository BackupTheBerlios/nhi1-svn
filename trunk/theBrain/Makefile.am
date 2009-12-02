#+
#§  \file       libbrain/Makefile.am
#§  \brief      \$Id: Makefile.am 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

bin_PROGRAMS = brain
EXTRA_DIST = brain

CLEANFILES =

brain_SOURCES = brain.c 
#brabrainDD = -lm ../libmsgque/libmsgque.la
brain_CFLAGS = -I../libmsgque -I../tokyocabinet -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES
brain_LDADD = -lz -lbz2 -lrt -lm -lc ../libmsgque/*.lo ../tokyocabinet/libtokyocabinet.a
#brain_LDFLAGS = -static

