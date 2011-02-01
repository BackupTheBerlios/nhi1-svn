#+
#§  \file       theLink/ccmsgque/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

noinst_LTLIBRARIES = libtmp.la

libtmp_la_SOURCES = buffer.cc cache.cc event.cc \
    log.cc send.cc swap.cc sys_com.cc \
    token.cc uds_io.cc bufferL.cc error.cc \
    generic_io.cc io.cc msgque.cc read.cc \
    sys.cc tcp_io.cc pipe_io.cc trans.cc slave.cc config.cc \
    service.cc factory.cc link.cc sql.cc \
    \
    ccmsgque.cc ccsend.cc ccmisc.cc ccservice.cc \
    \
    ccmsgque.h ccmsgque_private.h msgque.h debug.h main.h msgque_private.h sys_com.h 

libtmp_la_CXXFLAGS = -DMQ_COMPILE_AS_CC -DMQ_IGNORE_EXTERN -I$(srcdir)/../sqlite $(AM_CFLAGS)
libtmp_la_LDFLAGS = -no-undefined
libtmp_la_LIBADD = $(builddir)/../sqlite/libtmp.la


# ordinary libraray

lib_LTLIBRARIES = libccmsgque.la

libccmsgque_la_SOURCES = buffer.cc cache.cc event.cc \
    log.cc send.cc swap.cc sys_com.cc \
    token.cc uds_io.cc bufferL.cc error.cc \
    generic_io.cc io.cc msgque.cc read.cc \
    sys.cc tcp_io.cc pipe_io.cc trans.cc slave.cc config.cc \
    service.cc factory.cc link.cc sql.cc \
    \
    ccmsgque.cc ccsend.cc ccmisc.cc ccservice.cc \
    \
    ccmsgque.h ccmsgque_private.h msgque.h debug.h main.h msgque_private.h sys_com.h 


libccmsgque_la_CXXFLAGS = -DMQ_COMPILE_AS_CC -DMQ_BUILD_LIBMSGQUE_DLL -I$(srcdir)/../sqlite $(AM_CFLAGS)
libccmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libccmsgque_la_LIBADD = $(builddir)/../sqlite/libtmp.la

include_HEADERS = ccmsgque.h
