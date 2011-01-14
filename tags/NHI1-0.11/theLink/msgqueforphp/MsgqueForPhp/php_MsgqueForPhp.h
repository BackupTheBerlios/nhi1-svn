/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/php_MsgqueForPhp.h
 *  \brief      \$Id: php_MsgqueForPhp.h 258 2010-10-26 19:38:14Z aotto1968 $
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 258 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef PHP_MSGQUEFORPHP_H
#define PHP_MSGQUEFORPHP_H

extern zend_module_entry MsgqueForPhp_module_entry;
#define phpext_MsgqueForPhp_ptr &MsgqueForPhp_module_entry

#ifdef PHP_WIN32
#	define PHP_MSGQUEFORPHP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_MSGQUEFORPHP_API __attribute__ ((visibility("default")))
#else
#	define PHP_MSGQUEFORPHP_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(MsgqueForPhp);
PHP_MSHUTDOWN_FUNCTION(MsgqueForPhp);
PHP_RINIT_FUNCTION(MsgqueForPhp);
PHP_RSHUTDOWN_FUNCTION(MsgqueForPhp);
PHP_MINFO_FUNCTION(MsgqueForPhp);

PHP_FUNCTION(confirm_MsgqueForPhp_compiled);	/* For testing, remove later. */

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(MsgqueForPhp)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(MsgqueForPhp)
*/

/* In every utility function you add that needs to use variables 
   in php_MsgqueForPhp_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as MSGQUEFORPHP_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MSGQUEFORPHP_G(v) TSRMG(MsgqueForPhp_globals_id, zend_MsgqueForPhp_globals *, v)
#else
#define MSGQUEFORPHP_G(v) (MsgqueForPhp_globals.v)
#endif

#endif	/* PHP_MSGQUEFORPHP_H */




