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
PHPCONFIG    =  --with-PhpMsgque

if DEBUG
PHPCONFIG    += --enable-symbols
endif

PhpMsgque/configure: PhpMsgque/config.m4
	cd PhpMsgque && $(PHPIZE)

$(PHPMAKEFILE): PhpMsgque/configure
	-test ! -d PhpMsgque && cp -pr $(srcdir)/PhpMsgque . && chmod -R u+w PhpMsgque && touch PhpMsgque/.distclean
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
	PhpMsgque/MqS_php.c \
	PhpMsgque/msgque_php.h \
	PhpMsgque/config.w32 \
	PhpMsgque/PhpMsgque.php \
	PhpMsgque/tests \
	PhpMsgque/tests/001.phpt \
	PhpMsgque/config.m4 \
	PhpMsgque/php_PhpMsgque.h \
	PhpMsgque/PhpMsgque.c \
	PhpMsgque/.cvsignore \
	PhpMsgque/CREDITS \
	PhpMsgque/EXPERIMENTAL

