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

arg ?= ALL

clean-local:
	rm -fr thread
	rm -fr nothread
	rm -f *.log

thread: build_performance_env.sh
	$(SHELL) build_performance_env.sh thread

nothread: build_performance_env.sh
	$(SHELL) build_performance_env.sh nothread

perftest: performance.sh | thread nothread
	$(SHELL) performance.sh $(arg) 2>&1 | tee perf.log

all-local: performance.sh build_performance_env.sh
	@echo
	@echo "usage: make (thread|nothread|perftest) ?arg=...|ALL?"
	@echo
	@echo "  thread ..... build thread code"
	@echo "  nothread ... build no-thread code"
	@echo "  perftest ... make the performance test"
	@echo
