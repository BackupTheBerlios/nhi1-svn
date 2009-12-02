pkglib_LTLIBRARIES = phpmsgque.la

phpmsgque_la_SOURCES =  php_phpmsgque.h phpmsgque.c

# use this if you want to include the libmsgque files into the phpmsgque libraray
phpmsgque_la_CFLAGS = -I$(top_srcdir)/libmsgque $(PHP_INCLUDES) $(AM_CFLAGS) -DMQ_IGNORE_EXTERN
#phpmsgque_la_LIBADD = ../libmsgque/*.lo

phpmsgque_la_LDFLAGS = -module -avoid-version -no-undefined -export-dynamic -prefer-pic $(PHP_LDFLAGS)

phpdir = $(pkglibdir)
#php_DATA = .libs/pkgIndex.php

# we need this 'pkgIndex.tcl' for testing only
#.libs/php.ini: phpmsgque.la
#        $(TCLSH) $(srcdir)/.MAKE_INDEX.tcl

#noinst_SCRIPTS = pkgIndex.tcl

#CLEANFILES = .libs/php.ini
#EXTRA_DIST = .MAKE_INDEX.tcl

#html: ./libs/pkgIndex.tcl

