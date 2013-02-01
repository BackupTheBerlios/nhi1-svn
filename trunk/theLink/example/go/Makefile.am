#+
#:  \file       theLink/example/go/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

##
## ==========================================================================
## GO
##
  
if USE_GO

#SUFFIXES = .go .bin $(EXEEXT)
#
#dummy:	all
#
#.go.bin:
#	$(GC) -I $(abs_top_builddir)/theLink/gomsgque/src/_obj -o $@ $<
#
#.bin$(EXEEXT):
#	$(LD) -L $(abs_top_builddir)/theLink/gomsgque/src/_obj -o $@ $<
#
#GOMSGQUE = $(abs_top_builddir)/theLink/gomsgque/src/_obj/gomsgque.a
#
### all executables depend on our new created "gomsgque.a" library
#
#BUILT_SOURCES = server$(EXEEXT)
#server$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += MyClient$(EXEEXT) MyServer$(EXEEXT)
#MyClient$(EXEEXT): $(GOMSGQUE)
#MyServer$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += Filter3$(EXEEXT)
#Filter3$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += mulclient$(EXEEXT)
#mulclient$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += mulserver$(EXEEXT)
#mulserver$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += manfilter$(EXEEXT)
#manfilter$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += testserver$(EXEEXT)
#testserver$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += testclient$(EXEEXT)
#testclient$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += Filter1$(EXEEXT)
#Filter1$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += Filter2$(EXEEXT)
#Filter2$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += Filter4$(EXEEXT)
#Filter4$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += Filter5$(EXEEXT)
#Filter5$(EXEEXT): $(GOMSGQUE)
#
#BUILT_SOURCES += Filter6$(EXEEXT)
#Filter6$(EXEEXT): $(GOMSGQUE)

endif

##
## ==========================================================================
## Main
##

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = $(srcdir)/*.go
