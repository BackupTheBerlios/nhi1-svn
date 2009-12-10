#+
#§  \file       theBrain/theBrain/Makefile.am
#§  \brief      \$Id: Makefile.am 49 2009-12-09 12:21:17Z aotto1968 $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 49 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

bin_PROGRAMS = abrain

CLEANFILES =

abrain_SOURCES = abrain.c 
abrain_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -I$(srcdir)/../libbrain \
		      -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES $(BRAIN_CPPFLAGS)

abrain_LDADD = ../libbrain/libtmp.la ../../theLink/libmsgque/libtmp.la

