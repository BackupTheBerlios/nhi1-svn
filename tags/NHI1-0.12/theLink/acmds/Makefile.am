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

docs/atool.help: atool
	./atool 2>docs/atool.help

docs/ajoin.help: atool
	./atool join --help 2>docs/ajoin.help

docs/acut.help: atool
	./atool cut --help 2>docs/acut.help

docs/asort.help: atool
	./atool sort --help 2>docs/asort.help

docs/asplit.help: atool
	./atool split --help 2>docs/asplit.help

docs/aexec.help: $(srcdir)/aexec
	$(srcdir)/aexec --help 2>docs/aexec.help

docs/msgque.help: $(srcdir)/aexec
	../tests/tcl.env $(srcdir)/aexec --help-msgque 2>docs/msgque.help

docs/tclmsgque.help: $(srcdir)/aexec
	../tests/tcl.env $(srcdir)/aexec --help-tclmsgque 1>docs/tclmsgque.help

bin_PROGRAMS = atool
EXTRA_DIST = aexec

HTMLFILES = docs/atool.help docs/ajoin.help docs/acut.help docs/asort.help \
	      docs/asplit.help docs/aexec.help docs/msgque.help docs/tclmsgque.help

html: $(HTMLFILES)

CLEANFILES =

atool_SOURCES = atool.c ajoin.c acut.c asort.c asplit.c optionL.c optionL.h
atool_LDADD = -lm ../libmsgque/libtmp.la

AM_CFLAGS = -I$(srcdir)/../libmsgque -DMQ_IGNORE_EXTERN


