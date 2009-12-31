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

mulclient_SOURCES = mulclient.c
mulclient_LDADD = -lm ../../libmsgque/libtmp.la

mulserver_SOURCES = mulserver.c
mulserver_LDADD = -lm ../../libmsgque/libtmp.la

manfilter_SOURCES = manfilter.c
manfilter_LDADD = -lm ../../libmsgque/libtmp.la

testclient_SOURCES = testclient.c
testclient_LDADD = -lm ../../libmsgque/libtmp.la

testserver_SOURCES = testserver.c
testserver_LDADD = -lm ../../libmsgque/libtmp.la

MyClient_SOURCES = MyClient.c
MyClient_LDADD = -lm ../../libmsgque/libtmp.la

MyServer_SOURCES = MyServer.c
MyServer_LDADD = -lm ../../libmsgque/libtmp.la

Filter1_SOURCES = Filter1.c
Filter1_LDADD = -lm ../../libmsgque/libtmp.la

Filter2_SOURCES = Filter2.c
Filter2_LDADD = -lm ../../libmsgque/libtmp.la

Filter3_SOURCES = Filter3.c
Filter3_LDADD = -lm ../../libmsgque/libtmp.la

Filter4_SOURCES = Filter4.c
Filter4_LDADD = -lm ../../libmsgque/libtmp.la

INCLUDES = -I$(srcdir)/../../libmsgque

