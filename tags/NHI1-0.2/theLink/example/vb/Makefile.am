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


#####################################################################################
## VPATH setup

.vpath_hook:
	$(VPATH_HOOK)

##
## ==========================================================================
## VisualBasic -> compile ONLY on windows
##

if USE_VB

manfilter.exe: manfilter.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

mulclient.exe: mulclient.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

mulserver.exe: mulserver.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

MyServer.exe: MyServer.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

MyClient.exe: MyClient.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

Filter1.exe: Filter1.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

Filter2.exe: Filter2.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

testclient.exe: testclient.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

testserver.exe: testserver.vb ../../csmsgque/csmsgque.dll
	@echo "WARNING: compile file '$@' on windows"

endif

BUILT_SOURCES = .vpath_hook manfilter.exe mulclient.exe mulserver.exe MyServer.exe \
		  MyClient.exe Filter1.exe Filter2.exe testclient.exe testserver.exe

##
## ==========================================================================
## Main
##

CLEANFILES = *.mdb
EXTRA_DIST = $(srcdir)/*.vb $(srcdir)/*.dll $(srcdir)/*.exe
MAINTAINERCLEANFILES = *.exe
VPATH_FILES = manfilter.exe mulclient.exe mulserver.exe MyServer.exe \
                  MyClient.exe Filter1.exe Filter2.exe testclient.exe \
		      testserver.exe csmsgque.dll

distclean-local:
	$(VPATH_HOOK_CLEANUP)

dist-hook:
	$(VPATH_HOOK_DIST)

