#+
#:  \file       theLink/javamsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

AM_JAVACFLAGS = -Xlint:unchecked
JAVAROOT = $(builddir)
SUFFIXES = .c .h .class .java .jar

srcdir_m = $(abs_top_srcdir_m)/theLink/javamsgque
builddir_m = $(abs_top_builddir_m)/theLink/javamsgque

#####################################################################################
## public JNI

JAVAH_H_FILES = javamsgque_MqS.h javamsgque_MqBufferS.h javamsgque_MqFactoryS.h javamsgque_MqDumpS.h \
		  javamsgque_MqS_WAIT.h javamsgque_MqS_TIMEOUT.h javamsgque_MqS_START.h

$(JAVAH_H_FILES): $(MQS_MAIN)
	"$(JAVAH)" -classpath . -force -jni javamsgque.MqS javamsgque.MqBufferS javamsgque.MqFactoryS javamsgque.MqDumpS

pkglib_LTLIBRARIES = libjavamsgque.la

libjavamsgque_la_SOURCES =  context_java.c context_java.h \
			    read_java.c send_java.c service_java.c \
			    misc_java.c config_java.c slave_java.c \
			    error_java.c link_java.c storage_java.c \
			    MqBufferS_java.c MqFactoryS_java.c MqDumpS_java.c

nodist_libjavamsgque_la_SOURCES = $(JAVAH_H_FILES)

# use this if you want to link again the dynmic library libmsgque...
libjavamsgque_la_CPPFLAGS = $(JAVA_CPPFLAGS)
libjavamsgque_la_CFLAGS = -shared -I$(srcdir)/../libmsgque -DMQ_IGNORE_EXTERN $(JAVA_CPPFLAGS)
libjavamsgque_la_LDFLAGS = -shared -module -avoid-version -no-undefined 
libjavamsgque_la_LIBADD = ../libmsgque/libtmp.la

if WIN32
libjavamsgque_la_LDFLAGS += -Wl,--add-stdcall-alias
endif

#####################################################################################
## JAR file

JAVA_FILES = $(srcdir)/*.java

MQS_MAIN = javamsgque/MqS.class

$(MQS_MAIN): $(JAVA_FILES)
	"$(JAVAC)" -classpath "$(builddir_m)/javamsgque.jar$(PATH_SEP)." $(JAVA_DEBUG) -d . -Xlint:unchecked $(srcdir_m)/*.java 

javamsgque.jar: $(MQS_MAIN) Manifest.txt 
	"$(JAR)" cfm "$@" Manifest.txt javamsgque/*.class

mqsdir = $(pkglibdir)
mqs_DATA = javamsgque.jar

#####################################################################################
## end

EXTRA_DIST = $(JAVA_FILES)

clean-local:
	-rm -fr javamsgque
	-rm -f $(JAVAH_H_FILES) javamsgque.jar

