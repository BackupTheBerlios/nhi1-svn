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
  int ret;
  ALLOC_HASHTABLE(ht);
  ret = zend_hash_init(ht, 3, NULL, NULL, 1);
  if (ret == FAILURE) ...
  zend_hash_add(fooHashTbl, "num", sizeof("txt"), &barZval, sizeof(zval*), NULL);
  zend_hash_add(fooHashTbl, "code", sizeof("num"), &barZval, sizeof(zval*), NULL);
  zend_hash_add(fooHashTbl, "txt", sizeof("txt"), &barZval, sizeof(zval*), NULL);

  object_and_properties_init(return_value,MqSExceptionC,);

  rb_ivar_set(self, id_num,  num);
  rb_ivar_set(self, id_code, code);
  rb_call_super(1, &txt);
  return self;
}
*/

#define NUM_PROP    zend_read_property(default_exception_ce, getThis(), "num",     sizeof("num")-1,     0 TSRMLS_CC)
#define CODE_PROP   zend_read_property(default_exception_ce, getThis(), "code",    sizeof("code")-1,    0 TSRMLS_CC)
#define TXT_PROP    zend_read_property(default_exception_ce, getThis(), "message", sizeof("message")-1, 0 TSRMLS_CC)

#define DEFAULT_0_PARAMS \
        if (zend_parse_parameters_none() == FAILURE) { \
                return; \
        }

static void _get_entry(zval *value, zval *return_value TSRMLS_DC) /* {{{ */
{
  *return_value = *value;
  zval_copy_ctor(return_value);
  INIT_PZVAL(return_value);
}


static
PHP_METHOD(MqSExceptionC,getNum)
{
  DEFAULT_0_PARAMS;
  _get_entry(NUM_PROP, return_value TSRMLS_CC);
}

static
PHP_METHOD(MqSExceptionC,getCode)
{
  DEFAULT_0_PARAMS;
  _get_entry(CODE_PROP, return_value TSRMLS_CC);
}

PHP_METHOD(MqSExceptionC,getTxt)
{
  DEFAULT_0_PARAMS;
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
    } else if (instanceof_function(Z_OBJCE_P(ex), default_exception_ce TSRMLS_CC)) {
      zval *message = zend_read_property(default_exception_ce, ex, "message", strlen("message")-1, 0 TSRMLS_CC);
      //zval *trace = zend_read_property(default_exception_ce, ex, "trace", strlen("trace")-1, 0 TSRMLS_CC)
      MqErrorC(mqctx, "PHP-Error", -1, VAL2CST(message));
      return;
    }
  }
  zend_error(E_ERROR, "expect 'Exception' type as argument");
}

void NS(MqSException_Raise) (struct MqS* mqctx TSRMLS_DC) {
  zval *MqSExceptionO = NULL;
  MqErrorReset(mqctx);
  MAKE_STD_ZVAL(MqSExceptionO);
  ErrorCheck(object_init_ex(MqSExceptionO, MqSExceptionC));
  zend_update_property_string(default_exception_ce, MqSExceptionO, "message", sizeof("message")-1, MqErrorGetText(mqctx) TSRMLS_CC);
  zend_update_property_long(default_exception_ce, MqSExceptionO, "code", sizeof("code")-1, MqErrorGetCodeI(mqctx) TSRMLS_CC);
  zend_update_property_long(default_exception_ce, MqSExceptionO, "num", sizeof("num")-1, MqErrorGetNumI(mqctx) TSRMLS_CC);
  zend_throw_exception_object(MqSExceptionO TSRMLS_CC);
  return;
error:
  zend_error(E_ERROR, "unable to raise MqSException");
}

static const zend_function_entry MqSException_functions[] = {
  PHP_ME(MqSExceptionC, getNum, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(MqSExceptionC, getTxt, NULL, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};

void NS(MqSException_Init) (TSRMLS_D) {
  zend_class_entry me_ce;
  default_exception_ce = zend_exception_get_default(TSRMLS_C);

  // create class and make depend on "Exception"
  INIT_CLASS_ENTRY(me_ce,"MqSException", MqSException_functions);
  MqSExceptionC = zend_register_internal_class_ex(&me_ce, default_exception_ce, NULL TSRMLS_CC);

  // define additional properties ("message" and "code" is covered by "Exception")
  zend_declare_property_null(MqSExceptionC, "num",  strlen("num")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}

