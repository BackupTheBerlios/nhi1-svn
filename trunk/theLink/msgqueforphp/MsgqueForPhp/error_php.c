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
  SETUP_mqctx;
  CST2VAL(return_value, MqErrorGetText(mqctx));
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorGetNum)
{ 
  SETUP_mqctx;
  INT2VAL(return_value, MqErrorGetNum(mqctx));
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorC)
{ 
  SETUP_mqctx;
  char* prefix;  int prefixlen;
  char* message; int messagelen;
  long errnum;
  PhpErrorCheck(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
    "sls", &prefix, &prefixlen, &errnum, &message, &messagelen));
  MqErrorC(mqctx, prefix, (MQ_INT)errnum, message);
  RETURN_NULL();
error:
  RETURN_ERROR("usage: ErrorC(string: prefix, integer: errnum, string: message)");
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorSet)
{ 
  SETUP_mqctx;
  ARG2OBJ(ErrorSet,ex)
  NS(MqSException_Set)(mqctx, ex TSRMLS_CC);
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorSetCONTINUE)
{ 
  SETUP_mqctx;
  MqErrorSetCONTINUE(mqctx);
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorSetEXIT)
{ 
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqErrorSetEXIT(mqctx));
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorIsEXIT)
{ 
  SETUP_mqctx;
  BOL2VAL(return_value, MqErrorIsEXIT(mqctx));
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorReset)
{ 
  SETUP_mqctx;
  MqErrorReset(mqctx);
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorRaise)
{ 
  SETUP_mqctx;
  NS(MqSException_Raise)(mqctx TSRMLS_CC);
  RETURN_NULL();
} 

PHP_METHOD(MsgqueForPhp_MqS, ErrorPrint)
{ 
  SETUP_mqctx;
  MqErrorPrint(mqctx);
  RETURN_NULL();
} 

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Error_Init)(TSRMLS_D) {
}

