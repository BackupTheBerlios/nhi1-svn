/**
 *  \file       theLink/phpmsgque/PhpMsgque/php_PhpMsgque.h
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef PHP_PHPMSGQUE_H
#define PHP_PHPMSGQUE_H

extern zend_module_entry PhpMsgque_module_entry;
#define phpext_PhpMsgque_ptr &PhpMsgque_module_entry

#ifdef PHP_WIN32
#	define PHP_PHPMSGQUE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PHPMSGQUE_API __attribute__ ((visibility("default")))
#else
#	define PHP_PHPMSGQUE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(PhpMsgque);
PHP_MSHUTDOWN_FUNCTION(PhpMsgque);
PHP_RINIT_FUNCTION(PhpMsgque);
PHP_RSHUTDOWN_FUNCTION(PhpMsgque);
PHP_MINFO_FUNCTION(PhpMsgque);

PHP_FUNCTION(confirm_PhpMsgque_compiled);	/* For testing, remove later. */

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(PhpMsgque)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(PhpMsgque)
*/

/* In every utility function you add that needs to use variables 
   in php_PhpMsgque_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as PHPMSGQUE_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define PHPMSGQUE_G(v) TSRMG(PhpMsgque_globals_id, zend_PhpMsgque_globals *, v)
#else
#define PHPMSGQUE_G(v) (PhpMsgque_globals.v)
#endif

#endif	/* PHP_PHPMSGQUE_H */

