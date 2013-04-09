#+
#:  \file       theLink/example/vb/Makefile.am
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
## VisualBasic -> compile ONLY on windows
##

SUFFIXES = *.exe *.cs *.dll


csmsgque.dll: ../../csmsgque/csmsgque.dll
	cp $< .

%.exe: %.vb csmsgque.dll
if WIN32
	$(VBCOMP) $@ "-reference:csmsgque.dll -target:exe" '$(shell $(CYGPATH_W) "$<")'
else
	$(VBCOMP) -nologo $(CSHARP_DEBUG) -reference:csmsgque.dll -out:$@ -target:exe $<
endif

all-local: $(patsubst $(srcdir)/%.vb, $(builddir)/%.exe, $(wildcard $(srcdir)/*.vb))

##
## ==========================================================================
## Main
##

MAINTAINERCLEANFILES = *.exe *.mdb *.dll
EXTRA_DIST = $(srcdir)/*.vb

