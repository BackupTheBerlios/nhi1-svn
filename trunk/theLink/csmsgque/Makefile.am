#+
#:  \file       theLink/csmsgque/Makefile.am
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
## C# setup
##

SUFFIXES = .cs .dll .exe

CSHARP_FILES = $(srcdir)/MqBufferS.cs $(srcdir)/MqDumpS.cs $(srcdir)/MqS.cs $(srcdir)/MqFactoryS.cs \
	$(srcdir)/link.cs $(srcdir)/pointer.cs $(srcdir)/send.cs $(srcdir)/slave.cs $(srcdir)/context.cs \
	  $(srcdir)/error.cs $(srcdir)/misc.cs $(srcdir)/read.cs $(srcdir)/service.cs $(srcdir)/config.cs \
	    $(srcdir)/storage.cs

csmsgque.dll: $(CSHARP_FILES)
if WIN32
	$(CSCOMP) $@ -target:library '$(shell $(CYGPATH_W) $(CSHARP_FILES))'
else
	$(CSCOMP) $(CSHARP_OPT) -nologo -define:$(MQ_IS) $(CSHARP_DEBUG) -out:$@ -target:library $(CSHARP_FILES)
endif

csharpdir = $(pkglibdir)
csharp_DATA = csmsgque.dll

BUILT_SOURCES = csmsgque.dll

EXTRA_DIST = $(CSHARP_FILES)

CLEANFILES = *.exe *.mdb *.dll

