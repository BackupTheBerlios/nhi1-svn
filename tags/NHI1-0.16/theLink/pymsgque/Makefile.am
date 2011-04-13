#+
#§  \file       theLink/pymsgque/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

AM_LIBTOOLFLAGS = --tag=disable-static

pkglib_LTLIBRARIES = pymsgque.la

pymsgque_la_SOURCES = MqS_python.c MqBufferS_python.c \
  msgque_python.c msgque_python.h misc_python.c send_python.c read_python.c \
    service_python.c error_python.c config_python.c slave_python.c context_python.c \
      link_python.c storage_python.c MqFactoryS_python.c MqDumpS_python.c

pymsgque_la_CFLAGS = -I$(srcdir)/../libmsgque $(PYTHON_CPPFLAGS) $(AM_CFLAGS) -DMQ_IGNORE_EXTERN
#pymsgque_la_CFLAGS = -I$(srcdir)/../libmsgque -DMQ_IGNORE_EXTERN $(PYTHON_CPPFLAGS) -fno-strict-aliasing $(AM_CFLAGS)
pymsgque_la_LIBADD = ../libmsgque/libtmp.la $(PYTHON_EXTRA_LIBS)

pymsgque_la_LDFLAGS = -shared -module -avoid-version -no-undefined $(PYTHON_LDFLAGS) $(PYTHON_EXTRA_LDFLAGS)

EXTRA_DIST = setup.py

#CLEANFILES = 
#html: libtclmsgque.la pkgIndex.tcl

