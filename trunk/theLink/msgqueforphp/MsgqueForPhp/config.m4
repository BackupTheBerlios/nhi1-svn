dnl
dnl  \file       theLink/msgqueforphp/MsgqueForPhp/config.m4
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

PHP_ARG_WITH(MsgqueForPhp, for MsgqueForPhp support,
[  --with-MsgqueForPhp[=DIR]  Include MsgqueForPhp support])
PHP_ARG_WITH(MsgqueForPhpSrc, for MsgqueForPhp source directory,
[  --with-MsgqueForPhpSrc[=DIR]  Include MsgqueForPhp source-directory])

dnl Otherwise use enable:

if test "$PHP_MSGQUEFORPHP" != "no"; then
  dnl libmsgque sources

  PHP_CHECK_64BIT([AC_MSG_ERROR([MsgqueForPHP require 64 Bit !!])], [])

  if test "$PHP_MSGQUEFORPHP" == "yes"; then
    PHP_MSGQUEFORPHP=../../libmsgque
  fi
  if test "$PHP_MSGQUEFORPHPSRC" == "no" -o "$PHP_MSGQUEFORPHPSRC" == "yes" ; then
    PHP_MSGQUEFORPHPSRC=../../libmsgque
  fi

  EXTRA_LDFLAGS=${PHP_MSGQUEFORPHP}/libtmp.la

  dnl --with-MsgqueForPhpSrc -> add include path
  PHP_ADD_INCLUDE($PHP_MSGQUEFORPHPSRC)
  PHP_ADD_INCLUDE(../../..)
  dnl PHP_ADD_LIBPATH($PHP_MSGQUEFORPHP,1)
  dnl PHP_ADD_LIBRARY(libtmp.la, 1, 1)
 
  dnl PHP_SUBST(MSGQUEFORPHP_SHARED_LIBADD)
  dnl add extension 
  PHP_NEW_EXTENSION(MsgqueForPhp, MsgqueForPhp.c MqS_php.c MqSException_php.c link_php.c config_php.c misc_php.c slave_php.c service_php.c send_php.c read_php.c MqBufferS_php.c error_php.c MqFactoryS_php.c MqDumpS_php.c storage_php.c, $ext_shared,,-DMQ_IGNORE_EXTERN)
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



