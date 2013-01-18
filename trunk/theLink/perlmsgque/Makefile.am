#+
#§  \file       theLink/perlmsgque/Makefile.am
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

if USE_PERL

if WIN32
MF = Makefile.PL.win
else
MF = Makefile.PL
endif

PERLMAKEFILE  =	Net-PerlMsgque/Makefile
PERLDEPEND    =	Net-PerlMsgque/$(MF) Net-PerlMsgque/typemap Net-PerlMsgque/lib/Net/PerlMsgque.pm
PERLCONFIG    =	  PREFIX="$(prefix)" \
		  INC="-Wall -I. -I$(abs_top_builddir) -I$(abs_srcdir)/../libmsgque" \
		  OBJECT="\$$(O_FILES) $(abs_builddir)/../libmsgque/.libs/libtmp*.o $(abs_builddir)/../sqlite/.libs/libtmp*.o"

if DEBUG
PERLCONFIG    += OPTIMIZE=-g
endif

PERLCONFIG    += DEFINE="-DMQ_IGNORE_EXTERN"

$(PERLMAKEFILE): $(PERLDEPEND) Makefile.am
	-test ! -d $(builddir)/Net-PerlMsgque && cp -pr $(srcdir)/Net-PerlMsgque $(builddir)/Net-PerlMsgque \
	   && chmod -R u+w $(builddir)/Net-PerlMsgque && touch $(builddir)/Net-PerlMsgque/.distclean
	cd $(builddir)/Net-PerlMsgque && @PERL@ $(MF) $(PERLCONFIG)

all-local: $(PERLMAKEFILE)
	cd $(builddir)/Net-PerlMsgque && $(MAKE) $(AM_MAKEFLAGS)

clean-local: $(PERLMAKEFILE)
	cd $(builddir)/Net-PerlMsgque && $(MAKE) $(AM_MAKEFLAGS) clean

install-exec-local:
	cd $(builddir)/Net-PerlMsgque && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" install

uninstall-local:
	rm -f $(DESTDIR)$(prefix)/lib/*/site_perl/*/*/Net/PerlMsgque.pm
	rm -f $(DESTDIR)$(prefix)/lib/*/site_perl/*/*/auto/Net/PerlMsgque/PerlMsgque.so
	rm -f $(DESTDIR)$(prefix)/lib/*/site_perl/*/*/auto/Net/PerlMsgque/PerlMsgque.bs
	rm -f $(DESTDIR)$(prefix)/lib/*/site_perl/*/*/auto/Net/PerlMsgque/autosplit.ix
	rm -f $(DESTDIR)$(prefix)/lib/*/site_perl/*/*/auto/Net/PerlMsgque/.packlist
	rm -f $(DESTDIR)$(prefix)/lib/*/*/*/perllocal.pod
	rm -f $(DESTDIR)$(prefix)/share/man/man3/Net::PerlMsgque.3

distclean-local:
	-test -f $(builddir)/Net-PerlMsgque/.distclean && rm -fr $(builddir)/Net-PerlMsgque

endif

EXTRA_DIST = \
	Net-PerlMsgque/ppport.h \
	Net-PerlMsgque/PerlMsgque.xs \
	Net-PerlMsgque/lib/Net/PerlMsgque.pm \
	Net-PerlMsgque/Makefile.PL \
	Net-PerlMsgque/Makefile.PL.win \
	Net-PerlMsgque/README \
	Net-PerlMsgque/t/Net-PerlMsgque.t \
	Net-PerlMsgque/Changes \
	Net-PerlMsgque/MANIFEST \
	Net-PerlMsgque/typemap

