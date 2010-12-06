#+
#§  \file       theLink/tests/Makefile.am
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
## test tools setup
##

LIBMSGQUE = ../libmsgque/liblocal.la
CCMSGQUE = ../ccmsgque/liblocal.la

SUFFIXES =
BUILT_SOURCES = .vpath_hook

docs/server.help: server
	./server$(EXEEXT) --help 2>docs/server.help

docs/client.help: client
	./client$(EXEEXT) --help 2>docs/client.help

docs/debug.out: client server
	./client$(EXEEXT) --num 1 --debug 9 @ ./server$(EXEEXT) 2>docs/debug.out

docs/client.perf: client server
	./client$(EXEEXT) --all @ ./server$(EXEEXT) 2>docs/client.perf

noinst_PROGRAMS = server$(EXEEXT) client$(EXEEXT)

HTMLFILES = docs/server.help docs/client.help docs/debug.out docs/client.perf

html: $(HTMLFILES)

server_SOURCES = server.c 
server_CFLAGS = -I$(srcdir)/../libmsgque -DMQ_IGNORE_EXTERN
server_LDADD = -lm $(LIBMSGQUE)

client_SOURCES = client.c stat.c stat.h
client_CFLAGS = -I$(srcdir)/../libmsgque -DMQ_IGNORE_EXTERN
client_LDADD = -lm $(LIBMSGQUE)

##
## ==========================================================================
## C++ setup
##

if USE_CXX
noinst_PROGRAMS += ccserver$(EXEEXT)
ccserver_SOURCES = ccserver.cc
ccserver_CXXFLAGS = -I$(srcdir)/../ccmsgque -DMQ_IGNORE_EXTERN
ccserver_LDADD = $(CCMSGQUE)
endif

##
## ==========================================================================
## JAVA setup
##

if USE_JAVA

SUFFIXES += .java .class

RULE_JAVA_EXE = $(JAVAC) -classpath "../javamsgque/javamsgque.jar$(PATH_SEP)." $(JAVA_DEBUG) -d . -Xlint:unchecked "$<"

example/Server.class: Server.java ../javamsgque/javamsgque.jar .vpath_hook
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/Server.class

endif

##
## ==========================================================================
## C# setup
##

if USE_CSHARP

csserver.exe: $(srcdir)/csserver.cs ../csmsgque/csmsgque.dll .vpath_hook
	$(CSCOMP) -v -nologo $(CSHARP_DEBUG) -reference:../csmsgque/csmsgque.dll -out:$@ -target:exe "$<"

BUILT_SOURCES += csserver.exe

endif

##
## ==========================================================================
## VB setup
##

if USE_VB

# not yet ready !!!
#$(VBCOMP) -nologo $(VB_DEBUG) -r:../csmsgque/csmsgque.dll -out:$@ -target:exe $(srcdir)/vbserver.vb

SUFFIXES += .exe .vb 

vbserver.exe: vbserver.vb ../csmsgque/csmsgque.dll .vpath_hook
	@echo "WARNING: compile file '$@' on windows"

BUILT_SOURCES += vbserver.exe

endif

##
## ==========================================================================
## Main
##

EXTRA_DIST = $(srcdir)/*.tcl $(srcdir)/*.test $(srcdir)/*.py $(srcdir)/*.pl \
		README.tests $(srcdir)/*.java $(srcdir)/*.cs $(srcdir)/*.vb \
		  $(srcdir)/*.rb $(srcdir)/*.php $(srcdir)/*.exe $(srcdir)/*.env \
		    MakeTclTest.bash massclient.bash

VPATH_FILES = vbserver.exe csserver.exe example

MAINTAINERCLEANFILES = vbserver.exe csserver.exe example/*.class

distclean-local:
	$(VPATH_HOOK_CLEANUP)

dist-hook: example/Server.class
	$(VPATH_HOOK_DIST)
	mkdir $(distdir)/example
	chmod u+w $(distdir)/example
	cp $(srcdir)/example/*.class $(distdir)/example
	chmod u+w $(distdir)/example/*.class

TESTS_ENVIRONMENT = $(TCLSH)
TESTS = $(srcdir)/all.tcl

