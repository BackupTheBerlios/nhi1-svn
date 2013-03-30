#+
#:  \file       bin/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

EXTRA_DIST =  Nhi1Label rcBuild Nhi1BuildLanguage Nhi1Config Nhi1Exec Nhi1Docs

docs:
	mkdir -p docs

docs/%.help: $(srcdir)/% docs
	$< -h 1>$@ 2>&1;true

html-local: $(patsubst %,docs/%.help,$(EXTRA_DIST))

