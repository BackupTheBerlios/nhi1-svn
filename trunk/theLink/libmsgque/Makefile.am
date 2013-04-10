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

H_FILES = msgque.h debug.h msgque_private.h sys_com.h main.h

lib_LTLIBRARIES = libmsgque.la

libmsgque_la_SOURCES = $(C_FILES) $(H_FILES)

libmsgque_la_CFLAGS = -DMQ_BUILD_LIBMSGQUE_DLL -Wstrict-prototypes -I$(srcdir)/../sqlite $(WINDEFS)
libmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libmsgque_la_LIBADD = $(WINLIBS)

include_HEADERS = msgque.h

## ===========================================================================================
## windows with SDK
##

if USE_WINSDK

.libs:
	mkdir .libs

VPATH += $(srcdir)/../sqlite

.libs/%.obj: %.c $(H_FILES) | .libs
	$(SDK_CL) -I../.. -I$(abs_top_srcdir_m)/theLink/sqlite -DMQ_BUILD_LIBMSGQUE_DLL "$(shell $(CYGPATH_M) $<)"

.libs/msgque.dll: $(patsubst %.c,.libs/%.obj, $(notdir $(C_FILES))) | .libs
	$(SDK_LINK) "$@" "$^" ws2_32.lib

.libs/msgque.exp .libs/msgque.lib .libs/msgque.pdb: .libs/msgque.dll

sdklibdir = $(libdir)
sdklib_DATA = .libs/msgque.exp .libs/msgque.lib

sdkbindir = $(bindir)
sdkbin_DATA = .libs/msgque.dll .libs/msgque.pdb

endif

