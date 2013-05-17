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

noinst_PROGRAMS = mulclient mulserver manfilter \
		    testclient testserver MyClient MyServer \
			Filter1 Filter2 Filter3 Filter4 Filter5 \
			    Filter6 server client

AM_CFLAGS = -static -I$(srcdir)/../../libmsgque -DMQ_IGNORE_EXTERN
AM_LDFLAGS = -static
LDADD = ../../libmsgque/libmsgque.la

client_SOURCES = client.c stat.c stat.h

./docs:
	mkdir -p docs

./docs/server.help: ./docs server
	./server --help 2>docs/server.help

./docs/client.help: ./docs client
	./client --help 2>docs/client.help

./docs/debug.out: ./docs client server
	./client --num 1 --debug 9 @ ./server 2>docs/debug.out

./docs/client.perf: ./docs client server
	./client --all @ ./server 2>docs/client.perf

HTMLFILES = ./docs/server.help ./docs/client.help ./docs/debug.out ./docs/client.perf

html-local: $(HTMLFILES)

CLEANFILES = $(HTMLFILES)

