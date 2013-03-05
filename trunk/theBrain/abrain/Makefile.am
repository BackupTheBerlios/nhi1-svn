#+
#:  \file       theBrain/abrain/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

bin_PROGRAMS = abrain$(EXEEXT)

CLEANFILES =

abrain_SOURCES = abrain.c 
abrain_CFLAGS = -static -I$(srcdir)/../../theLink/libmsgque -I$(srcdir)/../../theLink/sqlite \
		      -DMQ_IGNORE_EXTERN

abrain_LDADD = ../../theLink/libmsgque/libtmps.la

