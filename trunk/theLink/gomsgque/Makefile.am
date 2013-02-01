#+
#:  \file       theLink/gomsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

if USE_GO

#####################################################################################
## GO files

GOPATH = $(abs_builddir)
export GOPATH

GOBIN = $(abs_builddir)
export GOBIN


GO_FILES =  src/gomsgque/MqS.go src/gomsgque/MqSException.go src/gomsgque/MqBufferS.go \
	    src/gomsgque/MqBinary.go src/gomsgque/MqDumpS.go src/gomsgque/send.go \
	    src/gomsgque/read.go src/gomsgque/link.go src/gomsgque/config.go \
	    src/gomsgque/service.go src/gomsgque/slave.go src/gomsgque/storage.go \
	    src/gomsgque/MqFactoryS.go

C_FILES =   src/gomsgque/gomsgque.h src/gomsgque/gomsgque.c

#####################################################################################
## GO files

$(abs_builddir)/src:
	if test ! -d src ; then cp -pr $(srcdir)/src . && chmod -R u+w src && touch src/.distclean; else true; fi

$(abs_builddir)/src/gomsgque/%: $(abs_srcdir)/src/gomsgque/%
	cp $< $@

all-local: $(abs_builddir)/src $(addprefix $(abs_builddir)/, $(GO_FILES))
	go install gomsgque

clean-local: 
	go clean gomsgque

distclean-local:
	if test -f src/.distclean ; then rm -fr src; else true; fi

endif

EXTRA_DIST = $(GO_FILES) $(C_FILES)


