/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/error_php.c
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

PHP_METHOD(MsgqueForPhp_MqS, ErrorGetText)
{ 
  CST2VAL(return_value, MqErrorGetText(MQCTX));
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorGetNum)
{ 
  INT2VAL(return_value, MqErrorGetNum(MQCTX));
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorC)
{ 
  char* prefix;  int prefixlen;
  char* message; int messagelen;
  long errnum;
  PhpErrorCheck(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, 
    "sls", &prefix, &prefixlen, &errnum, &message, &messagelen));
  MqErrorC(MQCTX, prefix, (MQ_INT)errnum, message);
  RETURN_NULL();
error:
  RETURN_ERROR("usage: ErrorC(string: prefix, integer: errnum, string: message)");
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorSet)
{ 
  ARG2OBJ(ErrorSet,ex)
  NS(MqSException_Set)(MQCTX, ex TSRMLS_CC);
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorSetCONTINUE)
{ 
  MqErrorSetCONTINUE(MQCTX);
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorSetEXIT)
{ 
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqErrorSetEXIT(MQCTX));
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorIsEXIT)
{ 
  BOL2VAL(return_value, MqErrorIsEXIT(MQCTX));
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorReset)
{ 
  MqErrorReset(MQCTX);
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorRaise)
{ 
  NS(MqSException_Raise)(MQCTX TSRMLS_CC);
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorPrint)
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





