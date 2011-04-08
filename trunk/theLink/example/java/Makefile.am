#+
#§  \file       theLink/example/java/Makefile.am
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
## Java
##

RULE_JAVA_EXE = $(JAVAC) -classpath "../../javamsgque/javamsgque.jar$(PATH_SEP)." $(JAVA_DEBUG) -d . -Xlint:unchecked "$<"

BUILT_SOURCES =
SUFFIXES = .java .class

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

BUILT_SOURCES += example/Filter3.class
example/Filter3.class: $(srcdir)/Filter3.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/Filter4.class
example/Filter4.class: $(srcdir)/Filter4.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/Filter5.class
example/Filter5.class: $(srcdir)/Filter5.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/Filter6.class
example/Filter6.class: $(srcdir)/Filter6.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/testclient.class
example/testclient.class: $(srcdir)/testclient.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/testserver.class
example/testserver.class: $(srcdir)/testserver.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

BUILT_SOURCES += example/Server.class
example/Server.class: $(srcdir)/Server.java ../../javamsgque/javamsgque.jar
	$(RULE_JAVA_EXE)

##
## ==========================================================================
## Main
##

CLEANFILES = example/*.class
EXTRA_DIST = $(srcdir)/*.java

dist-hook:
	mkdir $(distdir)/example
	chmod u+w $(distdir)/example
	cp example/*.class $(distdir)/example
	chmod u+w $(distdir)/example/*.class

