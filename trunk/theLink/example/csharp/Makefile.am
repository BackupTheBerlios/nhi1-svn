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


##
## ==========================================================================
## C#
##

SUFFIXES = *.exe *.cs *.dll

RULE_CS_EXE = $(CSCOMP) $(CSHARP_OPT) -nologo $(CSHARP_DEBUG) \
		-reference:csmsgque.dll \
		-out:$@ -target:exe `$(CYGPATH_W) "$<"`

csmsgque.dll: ../../csmsgque/csmsgque.dll
	cp ../../csmsgque/csmsgque.dll .

manfilter.exe: $(srcdir)/manfilter.cs csmsgque.dll 
	$(RULE_CS_EXE)

mulclient.exe: $(srcdir)/mulclient.cs csmsgque.dll
	$(RULE_CS_EXE)

mulserver.exe: $(srcdir)/mulserver.cs csmsgque.dll
	$(RULE_CS_EXE)

MyServer.exe: $(srcdir)/MyServer.cs csmsgque.dll
	$(RULE_CS_EXE)

MyClient.exe: $(srcdir)/MyClient.cs csmsgque.dll
	$(RULE_CS_EXE)

Filter1.exe: $(srcdir)/Filter1.cs csmsgque.dll
	$(RULE_CS_EXE)

Filter2.exe: $(srcdir)/Filter2.cs csmsgque.dll
	$(RULE_CS_EXE)

Filter3.exe: $(srcdir)/Filter3.cs csmsgque.dll
	$(RULE_CS_EXE)

Filter4.exe: $(srcdir)/Filter4.cs csmsgque.dll
	$(RULE_CS_EXE)

Filter5.exe: $(srcdir)/Filter5.cs csmsgque.dll
	$(RULE_CS_EXE)

Filter6.exe: $(srcdir)/Filter6.cs csmsgque.dll
	$(RULE_CS_EXE)

testclient.exe: $(srcdir)/testclient.cs csmsgque.dll
	$(RULE_CS_EXE)

testserver.exe: $(srcdir)/testserver.cs csmsgque.dll
	$(RULE_CS_EXE)

server.exe: $(srcdir)/server.cs csmsgque.dll
	$(RULE_CS_EXE)

BUILT_SOURCES = manfilter.exe mulclient.exe mulserver.exe MyServer.exe MyClient.exe \
                  Filter1.exe Filter2.exe Filter3.exe Filter4.exe Filter5.exe \
		    Filter6.exe testclient.exe testserver.exe server.exe

##
## ==========================================================================
## Main
##

CLEANFILES = *.exe *.mdb *.dll
EXTRA_DIST = $(srcdir)/*.cs

