#+
#:  \file       theGuard/tests/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

EXTRA_DIST = $(srcdir)/*.tcl 

TESTS_ENVIRONMENT = ! grep -E -A 10 "(Failed\s+[1-9]|Test files exiting with errors)"
TESTS := $(patsubst $(abs_top_srcdir)/theLink/tests/%.test,%.res,$(wildcard $(abs_top_srcdir)/theLink/tests/*.test))

if WIN32
MAIN_ARGS=--only-pipe-tcp
else
MAIN_ARGS=--only-pipe-uds
endif

./%.res: $(abs_top_srcdir)/theLink/tests/%.test
	$(abs_top_srcdir)/bin/Nhi1Exec $< --only-binary --max 50 $(MAIN_ARGS) $(MY_TEST_ARGS)

CLEANFILES = *.res *.uds *.txt *.dat *.pid

