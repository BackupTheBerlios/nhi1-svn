#+
#:  \file       theLink/tclmsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

pkglib_LTLIBRARIES = tclmsgque.la

tclmsgque_la_SOURCES =	MqS_tcl.c  misc_tcl.c  msgque_tcl.c read_tcl.c send_tcl.c \
			  config_tcl.c service_tcl.c msgque_tcl.h slave_tcl.c \
			    MqBufferS_tcl.c error_tcl.c link_tcl.c MqFactoryS_tcl.c \
			      MqDumpS_tcl.c

# use this if you want to include the libmsgque files into the tclmsgque libraray
tclmsgque_la_CFLAGS = -shared -I$(srcdir)/../libmsgque $(TCL_INCLUDE_SPEC) $(TCL_CFLAGS) -DMQ_IGNORE_EXTERN
tclmsgque_la_LDFLAGS = -shared -module -avoid-version -no-undefined $(TCL_LIBADD)
tclmsgque_la_LIBADD = ../libmsgque/libmsgque.la

tcldir = $(pkglibdir)
tcl_DATA = .libs/pkgIndex.tcl

# we need this 'pkgIndex.tcl' for testing only
.libs/pkgIndex.tcl: tclmsgque.la
	$(TCLSH) "$$($(CYGPATH_W) "$(srcdir)/.MAKE_INDEX.tcl")"

#noinst_SCRIPTS = pkgIndex.tcl

CLEANFILES = .libs/pkgIndex.tcl
EXTRA_DIST = .MAKE_INDEX.tcl

html-local: .libs/pkgIndex.tcl
