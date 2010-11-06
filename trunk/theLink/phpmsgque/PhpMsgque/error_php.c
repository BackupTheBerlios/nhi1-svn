/**
 *  \file       theLink/phpmsgque/PhpMsgque/error_php.c
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

PHP_METHOD(PhpMsgque_MqS, ErrorGetText)
{ 
  CST2VAL(return_value, MqErrorGetText(MQCTX));
} 

PHP_METHOD(PhpMsgque_MqS, ErrorGetNum)
{ 
  INT2VAL(return_value, MqErrorGetNum(MQCTX));
} 

PHP_METHOD(PhpMsgque_MqS, ErrorC)
{ 
  char* prefix;  int prefixlen;
  char* message; int messagelen;
  long errnum;
  PhpErrorCheck(zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, 
    "sls", &prefix, &prefixlen, &errnum, &message, &messagelen));
  MqErrorC(MQCTX, prefix, (MQ_INT)errnum, message);
  RETURN_NULL();
error:
  RaiseError("usage: ErrorC(string: prefix, long: errnum, string: message)");
  return;
} 

PHP_METHOD(PhpMsgque_MqS, ErrorSet)
{ 
  ARG2OBJ(ErrorSet,ex)
  NS(MqSException_Set)(MQCTX, ex TSRMLS_CC);
  RETURN_NULL();
} 

PHP_METHOD(PhpMsgque_MqS, ErrorSetCONTINUE)
{ 
  MqErrorSetCONTINUE(MQCTX);
  RETURN_NULL();
} 

PHP_METHOD(PhpMsgque_MqS, ErrorCreateEXIT)
{ 
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqErrorCreateEXIT(MQCTX));
  RETURN_NULL();
} 

PHP_METHOD(PhpMsgque_MqS, ErrorIsEXIT)
{ 
  BOL2VAL(return_value, MqErrorIsEXIT(MQCTX));
} 

PHP_METHOD(PhpMsgque_MqS, ErrorReset)
{ 
  MqErrorReset(MQCTX);
  RETURN_NULL();
} 

PHP_METHOD(PhpMsgque_MqS, ErrorRaise)
{ 
  NS(MqSException_Raise)(MQCTX TSRMLS_CC);
  RETURN_NULL();
} 

PHP_METHOD(PhpMsgque_MqS, ErrorPrint)
{ 
  MqErrorPrint(MQCTX);
  RETURN_NULL();
} 

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Error_Init)(TSRMLS_D) {
}
