#+
#§  \file       theLink/gomsgque/src/Makefile
#§  \brief      \$Id: Makefile.in 316 2010-11-19 18:40:05Z aotto1968 $
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 316 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

include ./MakeGoMsgque.inc
include $(GOROOT)/src/Make.inc

MQOBJ  = $(builddir)/../../libmsgque/.libs/liblocal.a

TARG=gomsgque
CGOFILES=MqS.go MqSException.go MqBufferS.go MqBinary.go send.go read.go link.go config.go \
	  service.go slave.go

CGO_CFLAGS=-I . -I $(srcdir)/../../libmsgque -DMQ_IGNORE_EXTERN
CGO_LDFLAGS= $(MQOBJ) _cgo_export.o gomsgque.o
CGO_DEPS=$(MQOBJ) _cgo_export.o gomsgque.o 

include $(GOROOT)/src/Make.pkg

gomsgque.o: gomsgque.c gomsgque.h _cgo_export.h
	gcc $(_CGO_CFLAGS_$(GOARCH)) -o gomsgque.o -c -fPIC $(CFLAGS) $(CGO_CFLAGS) gomsgque.c

%: install %.go
	$(GC) $*.go
	$(LD) -o $@ $*.$O

all: _obj/gomsgque.a server client
