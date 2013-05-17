#+
#:  \file       lxc/Makefile.am
#:  \brief      \$Id: Makefile.am 685 2013-04-23 11:43:18Z aotto1968 $
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev: 685 $
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

EXTRA_DIST = lxc-nhi1  lxc-watchdog

docs:
	mkdir -p docs

docs/%.help: $(srcdir)/% | docs
	$< -h 1>$@ 2>&1;true

html-local: $(patsubst lxc-%,docs/lxc-%.help,$(EXTRA_DIST))

