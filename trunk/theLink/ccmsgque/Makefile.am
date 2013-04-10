#+
#:  \file       theLink/ccmsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:


#	If one of the sublibraries contains non-C source, it is important
#	that the appropriate linker be chosen.  One way to achieve this is to
#	pretend that there is such a non-C file among the sources of the
#	library, thus forcing `automake' to select the appropriate linker.
#	Here is the top-level `Makefile' of our example updated to force C++
#	linking.
#	
#	     SUBDIRS = sub1 sub2 ...
#	     lib_LTLIBRARIES = libtop.la
#	     libtop_la_SOURCES =
#	     # Dummy C++ source to cause C++ linking.
#	     nodist_EXTRA_libtop_la_SOURCES = dummy.cxx
#	     libtop_la_LIBADD = \
#	       sub1/libsub1.la \
#	       sub2/libsub2.la \
#	       ...
#	
#	   `EXTRA_*_SOURCES' variables are used to keep track of source files
#	that might be compiled (this is mostly useful when doing conditional
#	compilation using `AC_SUBST', *note Conditional Libtool Sources::), and
#	the `nodist_' prefix means the listed sources are not to be distributed
#	(*note Program and Library Variables::).  In effect the file
#	`dummy.cxx' does not need to exist in the source tree.  Of course if
#	you have some real source file to list in `libtop_la_SOURCES' there is
#	no point in cheating with `nodist_EXTRA_libtop_la_SOURCES'.

CC_FILES = buffer.cc cache.cc event.cc \
    log.cc send.cc swap.cc sys_com.cc \
    token.cc uds_io.cc bufferL.cc error.cc \
    generic_io.cc io.cc msgque.cc read.cc \
    sys.cc tcp_io.cc pipe_io.cc trans.cc slave.cc config.cc \
    service.cc factory.cc link.cc sql.cc \
    \
    ccmsgque.cc ccsend.cc ccmisc.cc ccservice.cc

C_FILES = ../sqlite/sqlite3.c

H_FILES = ccmsgque.h ccmsgque_private.h

lib_LTLIBRARIES = libccmsgque.la

libccmsgque_la_SOURCES = $(CC_FILES) $(C_FILES) $(H_FILES)

libccmsgque_la_CXXFLAGS = -DMQ_COMPILE_AS_CC -DMQ_BUILD_LIBMSGQUE_DLL -I$(srcdir)/../libmsgque -I$(srcdir)/../sqlite $(WINDEFS)
libccmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libccmsgque_la_LIBADD = $(WINLIBS)

include_HEADERS = ccmsgque.h

## ===========================================================================================
## windows with SDK
##

if USE_WINSDK

.libs:
	mkdir .libs

VPATH += $(srcdir)/../sqlite

.libs/%.obj: %.c $(H_FILES) | .libs
	$(SDK_CL) -I../.. -I$(abs_top_srcdir_m)/theLink/libmsgque -I$(abs_top_srcdir_m)/theLink/sqlite -DMQ_COMPILE_AS_CC -DMQ_BUILD_LIBMSGQUE_DLL "-Tc$(shell $(CYGPATH_M) $<)"

.libs/%.obj: %.cc $(H_FILES) | .libs
	$(SDK_CL) -I../.. -I$(abs_top_srcdir_m)/theLink/libmsgque -I$(abs_top_srcdir_m)/theLink/sqlite -DMQ_COMPILE_AS_CC -DMQ_BUILD_LIBMSGQUE_DLL "-Tp$(shell $(CYGPATH_M) $<)"

.libs/ccmsgque.dll: $(patsubst %.cc,.libs/%.obj, $(notdir $(CC_FILES))) $(patsubst %.c,.libs/%.obj, $(notdir $(C_FILES))) | .libs
	$(SDK_LINK) "$@" "$^" ws2_32.lib

.libs/ccmsgque.exp .libs/ccmsgque.lib .libs/ccmsgque.pdb: .libs/ccmsgque.dll

sdklibdir = $(libdir)
sdklib_DATA = .libs/ccmsgque.exp .libs/ccmsgque.lib

sdkbindir = $(bindir)
sdkbin_DATA = .libs/ccmsgque.dll .libs/ccmsgque.pdb

endif

