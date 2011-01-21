#+
#§  \file       theBrain/abrain/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

bin_PROGRAMS = abrain$(EXEEXT)

CLEANFILES =

abrain_SOURCES = abrain.c 
abrain_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -I$(srcdir)/../libbrain \
		      -DMQ_IGNORE_EXTERN $(BRAIN_CPPFLAGS)

abrain_LDADD = ../libbrain/libtmp.la ../../theLink/libmsgque/libtmp.la

