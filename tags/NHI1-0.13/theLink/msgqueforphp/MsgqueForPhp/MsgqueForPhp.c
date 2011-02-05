/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/MsgqueForPhp.c
 *  \brief      \$Id: MsgqueForPhp.c 280 2010-11-08 19:22:52Z aotto1968 $
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 280 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_php.h"
#include "SAPI.h"

/* If you declare any globals in php_MsgqueForPhp.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(MsgqueForPhp)
*/

/* True global resources - no need for thread safety here */
//static int le_MsgqueForPhp;

PHP_FUNCTION(FactoryAdd);
PHP_FUNCTION(FactoryDefault);
PHP_FUNCTION(FactoryDefaultIdent);
PHP_FUNCTION(FactoryGet);
PHP_FUNCTION(FactoryGetCalled);

/* {{{ MsgqueForPhp_functions[]
 *
 * Every user visible function must have an entry in MsgqueForPhp_functions[].
 */
  
ZEND_BEGIN_ARG_INFO_EX(no_arg, 0, 0, 0)             
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(FactoryCreate_arg, 0, 0, 2)
  ZEND_ARG_INFO(0, "ident")
  ZEND_ARG_INFO(0, "class")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ident_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "ident")
ZEND_END_ARG_INFO()

const zend_function_entry NS(functions)[] = {
  PHP_FE(FactoryAdd,		    FactoryCreate_arg)
  PHP_FE(FactoryDefault,	    FactoryCreate_arg)
  PHP_FE(FactoryDefaultIdent,	    no_arg)
  PHP_FE(FactoryGet,		    ident_arg)
  PHP_FE(FactoryGetCalled,	    ident_arg)
  {NULL, NULL, NULL}						/* Must be the last line in MsgqueForPhp_functions[] */
};

/* }}} */

/* {{{ MsgqueForPhp_module_entry
 */
zend_module_entry MsgqueForPhp_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"MsgqueForPhp",
	NS(functions),
	PHP_MINIT(MsgqueForPhp),
	PHP_MSHUTDOWN(MsgqueForPhp),
	PHP_RINIT(MsgqueForPhp),	    /* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(MsgqueForPhp),	    /* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(MsgqueForPhp),
#if ZEND_MODULE_API_NO >= 20010901
	LIBMSGQUE_VERSION,		    /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MSGQUEFORPHP
ZEND_GET_MODULE(MsgqueForPhp)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("MsgqueForPhp.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_MsgqueForPhp_globals, MsgqueForPhp_globals)
    STD_PHP_INI_ENTRY("MsgqueForPhp.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_MsgqueForPhp_globals, MsgqueForPhp_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_MsgqueForPhp_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_MsgqueForPhp_init_globals(zend_MsgqueForPhp_globals *MsgqueForPhp_globals)
{
	MsgqueForPhp_globals->global_value = 0;
	MsgqueForPhp_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */

void NS(MqS_Init)	    (TSRMLS_D);
void NS(MqSException_Init)  (TSRMLS_D);
void NS(MqBufferS_Init)	    (TSRMLS_D);
void NS(MqFactoryS_Init)    (TSRMLS_D);

PHP_MINIT_FUNCTION(MsgqueForPhp)
{
  // we need the global variable $php_errormsg to act on errors
  //zend_alter_ini_entry(ID2(track_errors), ID(1), PHP_INI_SYSTEM, PHP_INI_STAGE_STARTUP);

  NS(MqS_Init)		(TSRMLS_C);
  NS(MqSException_Init)	(TSRMLS_C);
  NS(MqBufferS_Init)	(TSRMLS_C);
  NS(MqFactoryS_Init)	(TSRMLS_C);

  /* If you have INI entries, uncomment these lines 
  REGISTER_INI_ENTRIES();
  */

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(MsgqueForPhp)
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
PHP_RINIT_FUNCTION(MsgqueForPhp)
{
  zval **_SERVER;
  zval **SCRIPT_FILENAME;

  /* fetch the script name */
  zval *a0 = cfg_get_entry(ID(cfg_file_path) + 1);
  convert_to_string(a0);

  /* Fetch $_SERVER from the global scope */
  zend_hash_find(&EG(symbol_table), ID(_SERVER)+1, (void**)&_SERVER);

  /* FETCH $_SERVER['SCRIPT_FILENAME'] */
  if (SUCCESS == zend_hash_find(Z_ARRVAL_PP(_SERVER), ID(SCRIPT_FILENAME)+1, (void **) &SCRIPT_FILENAME)) {

    /* init libmsgque global data */
    if (MqInitGet() == NULL && a0 != NULL && Z_TYPE_P(a0) != IS_NULL) {
      struct MqBufferLS * initB = MqInitCreate();
      MqBufferLAppendC(initB, sapi_module.executable_location ? sapi_module.executable_location : "php");
      MqBufferLAppendC(initB, "-c");
      MqBufferLAppendC(initB, VAL2CST(a0));
      MqBufferLAppendC(initB, VAL2CST(*SCRIPT_FILENAME));
    } else {
      return FAILURE;
    }
  } else {
    return FAILURE;
  }
  return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(MsgqueForPhp)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(MsgqueForPhp)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "MsgqueForPhp support", "enabled");
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
/* {{{ proto string confirm_MsgqueForPhp_compiled(string arg)
   Return a string to confirm that the module is compiled in */
/*
PHP_FUNCTION(confirm_MsgqueForPhp_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "MsgqueForPhp", arg);
	RETURN_STRINGL(strg, len, 0);
}
*/
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





