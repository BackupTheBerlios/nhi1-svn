#+
#§  \file       theLink/acmds/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

$(builddir)/docs:
	mkdir -p docs

$(builddir)/docs/atool.help: $(builddir)/docs atool
	./atool 2>docs/atool.help

$(builddir)/docs/ajoin.help: $(builddir)/docs atool
	./atool join --help 2>docs/ajoin.help

$(builddir)/docs/acut.help: $(builddir)/docs atool
	./atool cut --help 2>docs/acut.help

$(builddir)/docs/asort.help: $(builddir)/docs atool
	./atool sort --help 2>docs/asort.help

$(builddir)/docs/asplit.help: $(builddir)/docs atool
	./atool split --help 2>docs/asplit.help

$(builddir)/docs/aexec.help: $(builddir)/docs $(srcdir)/aexec
	../tests/exec.env $(srcdir)/aexec --help 2>docs/aexec.help

$(builddir)/docs/msgque.help: $(builddir)/docs $(srcdir)/aexec
	../tests/exec.env $(srcdir)/aexec --help-msgque 2>docs/msgque.help

$(builddir)/docs/tclmsgque.help: $(builddir)/docs $(srcdir)/aexec
	../tests/exec.env $(srcdir)/aexec --help-tclmsgque 1>docs/tclmsgque.help

bin_PROGRAMS = atool
EXTRA_DIST = aexec

HTMLFILES = $(builddir)/docs/atool.help $(builddir)/docs/ajoin.help $(builddir)/docs/acut.help $(builddir)/docs/asort.help \
	      $(builddir)/docs/asplit.help $(builddir)/docs/aexec.help $(builddir)/docs/msgque.help $(builddir)/docs/tclmsgque.help

html: $(HTMLFILES)

CLEANFILES =

atool_SOURCES = atool.c ajoin.c acut.c asort.c asplit.c optionL.c optionL.h
atool_LDADD = -lm ../libmsgque/libtmp.la

AM_CFLAGS = -I$(srcdir)/../libmsgque -DMQ_IGNORE_EXTERN


