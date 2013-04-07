#+
#:  \file       theBrain/atrans/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

bin_PROGRAMS = atrans

atrans_SOURCES = atrans.cc
atrans_CPPFLAGS = -I$(srcdir)/../../theLink/ccmsgque -I$(srcdir)/../../theLink/libmsgque -DMQ_IGNORE_EXTERN
atrans_CFLAGS = -static
atrans_LDFLAGS = -static
atrans_LDADD = ../../theLink/ccmsgque/libccmsgque.la

