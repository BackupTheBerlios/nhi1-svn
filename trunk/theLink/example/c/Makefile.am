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

noinst_PROGRAMS = mulclient$(EXEEXT) mulserver$(EXEEXT) manfilter$(EXEEXT) \
		    testclient$(EXEEXT) testserver$(EXEEXT) \
		      MyClient$(EXEEXT) MyServer$(EXEEXT) \
			Filter1$(EXEEXT) Filter2$(EXEEXT)

mulclient_SOURCES = mulclient.c
mulclient_LDADD = -lm ../../libmsgque/*.lo

mulserver_SOURCES = mulserver.c
mulserver_LDADD = -lm ../../libmsgque/*.lo

manfilter_SOURCES = manfilter.c
manfilter_LDADD = -lm ../../libmsgque/*.lo

testclient_SOURCES = testclient.c
testclient_LDADD = -lm ../../libmsgque/*.lo

testserver_SOURCES = testserver.c
testserver_LDADD = -lm ../../libmsgque/*.lo

MyClient_SOURCES = MyClient.c
MyClient_LDADD = -lm ../../libmsgque/*.lo

MyServer_SOURCES = MyServer.c
MyServer_LDADD = -lm ../../libmsgque/*.lo

Filter1_SOURCES = Filter1.c
Filter1_LDADD = -lm ../../libmsgque/*.lo

Filter2_SOURCES = Filter2.c
Filter2_LDADD = -lm ../../libmsgque/*.lo

INCLUDES = -I$(srcdir)/../../libmsgque

