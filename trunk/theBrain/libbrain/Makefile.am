#+
#§  \file       theLink/libmsgque/Makefile.am
#§  \brief      \$Id: Makefile.am 25 2009-12-03 11:05:46Z aotto1968 $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 25 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

noinst_LTLIBRARIES = libbrain.la

# ccache gcc -c -I. -I/home/dev1usr/ext/x86_64-suse-linux/thread/include -I/home/dev1usr/include -I/usr/local/include 
# -DNDEBUG 
# -D_GNU_SOURCE=1 -D_REENTRANT -D__EXTENSIONS__ -UNDEBUG -D_TC_PREFIX="\"/home/dev1usr/ext/x86_64-suse-linux/thread\"" 
# -D_TC_INCLUDEDIR="\"/home/dev1usr/ext/x86_64-suse-linux/thread/include\"" 
# -D_TC_LIBDIR="\"/home/dev1usr/ext/x86_64-suse-linux/thread/lib\"" 
# -D_TC_BINDIR="\"/home/dev1usr/ext/x86_64-suse-linux/thread/bin\"" 
# -D_TC_LIBEXECDIR="\"/home/dev1usr/ext/x86_64-suse-linux/thread/libexec\"" 
# -D_TC_APPINC="\"-I/home/dev1usr/ext/x86_64-suse-linux/thread/include\"" 
# -D_TC_APPLIBS="\"-L/home/dev1usr/ext/x86_64-suse-linux/thread/lib 
# -ltokyocabinet -lbz2 -lz -lrt -lpthread -lm -lc \"" -g -O2 -std=c99 -Wall -fPIC -pedantic -fsigned-char -g -O0 tcadb.c

libbrain_la_SOURCES = md5.c  myconf.c  myconf.h  tcadb.c  tcadb.h  tcbdb.c  \
  tcbdb.h  tcfdb.c  tcfdb.h  tchdb.c  tchdb.h  tctdb.c  tctdb.h  tcutil.c  tcutil.h

libbrain_la_CFLAGS = -DMQ_BUILD_LIBMSGQUE_DLL $(AM_CFLAGS) -D_GNU_SOURCE=1 -D_REENTRANT -D__EXTENSIONS__ \
			-pedantic -fsigned-char $(BRAIN_CFLAGS)
libbrain_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libbrain_la_LIBADD = $(BRAIN_LIBADD)

include_HEADERS = tcadb.h  tcbdb.h  tcfdb.h  tchdb.h  tctdb.h  tcutil.h
