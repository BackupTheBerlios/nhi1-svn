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

arg ?= ALL

DX_DEPEND =	\
		doxygen.sh \
		$(top_srcdir)/theLink/libmsgque/*.c \
		$(top_srcdir)/theLink/libmsgque/*.h \
		$(top_srcdir)/theLink/javamsgque/*.c \
		$(top_srcdir)/theLink/javamsgque/*.h \
		$(top_srcdir)/theLink/javamsgque/*.java \
		$(top_srcdir)/theLink/pymsgque/*.c \
		$(top_srcdir)/theLink/pymsgque/*.h \
		$(top_srcdir)/theLink/rubymsgque/*.c \
		$(top_srcdir)/theLink/rubymsgque/*.h \
		$(top_srcdir)/theLink/msgqueforphp/MsgqueForPhp/*.c \
		$(top_srcdir)/theLink/msgqueforphp/MsgqueForPhp/*.h \
		$(top_srcdir)/theLink/tclmsgque/*.c \
		$(top_srcdir)/theLink/tclmsgque/*.h \
		$(top_srcdir)/theLink/ccmsgque/*.cc \
		$(top_srcdir)/theLink/ccmsgque/*.h \
		$(top_srcdir)/theLink/csmsgque/*.cs \
		$(top_srcdir)/theLink/acmds/*.c \
		$(top_srcdir)/theLink/acmds/*.h \
		$(top_srcdir)/theLink/acmds/aexec

html-local: $(DX_DEPEND) $(DX_DIST)
	${SHELL} doxygen.sh -release $(arg)

help:
	@echo
	@echo usage: make html ?arg=ALL?
	@echo
	@echo the following targets are supported:
	@echo
	@cd $(srcdir); find * -mindepth 1 -name "Doxyfile*"
	@echo

EXTRA_DIST = $(DX_DIST)


