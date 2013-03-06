#+
#:  \file       theBrain/tests/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

#TESTS = $(srcdir)/all.sh $(srcdir)/util_test.sh $(srcdir)/hdb_test.sh \
#	  $(srcdir)/bdb_test.sh

TESTS = $(srcdir)/all.sh

EXTRA_DIST = $(srcdir)/*.tcl $(srcdir)/*.test $(srcdir)/*.bash $(srcdir)/*.env

noinst_PROGRAMS = client

client_SOURCES = client.c stat.c stat.h
client_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -DMQ_IGNORE_EXTERN
client_LDADD = -lm ../../theLink/libmsgque/libtmp.la

