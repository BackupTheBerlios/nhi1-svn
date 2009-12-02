#+
#§  \file       libmsgque/Makefile.am
#§  \brief      \$Id: Makefile.am 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

SUBDIRS =
if USE_CXX
SUBDIRS += ccmsgque
endif
if USE_JAVA
SUBDIRS += javamsgque
endif
if USE_PYTHON
SUBDIRS += pymsgque
endif
if USE_CSHARP
SUBDIRS += csmsgque
endif
if USE_PHP
SUBDIRS += phpmsgque
endif
if USE_PERL
SUBDIRS += perlmsgque
endif
if USE_TCL
SUBDIRS += tclmsgque
endif

lib_LTLIBRARIES = libmsgque.la

libmsgque_la_SOURCES = buffer.c cache.c event.c log.c \
    send.c swap.c sys_com.c token.c uds_io.c bufferL.c error.c generic_io.c \
    io.c msgque.c read.c sys.c tcp_io.c pipe_io.c trans.c slave.c config.c \
    buffer.h debug.h generic_io.h log.h msgque_private.h send.h sys_com.h token.h \
    bufferL.h error.h main.h sys.h trans.h cache.h event.h \
    io_private.h mq_io.h read.h swap.h tcp_io.h pipe_io.h uds_io.h slave.h

libmsgque_la_CFLAGS = -DMQ_BUILD_LIBMSGQUE_DLL $(AM_CFLAGS)
libmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

include_HEADERS = msgque.h

html: libmsgque.la
