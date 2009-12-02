#+
#§  \file       example/c/Makefile.am
#§  \brief      \$Id: Makefile.am 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

noinst_PROGRAMS = mulclient$(EXEEXT) mulserver$(EXEEXT) manfilter$(EXEEXT) \
		    testclient$(EXEEXT) testserver$(EXEEXT) \
		      MyClient$(EXEEXT) MyServer$(EXEEXT) \
			Filter1$(EXEEXT) Filter2$(EXEEXT)

mulclient_SOURCES = mulclient.c
mulclient_LDADD = -lm ../../theLink/libmsgque/*.lo

mulserver_SOURCES = mulserver.c
mulserver_LDADD = -lm ../../theLink/libmsgque/*.lo

manfilter_SOURCES = manfilter.c
manfilter_LDADD = -lm ../../theLink/libmsgque/*.lo

testclient_SOURCES = testclient.c
testclient_LDADD = -lm ../../theLink/libmsgque/*.lo

testserver_SOURCES = testserver.c
testserver_LDADD = -lm ../../theLink/libmsgque/*.lo

MyClient_SOURCES = MyClient.c
MyClient_LDADD = -lm ../../theLink/libmsgque/*.lo

MyServer_SOURCES = MyServer.c
MyServer_LDADD = -lm ../../theLink/libmsgque/*.lo

Filter1_SOURCES = Filter1.c
Filter1_LDADD = -lm ../../theLink/libmsgque/*.lo

Filter2_SOURCES = Filter2.c
Filter2_LDADD = -lm ../../theLink/libmsgque/*.lo

INCLUDES = -I$(srcdir)/../../theLink/libmsgque

