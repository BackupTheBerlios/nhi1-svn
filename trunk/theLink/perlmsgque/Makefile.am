#+
#:  \file       theLink/perlmsgque/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

if USE_PERL

SUFFIXES =

if USE_WINSDK
setup = cd Net-PerlMsgque && $(SDK_EXEC)
make = cd Net-PerlMsgque && $(SDK_EXEC) nmake.exe
PERL_OPTS = 
PERL_OBJECT = OBJECT="\$$(O_FILES) @abs_top_builddir_m@/theLink/libmsgque/.libs/libmsgque.dll.lib"
PERL_DEFINE = DEFINE="-DPIC -DDLL_EXPORT"
else
setup = cd Net-PerlMsgque &&
make = $(MAKE) -C Net-PerlMsgque
PERL_OPTS = CC='@CC@' LD='@CC@' AR='@AR@'
PERL_OBJECT = OBJECT="\$$(O_FILES) @abs_top_builddir@/theLink/libmsgque/.libs/libtmp.a"
PERL_DEFINE = DEFINE="-DMQ_IGNORE_EXTERN"
endif

PERLMAKEFILE  =	Net-PerlMsgque/Makefile

setup += @PERL_M@ Makefile.PL PREFIX="@prefix_m@" $(PERL_DEBUG) $(PERL_OPTS) \
  INC="-Wall -I. -I@abs_top_builddir_m@ -I@abs_top_srcdir_m@/theLink/libmsgque" \
    $(PERL_OBJECT) $(PERL_DEFINE)

$(abs_builddir)/Net-PerlMsgque:
	mkdir Net-PerlMsgque

$(abs_builddir)/Net-PerlMsgque/%: $(abs_srcdir)/Net-PerlMsgque/% | $(abs_builddir)/Net-PerlMsgque
	if test "$(abs_builddir)" != "$(abs_srcdir)" ; then cp -r $< $@; else true; fi

$(PERLMAKEFILE): $(subst $(abs_srcdir), $(abs_builddir), $(wildcard $(abs_srcdir)/Net-PerlMsgque/*)) Makefile
	$(setup)

Net-PerlMsgque/PerlMsgque.obj: $(PERLMAKEFILE) $(srcdir)/../libmsgque/debug.h $(srcdir)/../libmsgque/msgque.h
	$(make)

all-local: Net-PerlMsgque/PerlMsgque.obj

clean-local:
	if test -f $(PERLMAKEFILE); then $(make) clean ; else true; fi

install-exec-local:
	if test -f $(PERLMAKEFILE); then $(make) DESTDIR="$(DESTDIR)" install ; else true; fi

uninstall-local:
	if test -f $(PERLMAKEFILE); then $(make) DESTDIR="$(DESTDIR)" uninstall ; else true; fi

distclean-local:
	if test "$(abs_builddir)" != "$(abs_srcdir)" ; then rm -fr Net-PerlMsgque; else true; fi

endif

EXTRA_DIST = \
	Net-PerlMsgque/ppport.h \
	Net-PerlMsgque/PerlMsgque.xs \
	Net-PerlMsgque/lib/Net/PerlMsgque.pm \
	Net-PerlMsgque/Makefile.PL \
	Net-PerlMsgque/README \
	Net-PerlMsgque/t/Net-PerlMsgque.t \
	Net-PerlMsgque/Changes \
	Net-PerlMsgque/MANIFEST \
	Net-PerlMsgque/typemap 

