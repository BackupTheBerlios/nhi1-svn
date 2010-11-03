/**
 *  \file       theLink/phpmsgque/PhpMsgque/send_php.c
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

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

#define Send(T,M) PHP_METHOD(PhpMsgque_MqS, Send ## T) \
{ \
  SETUP_mqctx; \
  ARG2 ## M(Send ## T,val); \
  ErrorMqToPhpWithCheck(MqSend ## T (mqctx, (MQ_ ## M)val)); \
  RETURN_NULL(); \
}

Send(Y,BYT)
Send(O,BOL)
Send(S,SRT)
Send(I,INT)
Send(W,WID)
Send(F,FLT)
Send(D,DBL)
Send(C,CST)

PHP_METHOD(PhpMsgque_MqS, SendB)
{
  SETUP_mqctx;
  ARG2CST(SendB,val);
  ErrorMqToPhpWithCheck(MqSendB(mqctx,val,vallen));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendN)
{
  SETUP_mqctx;
  ARG2CST(SendN,val);
  ErrorMqToPhpWithCheck(MqSendN(mqctx,val,vallen));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendBDY)
{
  SETUP_mqctx;
  ARG2CST(SendBDY,val);
  ErrorMqToPhpWithCheck(MqSendBDY(mqctx,val,vallen));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendU)
{
  SETUP_mqctx;
  ARG2OBJ(SendU,buf);
  ErrorMqToPhpWithCheck(MqSendU(mqctx,VAL2MqBufferS(buf)));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendSTART)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqSendSTART(mqctx));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendEND)
{
  SETUP_mqctx;
  ARG2CST(SendEND, token);
  ErrorMqToPhpWithCheck(MqSendEND(mqctx, token));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendEND_AND_WAIT)
{
  SETUP_mqctx;
  int timeout = MQ_TIMEOUT_USER;
  char *token; int tokenlen;
  if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, 
      "s|l", &token, &tokenlen, &timeout) == FAILURE) {
    RaiseError("usage: SendEND_AND_WAIT(string: token, long: timeout)");
    return;
  }
  ErrorMqToPhpWithCheck(MqSendEND_AND_WAIT(mqctx, token, timeout));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendEND_AND_CALLBACK)
{
  SETUP_mqctx;
  char *str; int strlen;
  zval *callable;
  void *data;
  MqTokenF tokenF;
  MqTokenDataFreeF tokenDataFreeF;
  // get parameters
  PhpErrorCheck(zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sz", &str, &strlen, &callable));
  // get the callbacks
  ErrorMqToPhpWithCheck (
    NS(ProcInit) (mqctx, callable, &data, &tokenF, &tokenDataFreeF, NULL TSRMLS_CC)
  );
  ErrorMqToPhpWithCheck(MqSendEND_AND_CALLBACK(mqctx, str, tokenF, data, tokenDataFreeF));
  RETURN_NULL();
error:
  RaiseError("usage: SendEND_AND_CALLBACK(string: token, string or array: callback)");
  return;
}

PHP_METHOD(PhpMsgque_MqS, SendL_START)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqSendL_START(mqctx));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendL_END)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqSendL_END(mqctx));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendT_START)
{
  SETUP_mqctx;
  ARG2CST(SendT_START, token);
  ErrorMqToPhpWithCheck(MqSendT_START(mqctx, token));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendT_END)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqSendT_END(mqctx));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendRETURN)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqSendRETURN(mqctx));
  RETURN_NULL();
}

PHP_METHOD(PhpMsgque_MqS, SendERROR)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqSendERROR(mqctx));
  RETURN_NULL();
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Send_Init)(void) {
}

