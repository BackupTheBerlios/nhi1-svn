/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/config_php.c
 *  \brief      \$Id: LbMain 244 2010-10-07 18:12:40Z aotto1968 $
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 244 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_php.h"
#include "zend_exceptions.h"

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

#define Get(K,T) \
PHP_METHOD(MsgqueForPhp_MqS, ConfigGet ## K) \
{ \
  SETUP_mqctx; \
  T ## 2VAL(return_value, MqConfigGet ## K (mqctx)); \
}

#define Set(K,T) \
PHP_METHOD(MsgqueForPhp_MqS, ConfigSet ## K) \
{ \
  SETUP_mqctx; \
  ARG2 ## T(K,val); \
  MqConfigSet ## K (mqctx, (MQ_ ## T) (val)); \
  RETURN_NULL(); \
}
#define All(K,T) \
  Get(K,T) \
  Set(K,T)

All(Buffersize,	  INT)
All(Debug,	  INT)
All(Timeout,	  INT)
All(Name,	  CST)
All(SrvName,	  CST)
All(IsSilent,	  BOL)
All(IsString,	  BOL)
All(IsServer,	  BOL)
All(IoUdsFile,	  CST)
All(IoPipeSocket, INT)
All(StartAs,	  INT)

Get(IoTcpHost,	  CST)
Get(IoTcpPort,	  CST)
Get(IoTcpMyHost,  CST)
Get(IoTcpMyPort,  CST)
Get(StatusIs,     INT)

Set(IgnoreExit,	  BOL)

PHP_METHOD(MsgqueForPhp_MqS, ConfigSetIoTcp)
{
  SETUP_mqctx;
  MQ_CST host, port, myhost, myport;
  MQ_INT hostL, portL, myhostL, myportL;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss", 
	&host, &hostL, &port, &portL, &myhost, &myhostL, &myport, &myportL) == FAILURE) {
    RETURN_NULL();
  }
  MqConfigSetIoTcp (mqctx, host, port, myhost, myport);
  RETURN_NULL();
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Config_Init)(TSRMLS_D) {
}








