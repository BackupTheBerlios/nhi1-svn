#+
#:  \file       theBrain/abrain/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

C_FILES = abrain.c \
      ../../theLink/sqlite/sqlite3.c \
      ../../theLink/libmsgque/buffer.c \
      ../../theLink/libmsgque/cache.c \
      ../../theLink/libmsgque/event.c \
      ../../theLink/libmsgque/log.c \
      ../../theLink/libmsgque/send.c \
      ../../theLink/libmsgque/swap.c \
      ../../theLink/libmsgque/sys_com.c \
      ../../theLink/libmsgque/token.c \
      ../../theLink/libmsgque/uds_io.c \
      ../../theLink/libmsgque/bufferL.c \
      ../../theLink/libmsgque/error.c \
      ../../theLink/libmsgque/generic_io.c \
      ../../theLink/libmsgque/io.c \
      ../../theLink/libmsgque/msgque.c \
      ../../theLink/libmsgque/read.c \
      ../../theLink/libmsgque/sys.c \
      ../../theLink/libmsgque/tcp_io.c \
      ../../theLink/libmsgque/pipe_io.c \
      ../../theLink/libmsgque/trans.c \
      ../../theLink/libmsgque/slave.c \
      ../../theLink/libmsgque/config.c \
      ../../theLink/libmsgque/service.c \
      ../../theLink/libmsgque/link.c \
      ../../theLink/libmsgque/factory.c \
      ../../theLink/libmsgque/sql.c

bin_PROGRAMS = abrain

abrain_SOURCES = $(C_FILES)
abrain_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -I$(srcdir)/../../theLink/sqlite -DMQ_IGNORE_EXTERN -D_HAVE_SQLITE_CONFIG_H
abrain_CFLAGS = -static
abrain_LDLAGS = -static
#abrain_LDADD = ../../theLink/libmsgque/libmsgque.la

