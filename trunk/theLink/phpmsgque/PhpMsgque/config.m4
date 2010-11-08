dnl
dnl  \file       theLink/phpmsgque/PhpMsgque/config.m4
dnl  \brief      \$Id$
dnl  
dnl  (C) 2010 - NHI - #1 - Project - Group
dnl  
dnl  \version    \$Rev$
dnl  \author     EMail: aotto1968 at users.berlios.de
dnl  \attention  this software has GPL permissions to copy
dnl              please contact AUTHORS for additional information
dnl

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(PhpMsgque, for PhpMsgque support,
[  --with-PhpMsgque[=DIR]  Include PhpMsgque support])
PHP_ARG_WITH(PhpMsgqueSrc, for PhpMsgque source directory,
[  --with-PhpMsgqueSrc[=DIR]  Include PhpMsgque source-directory])

dnl Otherwise use enable:

if test "$PHP_PHPMSGQUE" != "no"; then
  dnl libmsgque sources

  if test "$PHP_PHPMSGQUE" == "yes"; then
    PHP_PHPMSGQUE=../../libmsgque
  fi
  if test "$PHP_PHPMSGQUESRC" == "no" -o "$PHP_PHPMSGQUESRC" == "yes" ; then
    PHP_PHPMSGQUESRC=../../libmsgque
  fi

  EXTRA_LDFLAGS=${PHP_PHPMSGQUE}/liblocal.la

  dnl --with-PhpMsgqueSrc -> add include path
  PHP_ADD_INCLUDE($PHP_PHPMSGQUESRC)
 
  dnl PHP_SUBST(PHPMSGQUE_SHARED_LIBADD)
  dnl add extension 
  PHP_NEW_EXTENSION(PhpMsgque, PhpMsgque.c MqS_php.c MqSException_php.c link_php.c config_php.c misc_php.c slave_php.c service_php.c send_php.c read_php.c MqBufferS_php.c error_php.c, $ext_shared,,-DMQ_IGNORE_EXTERN)
fi

AC_MSG_CHECKING([for build with symbols])
AC_ARG_ENABLE(symbols,
    AC_HELP_STRING([--enable-symbols], [build with debugging support]),
    enable_symbols=yes, enable_symbols=no
)
if test "x$enable_symbols" = "xyes"; then
  CFLAGS="-g $CFLAGS"
  CPPFLAGS="-D_DEBUG $CPPFLAGS"
fi
AC_MSG_RESULT($symbol)
