#+
#:  \file       theLink/sqlite/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

# convinience library

noinst_LTLIBRARIES = libtmp.la libtmps.la

libtmp_la_SOURCES = config.h sqlite3.h sqlite3ext.h sqlite3.c
libtmp_la_CFLAGS = -D_HAVE_SQLITE_CONFIG_H $(AM_CFLAGS)

libtmps_la_SOURCES = config.h sqlite3.h sqlite3ext.h sqlite3.c
libtmps_la_CFLAGS = -D_HAVE_SQLITE_CONFIG_H -static $(AM_CFLAGS)
libtmps_la_LDFLAGS = -static
