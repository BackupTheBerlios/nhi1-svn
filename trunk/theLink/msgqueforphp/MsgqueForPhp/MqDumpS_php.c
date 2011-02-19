/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/MqDumpS_php.c
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_php.h"

zend_class_entry *NS(MqDumpS);

#define SETUP_dump    struct MqDumpS* dump = VAL2MqDumpSelf(getThis());

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

static
PHP_METHOD(MsgqueForPhp_MqDumpS, Size)
{
  SETUP_dump;
  INT2VAL(return_value, MqDumpSize(dump));
}

static
PHP_METHOD(MsgqueForPhp_MqDumpS, __construct)
{
  RETURN_ERROR("it is not allowed to create an instance of 'MqDumpS'.");
}

static
PHP_METHOD(MsgqueForPhp_MqDumpS, __destruct)
{
  SETUP_dump;
  MqSysFree(dump);
}

ZEND_BEGIN_ARG_INFO_EX(no_arg, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry NS(MqDumpS_functions)[] = {
  PHP_ME(MsgqueForPhp_MqDumpS, __construct, NULL,   ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
  PHP_ME(MsgqueForPhp_MqDumpS, __destruct,  NULL,   ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
  PHP_ME(MsgqueForPhp_MqDumpS, Size,	    no_arg, ZEND_ACC_PUBLIC)

  {NULL, NULL, NULL}
};

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqDumpS_New) (zval *return_value, struct MqDumpS* dump TSRMLS_DC)
{
  // convert to an object instance
  if (object_init_ex(return_value, NS(MqDumpS)) == FAILURE) {
    RETURN_ERROR("unable to create an 'MqDumpS' instance.");
  }
  // link 'dump' with the object instance
  add_property_resource_ex(return_value, ID2(__dump), (long) dump TSRMLS_CC);
  return;
}

void NS(MqDumpS_Init) (TSRMLS_D) {
  zend_class_entry me_ce;

  // create class and make depend on "Exception"
  INIT_CLASS_ENTRY(me_ce,"MqDumpS", NS(MqDumpS_functions));
  NS(MqDumpS) = zend_register_internal_class(&me_ce TSRMLS_CC);
}

