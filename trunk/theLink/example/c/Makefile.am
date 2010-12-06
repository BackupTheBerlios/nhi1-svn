#+
#§  \file       theLink/example/c/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

noinst_PROGRAMS = mulclient mulserver manfilter \
		    testclient testserver \
		      MyClient MyServer \
			Filter1 Filter2 Filter3 Filter4

AM_CFLAGS = -I$(srcdir)/../../libmsgque -DMQ_IGNORE_EXTERN 
LDADD = -lm ../../libmsgque/libtmp.la

mulclient_SOURCES = mulclient.c
mulserver_SOURCES = mulserver.c
manfilter_SOURCES = manfilter.c
testclient_SOURCES = testclient.c
testserver_SOURCES = testserver.c
MyClient_SOURCES = MyClient.c
MyServer_SOURCES = MyServer.c
Filter1_SOURCES = Filter1.c
Filter2_SOURCES = Filter2.c
Filter3_SOURCES = Filter3.c
Filter4_SOURCES = Filter4.c

