#+
#§  \file       theLink/example/cc/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

##
## ==========================================================================
## performance tools setup
##

noinst_PROGRAMS = Filter1$(EXEEXT) mulserver$(EXEEXT) mulclient$(EXEEXT) manfilter$(EXEEXT) \
      testserver$(EXEEXT) testclient$(EXEEXT) MyClient$(EXEEXT)  MyServer$(EXEEXT) Filter2$(EXEEXT) \
	Filter3$(EXEEXT)

AM_CXXFLAGS = -I$(srcdir)/../../ccmsgque -DMQ_LINK_WITH_LIBRARY_OBJECT_FILES
LDADD = ../../ccmsgque/*.lo

Filter1_SOURCES = Filter1.cc
Filter2_SOURCES = Filter2.cc
Filter3_SOURCES = Filter3.cc
manfilter_SOURCES = manfilter.cc
mulclient_SOURCES = mulclient.cc
mulserver_SOURCES = mulserver.cc
MyClient_SOURCES = MyClient.cc
MyServer_SOURCES = MyServer.cc
testclient_SOURCES = testclient.cc
testserver_SOURCES = testserver.cc

