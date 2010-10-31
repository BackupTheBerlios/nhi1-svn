/**
 *  \file       theLink/phpmsgque/PhpMsgque/service_php.c
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

PHP_METHOD(PhpMsgque_MqS, ServiceGetToken)
{
  RETURN_STRING(MqServiceGetToken(MQCTX), 1);
}

PHP_METHOD(PhpMsgque_MqS, ServiceIsTransaction)
{
  RETURN_BOOL(MqServiceIsTransaction(MQCTX));
}

PHP_METHOD(PhpMsgque_MqS, ServiceGetFilter)
{
  SETUP_mqctx;
  struct MqS * filter;
  long id=0;
  if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "|l", &id) == FAILURE) {
    RaiseError("usage: ServiceGetFilter ?id=0?");
    return;
  }
  ErrorMqToPhpWithCheck(MqServiceGetFilter(mqctx, id, &filter));
  MqS2VAL(return_value,filter);
}

PHP_METHOD(PhpMsgque_MqS, ServiceCreate)
{
  SETUP_mqctx;
  char *str;
  int strlen;
  zval *callable;
  void *data;
  MqTokenF tokenF;
  MqTokenDataFreeF tokenDataFreeF;
  // get parameters
  PhpErrorCheck(zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sz", &str, &strlen, &callable));
  // get the callbacks
  ErrorMqToPhpWithCheck (
    NS(ProcInit) (mqctx, callable, &data, &tokenF, &tokenDataFreeF, NULL TSRMLS_CC) \
  );
  ErrorMqToPhpWithCheck(MqServiceCreate(mqctx, str, tokenF, data, tokenDataFreeF));
  RETURN_NULL();
error:
  RaiseError("usage: ServiceCreate(string: token, string or array: callback)");
  return;
}

PHP_METHOD(PhpMsgque_MqS, ServiceDelete)
{
  SETUP_mqctx;
  ARG2CST(ServiceDelete,token);
  ErrorMqToPhpWithCheck(MqServiceDelete(mqctx, token));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, ServiceProxy)
{
  SETUP_mqctx;
  char *str;
  int strlen;
  long id=0;
  if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &strlen, &id) == FAILURE) {
    RaiseError("usage: ServiceProxy token ?id=0?");
    return;
  }
  ErrorMqToPhpWithCheck(MqServiceProxy(mqctx, str, id));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, ProcessEvent)
{
  SETUP_mqctx;
  MQ_TIME_T timeout = MQ_TIMEOUT_DEFAULT;
  enum MqWaitOnEventE wait = MQ_WAIT_NO;
  long arg1=0, arg2=0;
  if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &arg1, &arg2) == FAILURE) {
    RaiseError("usage: ProcessEvent ?timeout? ?wait?");
    return;
  }
  if (ZEND_NUM_ARGS() == 1) {
    wait = arg1;
  }
  if (ZEND_NUM_ARGS() == 2) {
    timeout = arg1;
    wait = arg2;
  }
  ErrorMqToPhpWithCheck(MqProcessEvent(mqctx, timeout, wait));
  RETURN_NULL();
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Service_Init)(TSRMLS_D) {
}

