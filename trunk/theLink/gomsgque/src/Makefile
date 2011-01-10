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

include ../MakeGoMsgque.inc
include $(GOROOT)/src/Make.inc

TARG=gomsgque
CGOFILES=MqS.go MqSException.go MqBufferS.go MqBinary.go send.go read.go link.go config.go \
	  service.go slave.go factory.go
#CGO_OFILES=$(abs_top_builddir)/theLink/libmsgque/.libs/libtmp.a gomsgque.o
#CGO_OFILES=$(wildcard $(abs_top_builddir)/theLink/libmsgque/.libs/libtmp*.o) gomsgque.o
CGO_OFILES=gomsgque.o

CGO_CFLAGS=-I . -I $(abs_top_srcdir)/theLink/libmsgque -DMQ_IGNORE_EXTERN
CGO_LDFLAGS=-lpthread $(abs_top_builddir)/theLink/libmsgque/.libs/libtmp.a
#CGO_DEPS=$(abs_top_builddir)/theLink/libmsgque/.libs/libtmp.a

include $(GOROOT)/src/Make.pkg

#gomsgque.o: gomsgque.c gomsgque.h _cgo_export.h
#	gcc $(_CGO_CFLAGS_$(GOARCH)) -o gomsgque.o -c -fPIC $(CFLAGS) $(CGO_CFLAGS) gomsgque.c


#goinstall: install
#	test ! -d "$(DESTDIR)$(pkglibdir)" && mkdir -p "$(DESTDIR)$(pkglibdir)"
#	cp $(CGOTARG).so "$(DESTDIR)$(pkglibdir)"
#	cp _obj/$(TARG).a "$(DESTDIR)$(pkglibdir)"
#
#gouninstall:
#	rm "$(DESTDIR)$(pkglibdir)/$(CGOTARG).so"
#	rm "$(DESTDIR)$(pkglibdir)/$(TARG).a"

#%: install %.go
#	$(GC) $*.go
#	$(LD) -o $@ $*.$O
