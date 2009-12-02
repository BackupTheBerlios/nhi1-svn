#+
#§  \file       javamsgque/Makefile.am
#§  \brief      \$Id: Makefile.am 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
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

javamsgque_MqS.h: $(MQS_MAIN)
	$(JAVAH) -classpath . -force -jni javamsgque.MqS

javamsgque_MqBufferS.h: $(MQS_MAIN)
	$(JAVAH) -classpath . -force -jni javamsgque.MqBufferS

libjavamsgque_la_SOURCES =  context_java.c context_java.h \
			    read_java.c send_java.c service_java.c \
			    misc_java.c config_java.c slave_java.c \
			    MqBufferS_java.c error_java.c

# use this if you want to link again the dynmic library libmsgque...
libjavamsgque_la_CFLAGS = -I$(srcdir)/.. $(JAVA_CPPFLAGS) $(AM_CFLAGS) 
libjavamsgque_la_LIBADD = ../*.lo
libjavamsgque_la_LDFLAGS = -module -avoid-version -no-undefined 

if WIN32
libjavamsgque_la_LDFLAGS += -Wl,--add-stdcall-alias
endif

#####################################################################################
## JAR file

MQS_MAIN = javamsgque/MqS.class

$(MQS_MAIN): $(srcdir)/*.java
	$(JAVAC) -classpath "$(top_builddir)$(FILE_SEP)javamsgque$(FILE_SEP)javamsgque.jar$(PATH_SEP)." $(JAVA_DEBUG) -d . -Xlint:unchecked $(srcdir)/*.java 

if USE_JAVA
javamsgque.jar: $(MQS_MAIN) Manifest.txt 
	$(JAR) cfm "$@" Manifest.txt javamsgque
endif

mqsdir = $(pkglibdir)
mqs_DATA = javamsgque.jar

#####################################################################################
## end

pkglib_LTLIBRARIES = libjavamsgque.la
BUILT_SOURCES = .vpath_hook $(mqs_DATA) javamsgque_MqS.h javamsgque_MqBufferS.h
EXTRA_DIST = $(srcdir)/*.java $(BUILT_SOURCES) javamsgque_MqS_WAIT.h
MAINTAINERCLEANFILES = $(BUILT_SOURCES) javamsgque_MqS_WAIT.h javamsgque/*.class
VPATH_FILES = javamsgque javamsgque_MqS.h javamsgque_MqBufferS.h javamsgque_MqS_WAIT.h javamsgque.jar

distclean-local:
	$(VPATH_HOOK_CLEANUP)

dist-hook: $(MQS_MAIN)
	$(VPATH_HOOK_DIST)
	mkdir "$(distdir)/javamsgque"
	chmod u+w "$(distdir)/javamsgque"
	cp javamsgque/*.class "$(distdir)/javamsgque"
	chmod u+w $(distdir)/javamsgque/*.class

