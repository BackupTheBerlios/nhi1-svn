#+
#§  \file       theLink/example/csharp/Makefile.am
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
## C#
##

SUFFIXES = *.exe *.cs *.dll

RULE_CS_EXE = $(CSCOMP) -v -nologo $(CSHARP_DEBUG) -reference:../../csmsgque/csmsgque.dll -out:$@ -target:exe "$<"

manfilter.exe: $(srcdir)/manfilter.cs ../../csmsgque/csmsgque.dll 
	$(RULE_CS_EXE)

mulclient.exe: $(srcdir)/mulclient.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

mulserver.exe: $(srcdir)/mulserver.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

MyServer.exe: $(srcdir)/MyServer.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

MyClient.exe: $(srcdir)/MyClient.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

Filter1.exe: $(srcdir)/Filter1.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

Filter2.exe: $(srcdir)/Filter2.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

Filter3.exe: $(srcdir)/Filter3.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

Filter4.exe: $(srcdir)/Filter4.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

testclient.exe: $(srcdir)/testclient.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

testserver.exe: $(srcdir)/testserver.cs ../../csmsgque/csmsgque.dll
	$(RULE_CS_EXE)

BUILT_SOURCES = .vpath_hook manfilter.exe mulclient.exe mulserver.exe MyServer.exe \
		  MyClient.exe Filter1.exe Filter2.exe Filter3.exe Filter4.exe \
		    testclient.exe testserver.exe

##
## ==========================================================================
## Main
##

MAINTAINERCLEANFILES = *.exe *.mdb
EXTRA_DIST = $(srcdir)/*.cs $(BUILT_SOURCES)
VPATH_FILES = manfilter.exe mulclient.exe mulserver.exe MyServer.exe MyClient.exe \
                  Filter1.exe Filter2.exe Filter3.exe Filter4.exe \
		    testclient.exe testserver.exe

distclean-local:
	$(VPATH_HOOK_CLEANUP)

dist-hook:
	$(VPATH_HOOK_DIST)
