#+
#:  \file       theLink/pymsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

AM_LIBTOOLFLAGS = --tag=disable-static

# To build a python extension on windows using active-state-python at
# C:/Python32 using the mingw cross-compiler 'x86_64-w64-mingw32' of 
# cygwin was NOT sucessfull. Reason:
# 1. dll at /cygdrive/c/Windows/Sysnative/python32.dll
#    32bit cygwin get with /cygdrive/c/Windows/Sytem32 the
#    /cygdrive/c//Windows/SysWOW64 directory
# 2. the default export library /cygdrive/c/Python32/libs/python32.lib
#    copied to /cygdrive/c/Python32/libs/libpython32.dll.a does NOT
#    contail ALL required import-definition for exported data.
#    exp. undefined reference to `__imp__Py_NoneStruct'
# 3. creating an import library using 'x86_64-w64-mingw32-dlltool.exe'
#    create an empty def file.
# next step: using the msvc compiler

pkglib_LTLIBRARIES = pymsgque.la

pymsgque_la_SOURCES = MqS_python.c MqBufferS_python.c \
  msgque_python.c msgque_python.h misc_python.c send_python.c read_python.c \
    service_python.c error_python.c config_python.c slave_python.c context_python.c \
      link_python.c storage_python.c MqFactoryS_python.c MqDumpS_python.c

pymsgque_la_CFLAGS = -I$(srcdir)/../libmsgque $(PYTHON_CFLAGS) -DMQ_IGNORE_EXTERN

if WIN32
pymsgque_la_CFLAGS += -U_DEBUG
endif

pymsgque_la_LIBADD = ../libmsgque/libtmp.la
pymsgque_la_LDFLAGS = -shared -module -avoid-version -no-undefined $(PYTHON_LDFLAGS)

