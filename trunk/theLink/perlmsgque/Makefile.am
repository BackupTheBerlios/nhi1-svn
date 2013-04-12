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

SUFFIXES =

if USE_WINSDK
setup = cd Net-PerlMsgque && $(SDK_EXEC)
make = cd Net-PerlMsgque && $(SDK_EXEC) nmake.exe
PERL_OPTS = 
else
setup = cd Net-PerlMsgque &&
make = $(MAKE) -C Net-PerlMsgque DESTDIR="$(DESTDIR)"
PERL_OPTS = CC='@CC@' LD='@CC@' AR='@AR@' DEFINE="-DPIC"
endif

PERLMAKEFILE  =	Net-PerlMsgque/Makefile

setup += @PERL_M@ Makefile.PL PREFIX="@prefix_m@" $(PERL_DEBUG) $(PERL_OPTS) \
  INC="-Wall -I. -I@abs_top_builddir_m@ -I@abs_top_srcdir_m@/theLink/libmsgque" \
     LIBS="-L@abs_top_builddir_m@/theLink/libmsgque/.libs -lmsgque"

$(abs_builddir)/Net-PerlMsgque:
	mkdir Net-PerlMsgque
 
$(abs_builddir)/Net-PerlMsgque/%: $(abs_srcdir)/Net-PerlMsgque/% | $(abs_builddir)/Net-PerlMsgque
	cp -r $< $@

$(PERLMAKEFILE): $(subst $(abs_srcdir), $(abs_builddir), $(wildcard $(abs_srcdir)/Net-PerlMsgque/*)) Makefile
	$(setup)

Net-PerlMsgque/blib/arch/auto/Net/PerlMsgque/PerlMsgque.dll: $(PERLMAKEFILE) ../libmsgque/debug.h ../libmsgque/msgque.h
	$(make)

all-local: Net-PerlMsgque/blib/arch/auto/Net/PerlMsgque/PerlMsgque.dll

clean-local:
	if test -f $(PERLMAKEFILE); then $(make) clean ; else true; fi

install-exec-local:
	if test -f $(PERLMAKEFILE); then $(make) install ; else true; fi

uninstall-local:
	rm -f $(DESTDIR)$(prefix)/man/man3/Net::PerlMsgque.3
	rm -fr $(DESTDIR)$(libexecdir)/perl5

mostlyclean-local:
	if test -f $(PERLMAKEFILE); then $(make) realclean ; else true; fi
	if test "$(abs_builddir)" != "$(abs_srcdir)" ; then chmod -R u+w Net-PerlMsgque && rm -fr Net-PerlMsgque; else true; fi

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

