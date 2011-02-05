/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/read_php.c
 *  \brief      \$Id: LbMain 265 2010-11-01 17:39:21Z aotto1968 $
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 265 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_php.h"

#define READ(T,M) \
PHP_METHOD(MsgqueForPhp_MqS, Read ## T) \
{ \
  SETUP_mqctx; \
  MQ_ ## M ret; \
  ErrorMqToPhpWithCheck(MqRead ## T (mqctx, &ret)); \
  M ## 2VAL (return_value, ret); \
}

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

READ(Y,BYT);
READ(O,BOL);
READ(S,SRT);
READ(I,INT);
READ(W,WID);
READ(F,FLT);
READ(D,DBL);
READ(C,CST);

PHP_METHOD(MsgqueForPhp_MqS, ReadB)
{
  MQ_BIN val;
  MQ_SIZE len;
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqReadB(mqctx, &val, &len));
  BIN2VAL(return_value,val,len);
}

PHP_METHOD(MsgqueForPhp_MqS, ReadN)
{
  MQ_CBI val;
  MQ_SIZE len;
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqReadN(mqctx, &val, &len));
  BIN2VAL(return_value, val,len);
}

PHP_METHOD(MsgqueForPhp_MqS, ReadBDY)
{
  MQ_BIN val;
  MQ_SIZE len;
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqReadBDY(mqctx, &val, &len));
  BIN2VAL(return_value, val,len);
}

PHP_METHOD(MsgqueForPhp_MqS, ReadU)
{
  MQ_BUF buf = NULL;
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqReadU(mqctx, &buf));
  MqBufferS2VAL(return_value, buf);
}

PHP_METHOD(MsgqueForPhp_MqS, ReadL_START)
{
  SETUP_mqctx;
  MQ_BUF buf = NULL;
  zval *bufZ = NULL;
  PhpErrorCheck(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|o", &bufZ));
  if (bufZ) {
    CheckType(bufZ, NS(MqBufferS));
    VAL2MqBufferS(buf, bufZ);
  }
  ErrorMqToPhpWithCheck(MqReadL_START(mqctx, buf));
  RETURN_NULL();
error:
  RETURN_ERROR("usage: ReadL_START(?MqBufferS: buf=NULL?)");
}

PHP_METHOD(MsgqueForPhp_MqS, ReadL_END)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqReadL_END(mqctx));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, ReadT_START)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqReadT_START(mqctx));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, ReadT_END)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqReadT_END(mqctx));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, ReadProxy)
{
  SETUP_mqctx;
  ARG2MqS(ReadProxy, mqs);
  ErrorMqToPhpWithCheck(MqReadProxy(mqctx, mqs));
}

PHP_METHOD(MsgqueForPhp_MqS, ReadGetNumItems)
{
  SETUP_mqctx;
  INT2VAL(return_value, MqReadGetNumItems(mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, ReadItemExists)
{
  SETUP_mqctx;
  BOL2VAL(return_value, MqReadItemExists(mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, ReadUndo)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqReadUndo(mqctx));
  RETURN_NULL();
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Read_Init)(void) {
}

