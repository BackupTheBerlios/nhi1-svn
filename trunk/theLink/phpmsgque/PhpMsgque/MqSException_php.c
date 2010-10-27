/**
 *  \file       theLink/phpmsgque/PhpMsgque/MqSException_php.c
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
#include "zend_exceptions.h"

static zend_class_entry *default_exception_ce;
static zend_class_entry *MqSExceptionC;

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/
  
/*
PHP_METHOD(PhpMsgque_MqSException, __construct)
{
}
*/

#define NUM_PROP    zend_read_property(default_exception_ce, getThis(), ID(num),     0 TSRMLS_CC)
#define CODE_PROP   zend_read_property(default_exception_ce, getThis(), ID(code),    0 TSRMLS_CC)
#define TXT_PROP    zend_read_property(default_exception_ce, getThis(), ID(message), 0 TSRMLS_CC)

#define DEFAULT_0_PARAMS if (zend_parse_parameters_none() == FAILURE) { return; }

static void _get_entry(zval *value, zval *return_value TSRMLS_DC) /* {{{ */
{
  *return_value = *value;
  zval_copy_ctor(return_value);
  INIT_PZVAL(return_value);
}

static
PHP_METHOD(MqSExceptionC,getNum)
{
  _get_entry(NUM_PROP, return_value TSRMLS_CC);
}

PHP_METHOD(MqSExceptionC,getTxt)
{
  _get_entry(TXT_PROP, return_value TSRMLS_CC);
}

inline static MQ_INT GetNumN(zval *this_ptr TSRMLS_DC)
{
  return VAL2INT(NUM_PROP);
}

inline static enum MqErrorE GetCodeN(zval *this_ptr TSRMLS_DC)
{
  return VAL2INT(CODE_PROP);
}

inline static MQ_CST GetTxtN(zval *this_ptr TSRMLS_DC)
{
  return VAL2CST(TXT_PROP);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqSException_Set) (struct MqS* mqctx, zval *ex TSRMLS_DC) {
  if (Z_TYPE_P(ex) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(ex), MqSExceptionC TSRMLS_CC)) {
      MqErrorSet (mqctx, GetNumN(ex TSRMLS_CC), GetCodeN(ex TSRMLS_CC), GetTxtN(ex TSRMLS_CC), NULL);
      return;
    } else if (instanceof_function(Z_OBJCE_P(ex), default_exception_ce TSRMLS_CC)) {
      zval *message = zend_read_property(default_exception_ce, ex, ID(message), 0 TSRMLS_CC);
      zval *trace = zend_read_property(default_exception_ce, ex, ID(trace), 0 TSRMLS_CC);
      MqErrorC(mqctx, "PHP-Error", -1, VAL2CST(message));
      return;
    }
  }
  zend_error(E_ERROR, "expect 'Exception' type as argument");
}

void NS(MqSException_Raise) (struct MqS* mqctx TSRMLS_DC) {
  zval *MqSExceptionO = NULL;
  MAKE_STD_ZVAL(MqSExceptionO);
  ErrorCheck(object_init_ex(MqSExceptionO, MqSExceptionC));
  zend_update_property_string (default_exception_ce, MqSExceptionO, ID(message),  MqErrorGetText  (mqctx) TSRMLS_CC);
  zend_update_property_long   (default_exception_ce, MqSExceptionO, ID(code),	  MqErrorGetCodeI (mqctx) TSRMLS_CC);
  zend_update_property_long   (default_exception_ce, MqSExceptionO, ID(num),	  MqErrorGetNumI  (mqctx) TSRMLS_CC);
  MqErrorReset(mqctx);
  zend_throw_exception_object(MqSExceptionO TSRMLS_CC);
  return;
error:
  zend_error(E_ERROR, "unable to raise MqSException");
}

ZEND_BEGIN_ARG_INFO_EX(PhpMsgque_no_arg, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry MqSException_functions[] = {
  PHP_ME(MqSExceptionC, getNum, PhpMsgque_no_arg, ZEND_ACC_PUBLIC)
  PHP_ME(MqSExceptionC, getTxt, PhpMsgque_no_arg, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};

void NS(MqSException_Init) (TSRMLS_D) {
  zend_class_entry me_ce;
  default_exception_ce = zend_exception_get_default(TSRMLS_C);

  // create class and make depend on "Exception"
  INIT_CLASS_ENTRY(me_ce,"MqSException", MqSException_functions);
  MqSExceptionC = zend_register_internal_class_ex(&me_ce, default_exception_ce, NULL TSRMLS_CC);

  // define additional properties ("message" and "code" is covered by "Exception")
  zend_declare_property_null(MqSExceptionC, ID(num), ZEND_ACC_PROTECTED TSRMLS_CC);
}

