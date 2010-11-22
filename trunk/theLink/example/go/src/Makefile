#+
#§  \file       theLink/example/go/src/Makefile
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

include ./MakeGoMsgque.inc
include $(GOROOT)/src/Make.inc

GOMAIN = 

%: Makefile %.go
	$(GC) $*.go
	$(LD) -o $@ $*.$O
	cp $@ ..


#all =	Filter1.go  Filter3.go  manfilter.go  mulserver.go  MyServer.go    testserver.go  \
#	Filter2.go  Filter4.go  mulclient.go  MyClient.go   testclient.go

all:	MyClient MyServer

