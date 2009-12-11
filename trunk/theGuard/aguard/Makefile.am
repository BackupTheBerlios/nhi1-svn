#+
#§  \file       theGuard/aguard/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

bin_PROGRAMS = aguard

CLEANFILES =

aguard_SOURCES = aguard.c 
aguard_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque \
		      -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES $(BRAIN_CPPFLAGS)

aguard_LDADD = ../../theLink/libmsgque/libtmp.la

