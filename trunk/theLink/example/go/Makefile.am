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

BUILT_SOURCES = MyClient$(EXEEXT) MyServer$(EXEEXT)

include $(GOROOT)/src/Make.inc

SUFFIXES = .go .bin $(EXEEXT)

dummy:	all

.go.bin:
	$(GC) -o $@ $<

.bin$(EXEEXT):
	$(LD) -o $@ $<

##
## ==========================================================================
## Main
##

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = *.go

