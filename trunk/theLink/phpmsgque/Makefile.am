#+
#§  \file       theLink/phpmsgque/Makefile.am
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

PHPMAKEFILE  =	PhpMsgque/Makefile
PHPCONFIGURE    =  --with-PhpMsgque=$(abs_builddir)/../libmsgque
PHPCONFIGURE    +=  --with-PhpMsgqueSrc=$(abs_srcdir)/../libmsgque
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


PhpMsgque/configure: PhpMsgque/config.m4
	-test ! -d PhpMsgque && cp -pr $(srcdir)/PhpMsgque . && chmod -R u+w PhpMsgque && touch PhpMsgque/.distclean
	cd PhpMsgque && $(PHPIZE)

$(PHPMAKEFILE): PhpMsgque/configure Makefile.am
	cd PhpMsgque && ./configure $(PHPCONFIGURE)

all-local: $(PHPMAKEFILE)
	cd PhpMsgque && $(MAKE) $(AM_MAKEFLAGS)

clean-local: $(PHPMAKEFILE)
	cd PhpMsgque && $(MAKE) $(AM_MAKEFLAGS) clean

#cd PhpMsgque && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" install
install-exec-local:
	cp PhpMsgque/modules/* "$(DESTDIR)/$(pkglibdir)"
	echo "extension=\"$(pkglibdir)/PhpMsgque.so\"" > "$(DESTDIR)/$(pkglibdir)/php.ini"

uninstall-local:
	rm -f "$(DESTDIR)/$(pkglibdir)"/PhpMsgque.*
	rm -f "$(DESTDIR)/$(pkglibdir)/php.ini"

distclean-local:
	-test -f PhpMsgque/.distclean && rm -fr PhpMsgque

endif

EXTRA_DIST = \
	PhpMsgque/.cvsignore \
	PhpMsgque/config.m4 \
	PhpMsgque/config.w32 \
	PhpMsgque/CREDITS \
	PhpMsgque/PhpMsgque.php \
	PhpMsgque/tests/001.phpt \
	PhpMsgque/msgque_php.h \
	PhpMsgque/php_PhpMsgque.h \
	PhpMsgque/PhpMsgque.c \
	PhpMsgque/MqS_php.c \
	PhpMsgque/MqBufferS_php.c \
	PhpMsgque/MqSException_php.c \
	PhpMsgque/config_php.c \
	PhpMsgque/link_php.c \
	PhpMsgque/read_php.c \
	PhpMsgque/service_php.c \
	PhpMsgque/error_php.c \
	PhpMsgque/misc_php.c \
	PhpMsgque/send_php.c \
	PhpMsgque/slave_php.c


