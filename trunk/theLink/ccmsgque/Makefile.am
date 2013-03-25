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


noinst_LTLIBRARIES = libtmp.la

libtmp_la_SOURCES = buffer.cc cache.cc event.cc \
    log.cc send.cc swap.cc sys_com.cc \
    token.cc uds_io.cc bufferL.cc error.cc \
    generic_io.cc io.cc msgque.cc read.cc \
    sys.cc tcp_io.cc pipe_io.cc trans.cc slave.cc config.cc \
    service.cc factory.cc link.cc sql.cc \
    \
    ccmsgque.cc ccsend.cc ccmisc.cc ccservice.cc \
    \
    ccmsgque.h ccmsgque_private.h msgque.h debug.h main.h msgque_private.h sys_com.h 

libtmp_la_CXXFLAGS = -DMQ_COMPILE_AS_CC -DMQ_IGNORE_EXTERN -I$(srcdir)/../sqlite
libtmp_la_LDFLAGS = -no-undefined
libtmp_la_LIBADD = $(builddir)/../sqlite/libtmp.la $(WINLIBS)


# ordinary libraray

lib_LTLIBRARIES = libccmsgque.la

libccmsgque_la_SOURCES = buffer.cc cache.cc event.cc \
    log.cc send.cc swap.cc sys_com.cc \
    token.cc uds_io.cc bufferL.cc error.cc \
    generic_io.cc io.cc msgque.cc read.cc \
    sys.cc tcp_io.cc pipe_io.cc trans.cc slave.cc config.cc \
    service.cc factory.cc link.cc sql.cc \
    \
    ccmsgque.cc ccsend.cc ccmisc.cc ccservice.cc \
    \
    ccmsgque.h ccmsgque_private.h msgque.h debug.h main.h msgque_private.h sys_com.h 


libccmsgque_la_CXXFLAGS = -DMQ_COMPILE_AS_CC -DMQ_BUILD_LIBMSGQUE_DLL -I$(srcdir)/../sqlite
libccmsgque_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libccmsgque_la_LIBADD = $(builddir)/../sqlite/libtmp.la $(WINLIBS)

include_HEADERS = ccmsgque.h
