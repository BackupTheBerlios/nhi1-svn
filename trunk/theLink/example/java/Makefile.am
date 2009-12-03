#+
#§  \file       theLink/example/java/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§


#####################################################################################
## VPATH setup

.vpath_hook:
	$(VPATH_HOOK)

##
## ==========================================================================
## Java
##

RULE_JAVA_EXE = $(JAVAC) -classpath "../../javamsgque/javamsgque.jar$(PATH_SEP)." $(JAVA_DEBUG) -d . -Xlint:unchecked "$<"

SUFFIXES = .java .class
VPATH_FILES = example
BUILT_SOURCES = .vpath_hook

BUILT_SOURCES += example/mulclient.class
example/mulclient.class: $(srcdir)/mulclient.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/mulserver.class
example/mulserver.class: $(srcdir)/mulserver.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/manfilter.class
example/manfilter.class: $(srcdir)/manfilter.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/MyClient.class
example/MyClient.class: $(srcdir)/MyClient.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/MyServer.class
example/MyServer.class: $(srcdir)/MyServer.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/Filter1.class
example/Filter1.class: $(srcdir)/Filter1.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/Filter2.class
example/Filter2.class: $(srcdir)/Filter2.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/testclient.class
example/testclient.class: $(srcdir)/testclient.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/testserver.class
example/testserver.class: $(srcdir)/testserver.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

##
## ==========================================================================
## Main
##

MAINTAINERCLEANFILES = example/*.class
EXTRA_DIST = $(srcdir)/*.java

distclean-local:
	$(VPATH_HOOK_CLEANUP)

dist-hook: $(BUILT_SOURCES)
	$(VPATH_HOOK_DIST)
	mkdir $(distdir)/example
	chmod u+w $(distdir)/example
	cp example/*.class $(distdir)/example
	chmod u+w $(distdir)/example/*.class

