#+
#§  \file       theBrain/tests/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

TESTS = $(srcdir)/all.tcl $(srcdir)/util_test.sh

EXTRA_DIST = $(srcdir)/*.tcl $(srcdir)/*.test $(srcdir)/*.bash $(srcdir)/*.env

noinst_PROGRAMS = client

client_SOURCES = client.c stat.c stat.h
client_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES
client_LDADD = -lm ../../theLink/libmsgque/libtmp.la

## test data

check_PROGRAMS = tcamgr tcutest tcumttest tcucodec

tcamgr_SOURCES = tcamgr.c
tcamgr_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcamgr_LDADD = ../libbrain/libbrain.la

tcutest_SOURCES = tcutest.c
tcutest_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcutest_LDADD = ../libbrain/libbrain.la

tcumttest_SOURCES = tcumttest.c
tcumttest_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcumttest_LDADD = ../libbrain/libbrain.la

tcucodec_SOURCES = tcucodec.c
tcucodec_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS) -D_TC_PREFIX="\".\"" -D_TC_INCLUDEDIR="\".\"" \
		    -D_TC_LIBDIR="\".\"" -D_TC_BINDIR="\".\"" -D_TC_LIBEXECDIR="\".\"" -D_TC_APPINC="\".\"" \
		      -D_TC_APPLIBS="\".\""
tcucodec_LDADD = ../libbrain/libbrain.la

