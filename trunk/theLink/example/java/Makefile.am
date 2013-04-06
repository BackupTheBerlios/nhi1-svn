#+
#:  \file       theLink/example/java/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

JAR_FILE = ../../javamsgque/javamsgque.jar
srcdir_m = $(abs_top_srcdir_m)/theLink/example/java

##
## ==========================================================================
## Java
##

SUFFIXES = .java .class

JAVA_FILES = $(srcdir)/*.java
CLASS_FILES = $(patsubst $(srcdir)/%.java,example/%.class, $(JAVA_FILES))

$(CLASS_FILES): $(JAVA_FILES) $(JAR_FILE)
	"$(JAVAC)" -classpath "$(JAR_FILE)" $(JAVA_DEBUG) -d . -Xlint:unchecked "$(srcdir_m)/*.java"

##
## ==========================================================================
## Main
##

EXTRA_DIST = $(JAVA_FILES)

all-local: $(CLASS_FILES)

clean-local:
	-rm -fr example
