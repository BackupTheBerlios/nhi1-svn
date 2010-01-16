#+
#§  \file       theBrain/atrans/Makefile.am
#§  \brief      \$Id: Makefile.am 61 2009-12-11 09:35:42Z aotto1968 $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 61 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

bin_PROGRAMS = atrans

CLEANFILES =

atrans_SOURCES = atrans.c 
atrans_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -I$(srcdir)/../libbrain \
		      -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES $(BRAIN_CPPFLAGS)

atrans_LDADD = ../libbrain/libtmp.la ../../theLink/libmsgque/libtmp.la

