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
  
include $(GOROOT)/src/Make.inc

SUFFIXES = .go .bin $(EXEEXT)

dummy:	all

.go.bin:
	$(GC) -o $@ $<

.bin$(EXEEXT):
	$(LD) -o $@ $<

## all executables depend on our new created "gomsgque.a" library

BUILT_SOURCES = server$(EXEEXT)
server$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += MyClient$(EXEEXT) MyServer$(EXEEXT)
MyClient$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a
MyServer$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += Filter3$(EXEEXT)
Filter3$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += mulclient$(EXEEXT)
mulclient$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += mulserver$(EXEEXT)
mulserver$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += manfilter$(EXEEXT)
manfilter$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += testserver$(EXEEXT)
testserver$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += testclient$(EXEEXT)
testclient$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += Filter1$(EXEEXT)
Filter1$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += Filter2$(EXEEXT)
Filter2$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += Filter4$(EXEEXT)
Filter4$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

BUILT_SOURCES += Filter5$(EXEEXT)
Filter5$(EXEEXT): $(QUOTED_GOROOT)/pkg/$(GOOS)_$(GOARCH)/gomsgque.a

##
## ==========================================================================
## Main
##

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = $(srcdir)/*.go
