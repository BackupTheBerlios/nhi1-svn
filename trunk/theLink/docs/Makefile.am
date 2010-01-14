#+
#§  \file       theLink/docs/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

include list.mk

DX_DEPEND =	\
		doxygen.sh \
		$(top_srcdir)/theLink/libmsgque/*.c \
		$(top_srcdir)/theLink/libmsgque/*.h \
		$(top_srcdir)/theLink/javamsgque/*.c \
		$(top_srcdir)/theLink/javamsgque/*.h \
		$(top_srcdir)/theLink/javamsgque/*.java \
		$(top_srcdir)/theLink/pymsgque/*.c \
		$(top_srcdir)/theLink/pymsgque/*.h \
		$(top_srcdir)/theLink/tclmsgque/*.c \
		$(top_srcdir)/theLink/tclmsgque/*.h \
		$(top_srcdir)/theLink/ccmsgque/*.cc \
		$(top_srcdir)/theLink/ccmsgque/*.h \
		$(top_srcdir)/theLink/csmsgque/*.cs \
		$(top_srcdir)/theLink/acmds/*.c \
		$(top_srcdir)/theLink/acmds/*.h \
		$(top_srcdir)/theLink/acmds/aexec

html: $(DX_DEPEND) $(DX_DIST)
	${SHELL} doxygen.sh -release ALL

EXTRA_DIST = $(DX_DIST)

