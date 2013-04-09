#+
#:  \file       theLink/example/csharp/Makefile.am
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
## C#
##

csmsgque.dll: ../../csmsgque/csmsgque.dll
	cp $< .

%.exe: %.cs csmsgque.dll
if WIN32
	$(CSCOMP) $@ "-reference:csmsgque.dll -target:exe" '$(shell $(CYGPATH_W) "$<")'
else
	$(CSCOMP) -nologo $(CSHARP_DEBUG) -reference:csmsgque.dll -out:$@ -target:exe $<
endif

all-local: $(patsubst $(srcdir)/%.cs, $(builddir)/%.exe, $(wildcard $(srcdir)/*.cs))


##
## ==========================================================================
## Main
##

CLEANFILES = *.exe *.mdb
EXTRA_DIST = $(srcdir)/*.cs

