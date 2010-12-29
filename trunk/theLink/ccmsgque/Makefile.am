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

libtmp_la_SOURCES = buffer.cpp cache.cpp event.cpp \
    log.cpp send.cpp swap.cpp sys_com.cpp \
    token.cpp uds_io.cpp bufferL.cpp error.cpp \
    generic_io.cpp io.cpp msgque.cpp read.cpp \
    sys.cpp tcp_io.cpp pipe_io.cpp trans.cpp slave.cpp config.cpp \
    service.cpp factory.cpp link.cpp \
    \
    ccmsgque.cpp ccsend.cpp ccmisc.cpp ccservice.cpp \
    \
    buffer.h ccmsgque.h error.h mq_io.h pipe_io.h \
    slave.h sys.h trans.h bufferL.h ccmsgque_private.h event.h \
    log.h msgque.h read.h swap.h tcp_io.h uds_io.h cache.h debug.h \
    generic_io.h main.h msgque_private.h send.h sys_com.h token.h \
    link.h config.h factory.h

libtmp_la_CXXFLAGS = -DMQ_COMPILE_AS_CC -DMQ_IGNORE_EXTERN $(AM_CFLAGS)
libtmp_la_LDFLAGS = -no-undefined


# ordinary libraray

lib_LTLIBRARIES = libccmsgque.la

libccmsgque_la_SOURCES = buffer.cpp cache.cpp event.cpp \
    log.cpp send.cpp swap.cpp sys_com.cpp \
    token.cpp uds_io.cpp bufferL.cpp error.cpp \
    generic_io.cpp io.cpp msgque.cpp read.cpp \
    sys.cpp tcp_io.cpp pipe_io.cpp trans.cpp slave.cpp config.cpp \
    service.cpp factory.cpp link.cpp \
    \
    ccmsgque.cpp ccsend.cpp ccmisc.cpp ccservice.cpp \
    \
    buffer.h ccmsgque.h error.h mq_io.h pipe_io.h \
    slave.h sys.h trans.h bufferL.h ccmsgque_private.h event.h \
    log.h msgque.h read.h swap.h tcp_io.h uds_io.h cache.h debug.h \
    generic_io.h main.h msgque_private.h send.h sys_com.h token.h \
    link.h config.h factory.h


libccmsgque_la_CXXFLAGS = -DMQ_COMPILE_AS_CC -DMQ_BUILD_LIBMSGQUE_DLL $(AM_CFLAGS)
libccmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

include_HEADERS = ccmsgque.h
