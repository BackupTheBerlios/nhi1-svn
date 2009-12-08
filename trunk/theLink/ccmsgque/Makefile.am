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
    \
    ccmsgque.cc cccontext.cc ccsend.cc ccmisc.cc ccservice.cc \
    \
    buffer.h ccmsgque.h error.h io_private.h mq_io.h pipe_io.h \
    slave.h sys.h trans.h bufferL.h ccmsgque_private.h event.h \
    log.h msgque.h read.h swap.h tcp_io.h uds_io.h cache.h debug.h \
    generic_io.h main.h msgque_private.h send.h sys_com.h token.h


libtmp_la_CXXFLAGS = -DMQ_COMPILE_AS_CC -DMQ_BUILD_LIBMSGQUE_DLL $(AM_CFLAGS)
libtmp_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined


# ordinary libraray

lib_LTLIBRARIES = libccmsgque.la

libccmsgque_la_SOURCES = 
libccmsgque_la_LIBADD = ./libtmp.la

include_HEADERS = ccmsgque.h
