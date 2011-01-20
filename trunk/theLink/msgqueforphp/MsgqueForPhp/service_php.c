/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/service_php.c
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

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

PHP_METHOD(MsgqueForPhp_MqS, ServiceGetToken)
{
  SETUP_mqctx;
  RETURN_STRING(MqServiceGetToken(mqctx), 1);
}

PHP_METHOD(MsgqueForPhp_MqS, ServiceIsTransaction)
{
  SETUP_mqctx;
  RETURN_BOOL(MqServiceIsTransaction(mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, ServiceGetFilter)
{
  SETUP_mqctx;
  struct MqS * filter;
  long id=0;
  PhpErrorCheck(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &id));
  ErrorMqToPhpWithCheck(MqServiceGetFilter(mqctx, id, &filter));
  MqS2VAL(return_value,filter);
  return;
error:
  RETURN_ERROR("usage: ServiceGetFilter(?integer: id=0?)");
}

PHP_METHOD(MsgqueForPhp_MqS, ServiceCreate)
{
  SETUP_mqctx;
  char *str; int strlen;
  zval *callable;
  void *data;
  MqTokenF tokenF;
  MqDataFreeF tokenDataFreeF;
  // get parameters
  PhpErrorCheck(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &str, &strlen, &callable));
  // get the callbacks
  ErrorMqToPhpWithCheck (
    NS(ProcInit) (mqctx, callable, &data, &tokenF, &tokenDataFreeF, NULL TSRMLS_CC) \
  );
  ErrorMqToPhpWithCheck(MqServiceCreate(mqctx, str, tokenF, data, tokenDataFreeF));
  RETURN_NULL();
error:
  RETURN_ERROR("usage: ServiceCreate(string: token, string or array: callback)");
}

PHP_METHOD(MsgqueForPhp_MqS, ServiceDelete)
{
  SETUP_mqctx;
  ARG2CST(ServiceDelete,token);
  ErrorMqToPhpWithCheck(MqServiceDelete(mqctx, token));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, ServiceProxy)
{
  SETUP_mqctx;
  char *str;
  int strlen;
  long id=0;
  PhpErrorCheck(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &strlen, &id));
  ErrorMqToPhpWithCheck(MqServiceProxy(mqctx, str, id));
  RETURN_NULL();
error:
  RETURN_ERROR("usage: ServiceProxy(string: token, ?integer: id=0?");
}

PHP_METHOD(MsgqueForPhp_MqS, ProcessEvent)
{
  SETUP_mqctx;
  MQ_TIME_T timeout = MQ_TIMEOUT_DEFAULT;
  enum MqWaitOnEventE wait = MQ_WAIT_NO;
  long arg1=0, arg2=0;
  PhpErrorCheck(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &arg1, &arg2));
  if (ZEND_NUM_ARGS() == 1) {
    wait = arg1;
  }
  if (ZEND_NUM_ARGS() == 2) {
    timeout = arg1;
    wait = arg2;
  }
  ErrorMqToPhpWithCheck(MqProcessEvent(mqctx, timeout, wait));
  RETURN_NULL();
error:
  RETURN_ERROR("usage: ProcessEvent(?integer|MqS::TIMEOUT_(DEFAULT|USER|MAX): timeout?, ?MqS::WAIT_(NO|ONCE|FOREVER): wait?)");
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Service_Init)(TSRMLS_D) {
}

