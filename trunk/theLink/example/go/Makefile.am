#+
#§  \file       theLink/example/go/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

##
## ==========================================================================
## GO
##

BUILT_SOURCES = MyClient$(EXEEXT) MyServer$(EXEEXT) Filter3$(EXEEXT) mulclient$(EXEEXT) \
  mulserver$(EXEEXT) manfilter$(EXEEXT) testserver$(EXEEXT) testclient$(EXEEXT) \
    Filter1$(EXEEXT) Filter2$(EXEEXT)
  

include $(GOROOT)/src/Make.inc

SUFFIXES = .go .bin $(EXEEXT)

dummy:	all

.go.bin:
	$(GC) -o $@ $<

.bin$(EXEEXT):
	$(LD) -o $@ $<

## all executables depend on our new created "gomsgque.a" library
MyClient$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
MyServer$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
Filter1$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
Filter2$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
Filter3$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
mulclient$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
mulserver$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
manfilter$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
testserver$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
testclient$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

##
## ==========================================================================
## Main
##

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = *.go

