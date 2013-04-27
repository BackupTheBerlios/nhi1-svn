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

TESTS_ENVIRONMENT = egrep -A 10 "(Failed\s+[1-9]|Test files exiting with errors)"

$(builddir)/%.res: $(srcdir)/%.test
	echo $(abs_top_srcdir)/bin/Nhi1Exec $< --full-testing $(MY_TEST_ARGS)

TESTS := $(abs_patsubst $(srcdir)/%.test,$(builddir)/%.res,$(wildcard $(srcdir)/*.test))

CLEANFILES = *.res *.uds *.txt *.dat *.pid

