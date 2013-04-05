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

noinst_LTLIBRARIES = libtmps.la libtmp.la

# static convinience library

libtmps_la_SOURCES = $(SOURCES)

libtmps_la_CFLAGS = -static -DMQ_IGNORE_EXTERN -Wstrict-prototypes -I$(srcdir)/../sqlite $(AM_CFLAGS) $(WINDEFS)
libtmps_la_LDFLAGS = -static -no-undefined
libtmps_la_LIBADD = $(WINLIBS)

# dynamic convinience library

libtmp_la_SOURCES = $(SOURCES)

libtmp_la_CFLAGS = -shared -DMQ_IGNORE_EXTERN -Wstrict-prototypes -I$(srcdir)/../sqlite $(AM_CFLAGS) $(WINDEFS)
libtmp_la_LDFLAGS = -shared -no-undefined
libtmp_la_LIBADD = $(WINLIBS)

# ordinaray library

lib_LTLIBRARIES = libmsgque.la

libmsgque_la_SOURCES = $(SOURCES)

libmsgque_la_CFLAGS = -shared -DMQ_BUILD_LIBMSGQUE_DLL -Wstrict-prototypes -I$(srcdir)/../sqlite $(AM_CFLAGS) $(WINDEFS)
libmsgque_la_LDFLAGS = -shared -version-info @VERSION_INFO@ -no-undefined
libmsgque_la_LIBADD = $(WINLIBS)

#if USE_WINSDK
#libmsgque_la_LDFLAGS += -Xlinker --output-def -Xlinker .libs/libmsgque.dll.def
#endif

include_HEADERS = msgque.h

html-local: libtmp.la libtmps.la

if USE_WINSDK

VPATH += $(srcdir)/../sqlite

.libs/%.obj: %.c | .libs
	$(SDK_CL) "-I../.. -I$(abs_top_srcdir_m)/theLink/sqlite" "$(shell $(CYGPATH_M) $<)"

.libs/libmsgque.dll .libs/libmsgque.exp .libs/libmsgque.lib .libs/libmsgque.pdb: $(patsubst %.c,.libs/%.obj, $(notdir $(C_FILES))) | .libs
	$(SDK_LINK) "$@" "$^" ws2_32.lib

sdklibdir = $(libdir)
sdklib_DATA = .libs/libmsgque.exp .libs/libmsgque.lib

sdkbindir = $(bindir)
sdkbin_DATA = .libs/libmsgque.dll .libs/libmsgque.pdb

endif
