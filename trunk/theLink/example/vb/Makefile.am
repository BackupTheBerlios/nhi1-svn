#+
#§  \file       theLink/example/vb/Makefile.am
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
## VisualBasic -> compile ONLY on windows
##

SUFFIXES = *.exe *.cs *.dll

RULE_VB_EXE = $(VBCOMP) $(CSHARP_OPT) -nologo $(CSHARP_DEBUG) \
		-reference:csmsgque.dll \
		-out:$@ -target:exe `$(CYGPATH_W) "$<"`

csmsgque.dll: ../../csmsgque/csmsgque.dll
	cp ../../csmsgque/csmsgque.dll .

manfilter.exe: $(srcdir)/manfilter.vb csmsgque.dll
	$(RULE_VB_EXE)

mulclient.exe: $(srcdir)/mulclient.vb csmsgque.dll
	$(RULE_VB_EXE)

mulserver.exe: $(srcdir)/mulserver.vb csmsgque.dll
	$(RULE_VB_EXE)

MyServer.exe: $(srcdir)/MyServer.vb csmsgque.dll
	$(RULE_VB_EXE)

MyClient.exe: $(srcdir)/MyClient.vb csmsgque.dll
	$(RULE_VB_EXE)

Filter1.exe: $(srcdir)/Filter1.vb csmsgque.dll
	$(RULE_VB_EXE)

Filter2.exe: $(srcdir)/Filter2.vb csmsgque.dll
	$(RULE_VB_EXE)

Filter3.exe: $(srcdir)/Filter3.vb csmsgque.dll
	$(RULE_VB_EXE)

Filter4.exe: $(srcdir)/Filter4.vb csmsgque.dll
	$(RULE_VB_EXE)

Filter5.exe: $(srcdir)/Filter5.vb csmsgque.dll
	$(RULE_VB_EXE)

Filter6.exe: $(srcdir)/Filter5.vb csmsgque.dll
	$(RULE_VB_EXE)

testclient.exe: $(srcdir)/testclient.vb csmsgque.dll
	$(RULE_VB_EXE)

testserver.exe: $(srcdir)/testserver.vb csmsgque.dll
	$(RULE_VB_EXE)

server.exe: $(srcdir)/server.vb csmsgque.dll
	$(RULE_VB_EXE)

if WIN32
BUILT_SOURCES = manfilter.exe mulclient.exe mulserver.exe MyServer.exe MyClient.exe \
                  Filter1.exe Filter2.exe Filter3.exe Filter4.exe Filter5.exe \
		    Filter6.exe testclient.exe testserver.exe server.exe
else
BUILT_SOURCES =
endif

##
## ==========================================================================
## Main
##

MAINTAINERCLEANFILES = *.exe *.mdb *.dll
EXTRA_DIST = $(srcdir)/*.vb

