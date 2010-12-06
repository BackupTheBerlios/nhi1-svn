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

#TESTS = $(srcdir)/all.sh $(srcdir)/util_test.sh $(srcdir)/hdb_test.sh \
#	  $(srcdir)/bdb_test.sh

TESTS = $(srcdir)/all.sh

EXTRA_DIST = $(srcdir)/*.tcl $(srcdir)/*.test $(srcdir)/*.bash $(srcdir)/*.env

noinst_PROGRAMS = client

client_SOURCES = client.c stat.c stat.h
client_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -DMQ_IGNORE_EXTERN
client_LDADD = -lm ../../theLink/libmsgque/liblocal.la

## test data

check_PROGRAMS = tcamgr tcutest tcumttest tcucodec \
		  tchmgr tchtest tchmttest \
		    tcbmgr tcbtest tcbmttest

## util progs
tcamgr_SOURCES = tcamgr.c
tcamgr_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcamgr_LDADD = ../libbrain/libtmp.la

tcutest_SOURCES = tcutest.c
tcutest_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcutest_LDADD = ../libbrain/libtmp.la

tcumttest_SOURCES = tcumttest.c
tcumttest_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcumttest_LDADD = ../libbrain/libtmp.la

tcucodec_SOURCES = tcucodec.c
tcucodec_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS) -D_TC_PREFIX="\".\"" -D_TC_INCLUDEDIR="\".\"" \
		    -D_TC_LIBDIR="\".\"" -D_TC_BINDIR="\".\"" -D_TC_LIBEXECDIR="\".\"" -D_TC_APPINC="\".\"" \
		      -D_TC_APPLIBS="\".\""
tcucodec_LDADD = ../libbrain/libtmp.la

## hdb progs
tchmgr_SOURCES = tchmgr.c
tchmgr_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tchmgr_LDADD = ../libbrain/libtmp.la

tchtest_SOURCES = tchtest.c
tchtest_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tchtest_LDADD = ../libbrain/libtmp.la

tchmttest_SOURCES = tchmttest.c
tchmttest_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tchmttest_LDADD = ../libbrain/libtmp.la

## bdb progs
tcbmgr_SOURCES = tcbmgr.c
tcbmgr_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcbmgr_LDADD = ../libbrain/libtmp.la

tcbtest_SOURCES = tcbtest.c
tcbtest_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcbtest_LDADD = ../libbrain/libtmp.la

tcbmttest_SOURCES = tcbmttest.c
tcbmttest_CPPFLAGS = -I$(srcdir)/../libbrain $(BRAIN_CPPFLAGS)
tcbmttest_LDADD = ../libbrain/libtmp.la
