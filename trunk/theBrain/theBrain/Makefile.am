#+
#§  \file       theBrain/Makefile.am
#§  \brief      \$Id: Makefile.am 33 2009-12-04 11:36:05Z aotto1968 $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 33 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

bin_PROGRAMS = theBrain

CLEANFILES =

theBrain_SOURCES = theBrain.c 
theBrain_CFLAGS = -I$(srcdir)/../../theLink/libmsgque $(BRAIN_CFLAGS) -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES

if HAS_STATIC
LIBMSGQUE = ../../theLink/libmsgque/*.o
else
LIBMSGQUE = ../../theLink/libmsgque/*.lo
endif

theBrain_LDADD = $(BRAIN_LDADD) $(LIBMSGQUE)

