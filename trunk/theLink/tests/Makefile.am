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

##
## ==========================================================================
## Main
##

EXTRA_DIST = README.tests massclient.bash $(wildcard $(srcdir)/*.tcl) $(wildcard $(srcdir)/*.test)

TESTS_ENVIRONMENT = PATH="$(path)" LD_LIBRARY_PATH="$(ld_library_path)" $(TCLSH)
#TESTS = $(srcdir)/all.tcl
TESTS = $(srcdir)/int.test

