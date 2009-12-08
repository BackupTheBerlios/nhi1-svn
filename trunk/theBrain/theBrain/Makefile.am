#+
#§  \file       theBrain/theBrain/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

bin_PROGRAMS = theBrain

CLEANFILES =

theBrain_SOURCES = theBrain.c 
theBrain_CFLAGS = -I$(srcdir)/../../theLink/libmsgque -I$(srcdir)/../libbrain -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES

theBrain_LDADD = ../libbrain/libbrain.la ../../theLink/libmsgque/libtmp.la

