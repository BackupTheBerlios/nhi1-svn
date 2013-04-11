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

bin_PROGRAMS = abrain

abrain_SOURCES = abrain.c $(srcdir)/../../theLink/sqlite/sqlite3.c
abrain_CPPFLAGS = -I$(srcdir)/../../theLink/libmsgque -I$(srcdir)/../../theLink/sqlite -DMQ_IGNORE_EXTERN -D_HAVE_SQLITE_CONFIG_H
abrain_CFLAGS = -static
abrain_LDLAGS = -static
abrain_LDADD = ../../theLink/libmsgque/libmsgque.la

