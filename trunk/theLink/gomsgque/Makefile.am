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

EXTRA_DIST =  src/gomsgque/MqS.go src/gomsgque/MqSException.go src/gomsgque/MqBufferS.go \
	      src/gomsgque/MqBinary.go src/gomsgque/MqDumpS.go src/gomsgque/send.go \
	      src/gomsgque/read.go src/gomsgque/link.go src/gomsgque/config.go \
	      src/gomsgque/service.go src/gomsgque/slave.go src/gomsgque/storage.go \
	      src/gomsgque/MqFactoryS.go \
	      src/gomsgque/gomsgque.h src/gomsgque/gomsgque.c

if USE_GO

#####################################################################################
## GO files

GOPATH = $(abs_builddir)
export GOPATH

GOBIN = $(abs_builddir)
export GOBIN

#####################################################################################
## GO files

$(abs_builddir)/src/gomsgque/%: $(abs_srcdir)/src/gomsgque/%
	if test -d $(dir $@) ; then true; else mkdir -p $(dir $@); fi
	cp $< $@

all-local: $(addprefix $(abs_builddir)/, $(EXTRA_DIST))
	$(GO) install gomsgque

clean-local: 
	$(GO) clean
	rm -fr pkg

distclean-local:
	if test "$(abs_builddir)" != "$(abs_srcdir)" ; then rm -fr src; else true; fi

endif

