#+
#§  \file       theLink/msgqueforphp/Makefile.am
#§  \brief      \$Id: Makefile.am 206 2010-02-17 14:21:42Z aotto1968 $
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 206 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

if USE_PHP

PHPMAKEFILE  =	MsgqueForPhp/Makefile
PHPCONFIGURE    =  --with-MsgqueForPhp=$(abs_builddir)/../libmsgque
PHPCONFIGURE    +=  --with-MsgqueForPhpSrc=$(abs_srcdir)/../libmsgque
PHPCONFIGURE    += --with-php-config=$(PHPCONFIG)

if DEBUG
PHPCONFIGURE    += --enable-symbols
endif

#PHPCONFIGURE    += --srcdir=$(srcdir)
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


MsgqueForPhp/configure: MsgqueForPhp/config.m4
	-test ! -d MsgqueForPhp && cp -pr $(srcdir)/MsgqueForPhp . && chmod -R u+w MsgqueForPhp && touch MsgqueForPhp/.distclean
	cd MsgqueForPhp && $(PHPIZE)

$(PHPMAKEFILE): MsgqueForPhp/configure Makefile.am
	cd MsgqueForPhp && ./configure $(PHPCONFIGURE)

all-local: $(PHPMAKEFILE)
	cd MsgqueForPhp && $(MAKE) $(AM_MAKEFLAGS)

clean-local: $(PHPMAKEFILE)
	cd MsgqueForPhp && $(MAKE) $(AM_MAKEFLAGS) clean

#cd MsgqueForPhp && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" install
install-exec-local:
	cp MsgqueForPhp/modules/* "$(DESTDIR)/$(pkglibdir)"
	echo "extension=\"$(pkglibdir)/MsgqueForPhp.so\"" > "$(DESTDIR)/$(pkglibdir)/php.ini"

uninstall-local:
	rm -f "$(DESTDIR)/$(pkglibdir)"/MsgqueForPhp.*
	rm -f "$(DESTDIR)/$(pkglibdir)/php.ini"

distclean-local:
	-test -f MsgqueForPhp/.distclean && rm -fr MsgqueForPhp

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




