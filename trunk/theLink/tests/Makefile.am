#+
#:  \file       theLink/tests/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

EXTRA_DIST = README.tests massclient.bash $(srcdir)/*.tcl $(srcdir)/*.test

TESTS_ENVIRONMENT = ! grep -E -A 10 "(Failed\s+[1-9]|Test files exiting with errors)"
TESTS := $(patsubst $(srcdir)/%.test,%.res,$(wildcard $(srcdir)/*.test))

if WIN32
MAIN_ARGS=--only-pipe-tcp
else
MAIN_ARGS=--only-pipe-uds
endif

./%.res: $(srcdir)/%.test
	$(abs_top_srcdir)/bin/Nhi1Exec $< --only-binary --max 50 $(MAIN_ARGS) $(MY_TEST_ARGS)

CLEANFILES = *.res *.uds *.txt *.dat *.pid

