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
dnl Make sure that the comment is aligned:
[  --with-PhpMsgque             Include PhpMsgque support])

dnl Otherwise use enable:

if test "$PHP_PHPMSGQUE" != "no"; then
  dnl libmsgque sources
  EXTRA_LDFLAGS=../../libmsgque/liblocal.la

  dnl --with-PhpMsgque -> add include path
  PHPMSGQUE_DIR=../../libmsgque
  PHP_ADD_INCLUDE($PHPMSGQUE_DIR)
 
  dnl PHP_SUBST(PHPMSGQUE_SHARED_LIBADD)
  dnl add extension 
  PHP_NEW_EXTENSION(PhpMsgque, PhpMsgque.c MqS_php.c MqSException_php.c link_php.c config_php.c misc_php.c slave_php.c service_php.c send_php.c read_php.c MqBufferS_php.c, $ext_shared,,-DMQ_IGNORE_EXTERN)
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
