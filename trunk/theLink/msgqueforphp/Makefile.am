#+
#:  \file       theLink/msgqueforphp/Makefile.am
#:  \brief      \$Id$
#:  
#:  (C) 2007 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

if USE_PHP

PHPMAKEFILE  =	MsgqueForPhp/Makefile
PHPCONFIGURE    =  --with-MsgqueForPhp=$(abs_builddir)/../libmsgque
PHPCONFIGURE    += --with-MsgqueForPhpSrc=$(abs_srcdir)/../libmsgque
PHPCONFIGURE    += --with-php-config=$(PHPCONFIG)

if DEBUG
PHPCONFIGURE    += --enable-symbols
endif

#PHPCONFIGURE   += --srcdir=$(srcdir)
PHPCONFIGURE    += --bindir=$(bindir)
PHPCONFIGURE    += --sbindir=$(sbindir)
PHPCONFIGURE    += --libexecdir=$(libexecdir)
PHPCONFIGURE    += --sysconfdir=$(sysconfdir)
PHPCONFIGURE    += --sharedstatedir=$(sharedstatedir)
PHPCONFIGURE    += --localstatedir=$(localstatedir)
PHPCONFIGURE    += --libdir=$(libdir)
PHPCONFIGURE    += --includedir=$(includedir)
PHPCONFIGURE    += --oldincludedir=$(oldincludedir)
PHPCONFIGURE    += --datarootdir=$(datarootdir)
PHPCONFIGURE    += --datadir=$(datadir)
PHPCONFIGURE    += --infodir=$(infodir)
PHPCONFIGURE    += --localedir=$(localedir)
PHPCONFIGURE    += --mandir=$(mandir)
PHPCONFIGURE    += --docdir=$(docdir)
PHPCONFIGURE    += --htmldir=$(htmldir)
PHPCONFIGURE    += --dvidir=$(dvidir)
PHPCONFIGURE    += --pdfdir=$(pdfdir)
PHPCONFIGURE    += --psdir=$(psdir)

$(abs_builddir)/MsgqueForPhp/configure: 
	-test ! -d MsgqueForPhp && cp -pr $(srcdir)/MsgqueForPhp . && chmod -R u+w MsgqueForPhp && touch MsgqueForPhp/.distclean
	cd MsgqueForPhp && $(PHPIZE)

$(abs_builddir)/MsgqueForPhp/%.c: $(abs_srcdir)/MsgqueForPhp/%.c
	cp $< $@

$(abs_builddir)/MsgqueForPhp/%.h: $(abs_srcdir)/MsgqueForPhp/%.h
	cp $< $@

$(abs_builddir)/$(PHPMAKEFILE): $(abs_builddir)/MsgqueForPhp/configure Makefile.am
	cd MsgqueForPhp && ./configure $(PHPCONFIGURE)

all-local: $(abs_builddir)/$(PHPMAKEFILE) $(wildcard $(abs_builddir)/MsgqueForPhp/*.[ch])
	$(MAKE) -C MsgqueForPhp $(AM_MAKEFLAGS)

clean-local:
	if test -f $(PHPMAKEFILE) ; then $(MAKE) -C MsgqueForPhp $(AM_MAKEFLAGS) clean ; else true; fi

#cd MsgqueForPhp && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" install
install-exec-local:
	cp MsgqueForPhp/modules/* "$(DESTDIR)/$(pkglibdir)"
	echo "extension=\"$(pkglibdir)/MsgqueForPhp.so\"" > "$(DESTDIR)/$(pkglibdir)/php.ini"

uninstall-local:
	if test -f $(PHPMAKEFILE) ; then $(MAKE) -C MsgqueForPhp $(AM_MAKEFLAGS) uninstall; else true; fi
	rm -f "$(DESTDIR)/$(pkglibdir)"/MsgqueForPhp.*
	rm -f "$(DESTDIR)/$(pkglibdir)/php.ini"

distclean-local:
	if test -f $(PHPMAKEFILE) ; then $(MAKE) -C MsgqueForPhp $(AM_MAKEFLAGS) distclean; else true; fi
	if test -f MsgqueForPhp/.distclean ; then rm -fr MsgqueForPhp; else true; fi

endif

EXTRA_DIST = \
	MsgqueForPhp/.cvsignore \
	MsgqueForPhp/config.m4 \
	MsgqueForPhp/config.w32 \
	MsgqueForPhp/CREDITS \
	MsgqueForPhp/MsgqueForPhp.php \
	MsgqueForPhp/tests/001.phpt \
	MsgqueForPhp/msgque_php.h \
	MsgqueForPhp/php_MsgqueForPhp.h \
	MsgqueForPhp/MsgqueForPhp.c \
	MsgqueForPhp/MqS_php.c \
	MsgqueForPhp/MqBufferS_php.c \
	MsgqueForPhp/MqDumpS_php.c \
	MsgqueForPhp/MqSException_php.c \
	MsgqueForPhp/config_php.c \
	MsgqueForPhp/link_php.c \
	MsgqueForPhp/read_php.c \
	MsgqueForPhp/service_php.c \
	MsgqueForPhp/error_php.c \
	MsgqueForPhp/misc_php.c \
	MsgqueForPhp/send_php.c \
	MsgqueForPhp/storage_php.c \
	MsgqueForPhp/MqFactoryS_php.c \
	MsgqueForPhp/slave_php.c

