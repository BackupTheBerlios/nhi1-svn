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
    io.c msgque.c read.c sys.c tcp_io.c pipe_io.c trans.c slave.c config.c \
    debug.h msgque_private.h sys_com.h main.h \
    service.c link.c factory.c sql.c

libtmp_la_CFLAGS = -DMQ_IGNORE_EXTERN -Wstrict-prototypes -I$(srcdir)/../sqlite $(AM_CFLAGS)
libtmp_la_LDFLAGS = -no-undefined
libtmp_la_LIBADD = $(builddir)/../sqlite/libtmp.la

html: libtmp.la

# ordinaray library

lib_LTLIBRARIES = libmsgque.la

libmsgque_la_SOURCES = buffer.c cache.c event.c log.c \
    send.c swap.c sys_com.c token.c uds_io.c bufferL.c error.c generic_io.c \
    io.c msgque.c read.c sys.c tcp_io.c pipe_io.c trans.c slave.c config.c \
    debug.h msgque_private.h sys_com.h main.h \
    service.c link.c factory.c sql.c

libmsgque_la_CFLAGS = -DMQ_BUILD_LIBMSGQUE_DLL -Wstrict-prototypes -I$(srcdir)/../sqlite $(AM_CFLAGS)
libmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libmsgque_la_LIBADD = $(builddir)/../sqlite/libtmp.la

include_HEADERS = msgque.h

