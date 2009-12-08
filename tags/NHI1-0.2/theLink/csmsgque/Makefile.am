#+
#§  \file       theLink/csmsgque/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§


#####################################################################################
## VPATH setup

.vpath_hook:
	$(VPATH_HOOK)

##
## ==========================================================================
## C# setup
##

SUFFIXES = .cs .dll .exe

if USE_CSHARP
csmsgque.dll: $(srcdir)/*.cs .vpath_hook
	$(CSCOMP) -v -nologo $(CSHARP_DEBUG) -out:$@ -target:library $(srcdir)/*.cs
endif

csharpdir = $(pkglibdir)
csharp_DATA = csmsgque.dll

BUILT_SOURCES = .vpath_hook csmsgque.dll

VPATH_FILES = csmsgque.dll

EXTRA_DIST = $(srcdir)/*.cs $(BUILT_SOURCES)

MAINTAINERCLEANFILES = $(VPATH_FILES)

distclean-local:
	$(VPATH_HOOK_CLEANUP)

dist-hook:
	$(VPATH_HOOK_DIST)

