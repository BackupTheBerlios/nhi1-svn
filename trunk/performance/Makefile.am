#+
#:  \file       performance/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

EXTRA_DIST = build_performance_env.sh.in performance.sh.in README

clean-local:
	rm -fr thread
	rm -fr nothread

thread: build_performance_env.sh
	$(SHELL) build_performance_env.sh thread

nothread: build_performance_env.sh
	$(SHELL) build_performance_env.sh nothread

perf: performance.sh thread nothread
	$(SHELL) performance.sh

