#+
#:  \file       performance/Makefile.am
#:  \brief      \$Id: Makefile.am 609 2013-02-07 16:12:35Z aotto1968 $
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev: 609 $
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

EXTRA_DIST = $(srcdir)/*.test $(srcdir)/*.tcl

clean-local:
	rm -fr thread
	rm -fr nothread

TESTS_ENVIRONMENT = $(abs_top_srcdir)/bin/Nhi1Test

TESTS = all.tcl

