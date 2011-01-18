#+
#§  \file       theLink/libmsgque/Makefile.am
#§  \brief      \$Id: Makefile.am 392 2010-12-30 18:50:45Z aotto1968 $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 392 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

# convinience library

noinst_LTLIBRARIES = libtmp.la

libtmp_la_SOURCES = config.h sqlite3.h sqlite3ext.h sqlite3.c

#libtmp_la_CFLAGS = -DSQLITE_EXTERN -D_HAVE_SQLITE_CONFIG_H $(AM_CFLAGS)

libtmp_la_CFLAGS = -D_HAVE_SQLITE_CONFIG_H $(AM_CFLAGS)

