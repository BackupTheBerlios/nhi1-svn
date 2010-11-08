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
PHPCONFIG    =  --with-PhpMsgque=$(abs_builddir)/../libmsgque
PHPCONFIG    +=  --with-PhpMsgqueSrc=$(abs_srcdir)/../libmsgque

if DEBUG
PHPCONFIG    += --enable-symbols
endif

#PHPCONFIG    += --srcdir=$(srcdir)
PHPCONFIG    += --bindir=$(bindir)
PHPCONFIG    += --sbindir=$(sbindir)
PHPCONFIG    += --libexecdir=$(libexecdir)
PHPCONFIG    += --sysconfdir=$(sysconfdir)
PHPCONFIG    += --sharedstatedir=$(sharedstatedir)
PHPCONFIG    += --localstatedir=$(localstatedir)
PHPCONFIG    += --libdir=$(libdir)
PHPCONFIG    += --includedir=$(includedir)
PHPCONFIG    += --oldincludedir=$(oldincludedir)
PHPCONFIG    += --datarootdir=$(datarootdir)
PHPCONFIG    += --datadir=$(datadir)
PHPCONFIG    += --infodir=$(infodir)
PHPCONFIG    += --localedir=$(localedir)
PHPCONFIG    += --mandir=$(mandir)
PHPCONFIG    += --docdir=$(docdir)
PHPCONFIG    += --htmldir=$(htmldir)
PHPCONFIG    += --dvidir=$(dvidir)
PHPCONFIG    += --pdfdir=$(pdfdir)
PHPCONFIG    += --psdir=$(psdir)


PhpMsgque/configure: PhpMsgque/config.m4
	-test ! -d PhpMsgque && cp -pr $(srcdir)/PhpMsgque . && chmod -R u+w PhpMsgque && touch PhpMsgque/.distclean
	cd PhpMsgque && $(PHPIZE)

$(PHPMAKEFILE): PhpMsgque/configure Makefile.am
	cd PhpMsgque && ./configure $(PHPCONFIG)

all-local: $(PHPMAKEFILE)
	cd PhpMsgque && $(MAKE) $(AM_MAKEFLAGS)

clean-local: $(PHPMAKEFILE)
	cd PhpMsgque && $(MAKE) $(AM_MAKEFLAGS) clean

install-exec-local:
	cd PhpMsgque && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$(DESTDIR)" install

uninstall-local:

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
