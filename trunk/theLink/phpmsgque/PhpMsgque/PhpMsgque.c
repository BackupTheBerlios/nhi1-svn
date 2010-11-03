/**
 *  \file       theLink/phpmsgque/PhpMsgque/PhpMsgque.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_php.h"

/* If you declare any globals in php_PhpMsgque.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(PhpMsgque)
*/

/* True global resources - no need for thread safety here */
//static int le_PhpMsgque;

/* {{{ PhpMsgque_functions[]
 *
 * Every user visible function must have an entry in PhpMsgque_functions[].
 */
const zend_function_entry NS(functions)[] = {
	PHP_FE(confirm_PhpMsgque_compiled,	NULL)		/* For testing, remove later. */
	{NULL, NULL, NULL}								/* Must be the last line in PhpMsgque_functions[] */
};
/* }}} */

/* {{{ PhpMsgque_module_entry
 */
zend_module_entry PhpMsgque_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"PhpMsgque",
	NS(functions),
	PHP_MINIT(PhpMsgque),
	PHP_MSHUTDOWN(PhpMsgque),
	PHP_RINIT(PhpMsgque),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(PhpMsgque),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(PhpMsgque),
#if ZEND_MODULE_API_NO >= 20010901
	LIBMSGQUE_VERSION,			/* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PHPMSGQUE
ZEND_GET_MODULE(PhpMsgque)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("PhpMsgque.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_PhpMsgque_globals, PhpMsgque_globals)
    STD_PHP_INI_ENTRY("PhpMsgque.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_PhpMsgque_globals, PhpMsgque_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_PhpMsgque_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_PhpMsgque_init_globals(zend_PhpMsgque_globals *PhpMsgque_globals)
{
	PhpMsgque_globals->global_value = 0;
	PhpMsgque_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */

void NS(MqS_Init)			(TSRMLS_D);
void NS(MqSException_Init)  (TSRMLS_D);
void NS(MqBufferS_Init)	    (TSRMLS_D);

PHP_MINIT_FUNCTION(PhpMsgque)
{
  // we need the global variable $php_errormsg to act on errors
  zend_alter_ini_entry(ID2(track_errors), ID(1), PHP_INI_SYSTEM, PHP_INI_STAGE_STARTUP);


  NS(MqS_Init)			(TSRMLS_C);
  NS(MqSException_Init)	(TSRMLS_C);
  NS(MqBufferS_Init)	(TSRMLS_C);

  /* If you have INI entries, uncomment these lines 
  REGISTER_INI_ENTRIES();
  */
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(PhpMsgque)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(PhpMsgque)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(PhpMsgque)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(PhpMsgque)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PhpMsgque support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_PhpMsgque_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_PhpMsgque_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "PhpMsgque", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
