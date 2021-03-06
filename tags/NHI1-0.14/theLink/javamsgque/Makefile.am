#+
#§  \file       theLink/javamsgque/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

AM_JAVACFLAGS = -Xlint:unchecked
JAVAROOT = $(builddir)
SUFFIXES = .c .h .class .java .jar

#####################################################################################
## VPATH setup

.vpath_hook:
	$(VPATH_HOOK)

#####################################################################################
## public JNI

AM_LIBTOOLFLAGS = --tag=disable-static

JAVAH_FILES = javamsgque_MqS.h javamsgque_MqBufferS.h javamsgque_MqFactoryS.h javamsgque_MqDumpS.h
JAVAH_EXTRA_FILES = javamsgque_MqS_WAIT.h javamsgque_MqS_TIMEOUT.h javamsgque_MqS_START.h

javamsgque_MqS.h: $(MQS_MAIN)
	$(JAVAH) -classpath . -force -jni javamsgque.MqS

javamsgque_MqBufferS.h: $(MQS_MAIN)
	$(JAVAH) -classpath . -force -jni javamsgque.MqBufferS

javamsgque_MqFactoryS.h: $(MQS_MAIN)
	$(JAVAH) -classpath . -force -jni javamsgque.MqFactoryS

javamsgque_MqDumpS.h: $(MQS_MAIN)
	$(JAVAH) -classpath . -force -jni javamsgque.MqDumpS

libjavamsgque_la_SOURCES =  context_java.c context_java.h \
			    read_java.c send_java.c service_java.c \
			    misc_java.c config_java.c slave_java.c \
			    error_java.c link_java.c storage_java.c \
			    MqBufferS_java.c MqFactoryS_java.c MqDumpS_java.c

# use this if you want to link again the dynmic library libmsgque...
libjavamsgque_la_CFLAGS = -I$(srcdir)/../libmsgque -DMQ_IGNORE_EXTERN $(JAVA_CPPFLAGS) $(AM_CFLAGS) 
libjavamsgque_la_LIBADD = ../libmsgque/libtmp.la
libjavamsgque_la_LDFLAGS = -module -avoid-version -no-undefined 

if WIN32
libjavamsgque_la_LDFLAGS += -Wl,--add-stdcall-alias
endif

#####################################################################################
## JAR file

JAVA_FILES = $(srcdir)/IBgError.java $(srcdir)/IServerSetup.java  \
    $(srcdir)/MqBufferS.java $(srcdir)/MqS.java $(srcdir)/ICallback.java $(srcdir)/IServerCleanup.java \
      $(srcdir)/IService.java $(srcdir)/MqSException.java $(srcdir)/IEvent.java $(srcdir)/MqFactoryS.java \
	$(srcdir)/MqDumpS.java

MQS_MAIN = javamsgque/MqS.class

$(MQS_MAIN): $(JAVA_FILES)
	$(JAVAC) -classpath "$(top_builddir)$(FILE_SEP)javamsgque$(FILE_SEP)javamsgque.jar$(PATH_SEP)." $(JAVA_DEBUG) -d . -Xlint:unchecked $(srcdir)/*.java 

if USE_JAVA
javamsgque.jar: $(MQS_MAIN) Manifest.txt 
	$(JAR) cfm "$@" Manifest.txt javamsgque/*.class
endif

mqsdir = $(pkglibdir)
mqs_DATA = javamsgque.jar

#####################################################################################
## end

pkglib_LTLIBRARIES = libjavamsgque.la
BUILT_SOURCES = .vpath_hook $(mqs_DATA) $(JAVAH_FILES)
EXTRA_DIST = $(JAVA_FILES) $(JAVAH_EXTRA_FILES) $(BUILT_SOURCES)
MAINTAINERCLEANFILES = $(BUILT_SOURCES) $(JAVAH_EXTRA_FILES) javamsgque/*.class
VPATH_FILES = $(JAVAH_FILES) $(JAVAH_EXTRA_FILES) javamsgque.jar

distclean-local:
	$(VPATH_HOOK_CLEANUP)

dist-hook: $(MQS_MAIN)
	$(VPATH_HOOK_DIST)
	mkdir "$(distdir)/javamsgque"
	chmod u+w "$(distdir)/javamsgque"
	cp $(srcdir)/javamsgque/*.class "$(distdir)/javamsgque"
	chmod u+w $(distdir)/javamsgque/*.class

