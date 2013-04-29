#+
#:  \file       tests/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

EXTRA_DIST = $(srcdir)/*.test $(srcdir)/*.tcl

all-local: TestControl.bash

TESTS_ENVIRONMENT = ! grep -E -A 10 "(Failed\s+[1-9]|Test files exiting with errors)"
TESTS := $(patsubst $(srcdir)/%.test,%.res,$(wildcard $(srcdir)/*.test))

./%.res: $(srcdir)/%.test
	$(abs_top_srcdir)/bin/Nhi1Exec $< 

CLEANFILES = *.res *.uds *.txt *.dat *.pid

