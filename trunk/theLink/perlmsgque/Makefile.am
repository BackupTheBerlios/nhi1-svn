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

if WIN32
setup = cmd.exe /E:ON /V:ON /T:0E /C call Net-PerlMsgque/setup.bat
make = cmd.exe /E:ON /V:ON /C call Net-PerlMsgque/make.bat
else
setup = $(shell) Net-PerlMsgque/setup.sh
make = $(shell) Net-PerlMsgque/make.sh
endif

os_abs_top_builddir = $(shell $(CYGPATH_M) $(abs_top_builddir))
os_abs_srcdir = $(shell $(CYGPATH_M) $(abs_srcdir))
os_abs_builddir = $(shell $(CYGPATH_M) $(abs_builddir))
os_prefix = $(shell $(CYGPATH_M) $(prefix))
os_perl = $(shell $(CYGPATH_M) @PERL@)
os_builddir = $(shell $(CYGPATH_M) $(builddir))

PERLMAKEFILE  =	Net-PerlMsgque/Makefile
PERLCONFIG    =	  PREFIX="$(os_prefix)" \
		  INC="-Wall -I. -I$(os_abs_top_builddir) -I$(os_abs_srcdir)/../libmsgque" \
		  OBJECT="\$$(O_FILES) $(os_abs_builddir)/../libmsgque/.libs/libtmp*.o $(os_abs_builddir)/../sqlite/.libs/libtmp*.o"

if DEBUG
PERLCONFIG    += OPTIMIZE=-g
endif

PERLCONFIG    += DEFINE="-DMQ_IGNORE_EXTERN"

$(abs_builddir)/Net-PerlMsgque/%: $(abs_srcdir)/Net-PerlMsgque/%
	if test -d $(dir $@) ; then true; else mkdir -p $(dir $@); fi
	if test "$(abs_builddir)" != "$(abs_srcdir)" ; then cp -r $< $@; else true; fi


$(PERLMAKEFILE): $(subst $(abs_srcdir), $(abs_builddir), $(wildcard $(abs_srcdir)/Net-PerlMsgque/*)) Makefile.am
	$(setup) $(os_perl) $(os_prefix) $(os_abs_top_builddir) $(os_abs_srcdir) $(os_abs_builddir)
	
	
all-local: $(PERLMAKEFILE) 
	if test -f $(PERLMAKEFILE); then $(make); else true; fi

clean-local:
	if test -f $(PERLMAKEFILE); then $(make) clean; else true; fi

install-exec-local:
	$(MAKE) -C  $(builddir)/Net-PerlMsgque $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" install

uninstall-local:
	$(MAKE) -C  $(builddir)/Net-PerlMsgque $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" uninstall

distclean-local:
	if test "$(abs_builddir)" != "$(abs_srcdir)" ; then rm -fr Net-PerlMsgque; else true; fi

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
	Net-PerlMsgque/typemap \
	Net-PerlMsgque/compile.bat \
	Net-PerlMsgque/make.bat

