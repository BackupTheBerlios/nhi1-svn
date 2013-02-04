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

EXTRA_DIST = src/*/*.go
