##
#§  libmsgque - tests/Makefile.am
#§  
#§  (C) 2007 - LibMsgque - Project - Group
#§  ------------------------------------------------------
#§  EMail: aotto1968 at users.sourceforge.net
#§  ------------------------------------------------------
#§  this software has GPL permissions to copy
#§  please contact AUTHORS for additional information
#§

include list.mk

DX_DEPEND =	\
		doxygen.sh \
		$(top_srcdir)/libmsgque/*.c \
		$(top_srcdir)/libmsgque/*.h \
		$(top_srcdir)/javamsgque/*.c \
		$(top_srcdir)/javamsgque/*.h \
		$(top_srcdir)/javamsgque/*.java \
		$(top_srcdir)/pymsgque/*.c \
		$(top_srcdir)/pymsgque/*.h \
		$(top_srcdir)/tclmsgque/*.c \
		$(top_srcdir)/tclmsgque/*.h \
		$(top_srcdir)/ccmsgque/*.cc \
		$(top_srcdir)/ccmsgque/*.h \
		$(top_srcdir)/csmsgque/*.cs \
		$(top_srcdir)/acmds/*.c \
		$(top_srcdir)/acmds/*.h \
		$(top_srcdir)/acmds/aexec

html: $(DX_DEPEND) $(DX_DIST)
	${SHELL} doxygen.sh -usedot -release ALL

EXTRA_DIST = $(DX_DIST)

