#+
#:  \file       theLink/example/c/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

noinst_PROGRAMS = mulclient$(EXEEXT) mulserver$(EXEEXT) manfilter$(EXEEXT) \
		    testclient$(EXEEXT) testserver$(EXEEXT) MyClient$(EXEEXT) MyServer$(EXEEXT) \
			Filter1$(EXEEXT) Filter2$(EXEEXT) Filter3$(EXEEXT) Filter4$(EXEEXT) Filter5$(EXEEXT) \
			    Filter6$(EXEEXT) server$(EXEEXT) client$(EXEEXT)

#noinst_PROGRAMS = server$(EXEEXT) client$(EXEEXT)

AM_CFLAGS = -static -I$(srcdir)/../../libmsgque -DMQ_IGNORE_EXTERN
AM_LDFLAGS = -static
LDADD = ../../libmsgque/libmsgque.la

#noinst_PROGRAMS = server$(EXEEXT) client$(EXEEXT) Filter6$(EXEEXT)

#noinst_PROGRAMS = server$(EXEEXT) client$(EXEEXT)

client_SOURCES = client.c stat.c stat.h

./docs:
	mkdir -p docs

./docs/server.help: ./docs server$(EXEEXT)
	./server$(EXEEXT) --help 2>docs/server.help

./docs/client.help: ./docs client$(EXEEXT)
	./client$(EXEEXT) --help 2>docs/client.help

./docs/debug.out: ./docs client$(EXEEXT) server$(EXEEXT)
	./client$(EXEEXT) --num 1 --debug 9 @ ./server$(EXEEXT) 2>docs/debug.out

./docs/client.perf: ./docs client$(EXEEXT) server$(EXEEXT)
	./client$(EXEEXT) --all @ ./server$(EXEEXT) 2>docs/client.perf

HTMLFILES = ./docs/server.help ./docs/client.help ./docs/debug.out ./docs/client.perf

html-local: $(HTMLFILES)

CLEANFILES = $(HTMLFILES)

