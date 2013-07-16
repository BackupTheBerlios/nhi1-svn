/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/MqBufferS_php.c
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

zend_class_entry *NS(MqBufferS);

#define CTX	      buf->context
#define BUF	      VAL2MqBufferSelf(getThis())
#define SETUP_buf     MQ_BUF buf = BUF;
#define ErrorBufToPhp() NS(MqSException_Raise)(buf->context TSRMLS_CC)
#define ErrorBufToPhpWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorBufToPhp(); \
  }

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

#define GET(T,M) static PHP_METHOD(MsgqueForPhp_MqBufferS, Get ## T) \
{ \
  SETUP_buf; \
  MQ_ ## M ret; \
  ErrorBufToPhpWithCheck(MqBufferGet ## T (buf, &ret)); \
  M ## 2VAL (return_value,ret); \
}

GET(Y,BYT);
GET(O,BOL);
GET(S,SRT);
GET(I,INT);
GET(W,WID);
GET(F,FLT);
GET(D,DBL);
GET(C,CST);

static
PHP_METHOD(MsgqueForPhp_MqBufferS, GetB)
{
  SETUP_buf;
  MQ_BIN val;
  MQ_SIZE len;
  ErrorBufToPhpWithCheck(MqBufferGetB(buf, &val, &len));
  BIN2VAL(return_value, val,len);
}

#define SET(T,M) static PHP_METHOD(MsgqueForPhp_MqBufferS, Set ## T) \
{ \
  SETUP_buf; \
  ARG2 ## M(Set ## T,val); \
  MqBufferSet ## T (buf, (MQ_ ## M)val); \
  MqBufferS2VAL(return_value, buf); \
}

SET(Y,BYT);
SET(O,BOL);
SET(S,SRT);
SET(I,INT);
SET(W,WID);
SET(F,FLT);
SET(D,DBL);
SET(C,CST);

static
PHP_METHOD(MsgqueForPhp_MqBufferS, SetB)
{
  SETUP_buf;
  ARG2CST(SendB,val);
  MqBufferSetB(buf,val,vallen);
  MqBufferS2VAL(return_value, buf);
}

static
PHP_METHOD(MsgqueForPhp_MqBufferS, GetType)
{
  MQ_STRB const str[2] = {MqBufferGetType(BUF), '\0'};
  CST2VAL(return_value, str);
}

static
PHP_METHOD(MsgqueForPhp_MqBufferS, Dup)
{
  MqBufferS2VAL(return_value, MqBufferDup(BUF));
}

static
PHP_METHOD(MsgqueForPhp_MqBufferS, Delete)
{
  SETUP_buf;
  MqBufferDelete(&buf);
}

static
PHP_METHOD(MsgqueForPhp_MqBufferS, __construct)
{
  RETURN_ERROR("it is not allowed to create an instance og 'MqBufferS'.");
}

ZEND_BEGIN_ARG_INFO_EX(no_arg, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(value_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "value")
ZEND_END_ARG_INFO()

static const zend_function_entry NS(MqBufferS_functions)[] = {
  PHP_ME(MsgqueForPhp_MqBufferS, __construct,	NULL,	      ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)

  PHP_ME(MsgqueForPhp_MqBufferS, GetY,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, GetO,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, GetS,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, GetI,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, GetW,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, GetF,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, GetD,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, GetC,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, GetB,		no_arg,	      ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqBufferS, SetY,		value_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, SetO,		value_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, SetS,		value_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, SetI,		value_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, SetW,		value_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, SetF,		value_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, SetD,		value_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, SetC,		value_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, SetB,		value_arg,    ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqBufferS, GetType,	no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, Dup,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqBufferS, Delete,	no_arg,	      ZEND_ACC_PUBLIC)

  {NULL, NULL, NULL}
};

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqBufferS_New) (zval *return_value, MQ_BUF buf TSRMLS_DC)
{
  // convert to an object instance
  if (object_init_ex(return_value, NS(MqBufferS)) == FAILURE) {
    RETURN_ERROR("unable to create an 'MqBufferS' instance.");
  }
  // link 'buf' with the object instance
  add_property_resource_ex(return_value, ID2(__buf), (long) buf TSRMLS_CC);
  return;
}

void NS(MqBufferS_Init) (TSRMLS_D) {
  zend_class_entry me_ce;

  // create class and make depend on "Exception"
  INIT_CLASS_ENTRY(me_ce,"MqBufferS", NS(MqBufferS_functions));
  NS(MqBufferS) = zend_register_internal_class(&me_ce TSRMLS_CC);
}
