#+
#:  \file       theLink/example/go/Makefile.am
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
## GO
##

EXTRA_DIST = \
    $(wildcard $(srcdir)/*.go) \
    src/mulserver/main.go \
    src/Filter5/main.go \
    src/Filter6/main.go \
    src/test1/main.go \
    src/manfilter/main.go \
    src/testserver/main.go \
    src/test2/main.go \
    src/server/main.go \
    src/MyServer/main.go \
    src/Filter4/main.go \
    src/Filter1/main.go \
    src/Filter3/main.go \
    src/testclient/main.go \
    src/mulclient/main.go \
    src/MyClient/main.go \
    src/Filter2/main.go \
    src/client/main.go
  
if USE_GO

GOPATH = $(abs_srcdir):$(abs_builddir)/../../gomsgque
export GOPATH

GOBIN = $(abs_builddir)
export GOBIN

GOMSGQUE = $(abs_builddir)/../../gomsgque/pkg/*/gomsgque.a

GO_COMP = $(GO) install -x -ldflags "-r '$(abs_builddir)/../../libmsgque/.libs'"

BUILT_SOURCES = server$(EXEEXT)
server$(EXEEXT): src/server/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += MyClient$(EXEEXT)
MyClient$(EXEEXT): src/MyClient/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += MyServer$(EXEEXT)
MyServer$(EXEEXT): src/MyServer/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += mulclient$(EXEEXT)
mulclient$(EXEEXT): src/mulclient/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += mulserver$(EXEEXT)
mulserver$(EXEEXT): src/mulserver/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += manfilter$(EXEEXT)
manfilter$(EXEEXT): src/manfilter/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += testserver$(EXEEXT)
testserver$(EXEEXT): src/testserver/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += testclient$(EXEEXT)
testclient$(EXEEXT): src/testclient/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += Filter1$(EXEEXT)
Filter1$(EXEEXT): src/Filter1/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += Filter2$(EXEEXT)
Filter2$(EXEEXT): src/Filter2/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += Filter3$(EXEEXT)
Filter3$(EXEEXT): src/Filter3/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += Filter4$(EXEEXT)
Filter4$(EXEEXT): src/Filter4/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += Filter5$(EXEEXT)
Filter5$(EXEEXT): src/Filter5/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

BUILT_SOURCES += Filter6$(EXEEXT)
Filter6$(EXEEXT): src/Filter6/main.go $(GOMSGQUE)
	$(GO_COMP) $(basename $@)

clean-local: 
	rm -f $(BUILT_SOURCES)

endif

##
## ==========================================================================
## Main
##
