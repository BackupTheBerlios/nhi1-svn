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

.libs:
	mkdir .libs

C_FILES = sqlite3.c

H_FILES = config.h sqlite3.h sqlite3ext.h

#noinst_LTLIBRARIES = libtmp.la libtmps.la
#
#libtmp_la_SOURCES = $(C_FILES) $(H_FILES)
#libtmp_la_CFLAGS = -D_HAVE_SQLITE_CONFIG_H $(AM_CFLAGS)
#
#libtmps_la_SOURCES = $(C_FILES) $(H_FILES)
#libtmps_la_CFLAGS = -D_HAVE_SQLITE_CONFIG_H -static $(AM_CFLAGS)
#libtmps_la_LDFLAGS = -static
#
#html-local: libtmp.la libtmps.la

if USE_WINSDK

.libs/%.obj: %.c | .libs
	$(SDK_CL) "-I../.. -D_HAVE_SQLITE_CONFIG_H" "$(shell $(CYGPATH_M) $<)"

.libs/libtmp.lib: $(patsubst %.c,.libs/%.obj, $(C_FILES)) | .libs
	$(SDK_LIB) "$@" "$^"

all-local: .libs/libtmp.lib

endif

