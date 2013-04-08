#+
#:  \file       theLink/libmsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

C_FILES = buffer.c cache.c event.c log.c \
    send.c swap.c sys_com.c token.c uds_io.c bufferL.c error.c generic_io.c \
    io.c msgque.c read.c sys.c tcp_io.c pipe_io.c trans.c slave.c config.c \
    service.c link.c factory.c sql.c \
    ../sqlite/sqlite3.c

H_FILES = debug.h msgque_private.h sys_com.h main.h \
    ../sqlite/config.h ../sqlite/sqlite3.h ../sqlite/sqlite3ext.h

SOURCES = $(C_FILES) $(H_FILES)

.libs:
	mkdir .libs

# ordinaray library

lib_LTLIBRARIES = libmsgque.la

libmsgque_la_SOURCES = $(SOURCES)

libmsgque_la_CFLAGS = -DMQ_BUILD_LIBMSGQUE_DLL -Wstrict-prototypes -I$(srcdir)/../sqlite $(AM_CFLAGS) $(WINDEFS)
libmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libmsgque_la_LIBADD = $(WINLIBS)

include_HEADERS = msgque.h

html-local: libtmp.la libtmps.la

if USE_WINSDK

VPATH += $(srcdir)/../sqlite

.libs/%.obj: %.c $(H_FILES) | .libs
	$(SDK_CL) "-I../.. -I$(abs_top_srcdir_m)/theLink/sqlite" -DMQ_BUILD_LIBMSGQUE_DLL "$(shell $(CYGPATH_M) $<)"

.libs/libmsgque.dll: $(patsubst %.c,.libs/%.obj, $(notdir $(C_FILES))) | .libs
	$(SDK_LINK) "$@" "$^" ws2_32.lib

.libs/libmsgque.exp .libs/libmsgque.lib .libs/libmsgque.pdb: .libs/libmsgque.dll

sdklibdir = $(libdir)
sdklib_DATA = .libs/libmsgque.exp .libs/libmsgque.lib

sdkbindir = $(bindir)
sdkbin_DATA = .libs/libmsgque.dll .libs/libmsgque.pdb

endif
