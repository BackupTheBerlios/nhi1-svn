/**
 *  \file       theLink/phpmsgque/PhpMsgque/config_php.c
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

#define ARG2INT(val) \
long val;\
if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) { \
  zend_throw_exception(zend_exception_get_default(TSRMLS_C),"invalid argument - expect long",1 TSRMLS_CC); \
  return; \
}
#define ARG2CST(val) \
MQ_CST val; int len;\
if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &val, &len) == FAILURE) { \
  zend_throw_exception(zend_exception_get_default(TSRMLS_C),"invalid argument - expect string",1 TSRMLS_CC); \
  return; \
}

#define ARG2BOL(val) \
zend_bool val;\
if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "b", &val) == FAILURE) { \
  zend_throw_exception(zend_exception_get_default(TSRMLS_C),"invalid argument - expect boolean",1 TSRMLS_CC); \
  return; \
}

#define Get(K,T) \
PHP_METHOD(PhpMsgque_MqS, ConfigGet ## K) \
{ \
  T ## 2VAL(return_value, MqConfigGet ## K (MQCTX)); \
}

#define Set(K,T) \
PHP_METHOD(PhpMsgque_MqS, ConfigSet ## K) \
{ \
  ARG2 ## T(val); \
  MqConfigSet ## K (MQCTX, (MQ_ ## T) (val)); \
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
All(Ident,	  CST)
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

Set(IgnoreExit,	  BOL)

PHP_METHOD(PhpMsgque_MqS, ConfigSetIoTcp)
{
  MQ_CST host, port, myhost, myport;
  MQ_INT hostL, portL, myhostL, myportL;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss", 
	&host, &hostL, &port, &portL, &myhost, &myhostL, &myport, &myportL) == FAILURE) {
    RETURN_NULL();
  }
  MqConfigSetIoTcp (MQCTX, host, port, myhost, myport);
  RETURN_NULL();
}

/*
#define CB(T) static VALUE ConfigSet ## T (VALUE self, VALUE callback) { \
  SETUP_mqctx; \
  MqServiceCallbackF procCall; \
  MQ_PTR procData; \
  MqTokenDataCopyF procCopy; \
  CheckType(callback, rb_cMethod, "usage: ConfigSet" #T " Method-Type-Arg"); \
  ErrorMqToRubyWithCheck(NS(ProcInit) (mqctx, callback, &procCall, &procData, &procCopy)); \
  MqConfigSet ## T (mqctx,  procCall, procData, NS(ProcFree), procCopy); \
  return Qnil; \
}
*/

/*
CB(ServerSetup)
CB(ServerCleanup)
CB(BgError)
CB(Event)

static VALUE FactoryCreateCall (VALUE proc) {
  //return rb_proc_call_with_block(proc, 0, NULL, Qnil);
  return rb_proc_call(proc, rb_ary_new());
}

static enum MqErrorE
FactoryCreate(
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR data,
  struct MqS  ** mqctxP
)
{

  VALUE self = NS(Rescue)(tmpl, FactoryCreateCall, PTR2VAL(data));

  //VALUE self = rb_proc_call_with_block(PTR2VAL(data), 0, NULL, Qnil);

  if (NIL_P(self)) {
    *mqctxP = NULL;
    return MqErrorGetCode(tmpl);
  } else {
    struct MqS * const mqctx = *mqctxP = VAL2MqS(self);
    INCR_REG(self);
    MqConfigDup(mqctx, tmpl);
    MqErrorCheck(MqSetupDup(mqctx, tmpl));
    return MQ_OK;
error:
    *mqctxP = NULL;
    MqErrorCopy (tmpl, mqctx);
    return MqErrorStack(tmpl);
  }
  return MQ_OK;
}

static void
FactoryDelete(
  struct MqS * mqctx,
  MQ_BOL doFactoryDelete,
  MQ_PTR data
)
{
  MqContextFree (mqctx);
  if (doFactoryDelete && mqctx->self != NULL) {
    DECR_REG((VALUE)mqctx->self);
    mqctx->self = NULL;
  }
}


static VALUE ConfigSetFactory (VALUE self, VALUE proc)
{
  //rb_io_puts(1, &proc, rb_stdout);
  SETUP_mqctx;
  MqServiceCallbackF procCall;
  MQ_PTR procData;
  MqTokenDataCopyF procCopy;
  CheckType(proc, rb_cProc, "usage: ConfigSetFactory Proc-Type-Arg");
  ErrorMqToRubyWithCheck(NS(ProcInit) (mqctx, proc, &procCall, &procData, &procCopy));
  MqConfigSetFactory(mqctx,
    FactoryCreate,  procData,  NS(ProcFree), procCopy,
    FactoryDelete,  NULL,      NULL,         NULL
  );
  return Qnil;
}

*/

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Config_Init)(TSRMLS_D) {
}

