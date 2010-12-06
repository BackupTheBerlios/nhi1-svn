#+
#§  \file       theLink/libmsgque/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

# convinience library

noinst_LTLIBRARIES = libtmp.la

libtmp_la_SOURCES = buffer.c cache.c event.c log.c \
    send.c swap.c sys_com.c token.c uds_io.c bufferL.c error.c generic_io.c \
    io.c msgque.c read.c sys.c tcp_io.c pipe_io.c trans.c slave.c config.c config.h \
    buffer.h debug.h generic_io.h log.h msgque_private.h send.h sys_com.h token.h \
    bufferL.h error.h main.h sys.h trans.h cache.h event.h \
    mq_io.h read.h swap.h tcp_io.h pipe_io.h uds_io.h slave.h \
    service.c link.c link.h factory.c factory.h

libtmp_la_CFLAGS = -DMQ_IGNORE_EXTERN -Wstrict-prototypes $(AM_CFLAGS)
libtmp_la_LDFLAGS = -no-undefined

html: libtmp.la

# ordinaray library

lib_LTLIBRARIES = libmsgque.la

libmsgque_la_SOURCES = buffer.c cache.c event.c log.c \
    send.c swap.c sys_com.c token.c uds_io.c bufferL.c error.c generic_io.c \
    io.c msgque.c read.c sys.c tcp_io.c pipe_io.c trans.c slave.c config.c config.h \
    buffer.h debug.h generic_io.h log.h msgque_private.h send.h sys_com.h token.h \
    bufferL.h error.h main.h sys.h trans.h cache.h event.h \
    mq_io.h read.h swap.h tcp_io.h pipe_io.h uds_io.h slave.h \
    service.c link.c link.h factory.c factory.h

libmsgque_la_CFLAGS = -DMQ_BUILD_LIBMSGQUE_DLL -Wstrict-prototypes $(AM_CFLAGS)
libmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

include_HEADERS = msgque.h

