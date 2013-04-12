#+
#:  \file       theGuard/aguard/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

bin_PROGRAMS = aguard

aguard_SOURCES = aguard.c
nodist_aguard_SOURCES = key.h
aguard_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -DMQ_IGNORE_EXTERN $(BRAIN_CPPFLAGS)
aguard_LDADD = ../../theLink/libmsgque/libmsgque.la

key.h: $(abs_top_srcdir)/sbin/KeyGen.tcl
	$(TCLSH) $(abs_top_srcdir_m)/sbin/KeyGen.tcl > key.h

BUILT_SOURCES=key.h

