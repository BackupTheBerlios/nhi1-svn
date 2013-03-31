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
setup = cmd.exe /E:ON /V:ON /T:0E /C call setup.bat Net-PerlMsgque
make = cmd.exe /E:ON /V:ON /C call make.bat Net-PerlMsgque
else
setup = $(SHELL) setup.sh Net-PerlMsgque
make = $(MAKE) -C Net-PerlMsgque
endif

PERLMAKEFILE  =	Net-PerlMsgque/Makefile

$(abs_builddir)/Net-PerlMsgque/%: $(abs_srcdir)/Net-PerlMsgque/%
	if test -d $(dir $@) ; then true; else mkdir -p $(dir $@); fi
	if test "$(abs_builddir)" != "$(abs_srcdir)" ; then cp -r $< $@; else true; fi

$(PERLMAKEFILE): $(subst $(abs_srcdir), $(abs_builddir), $(wildcard $(abs_srcdir)/Net-PerlMsgque/*))
	$(setup)
	
all-local: $(PERLMAKEFILE) 
	$(make)

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

