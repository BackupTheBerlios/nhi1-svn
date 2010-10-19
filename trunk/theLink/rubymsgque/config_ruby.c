/**
 *  \file       theLink/rubymsgque/config_ruby.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_ruby.h"

extern VALUE cMqS;

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

#define Get(K,T) \
static VALUE ConfigGet ## K (VALUE self) { \
  return T ## 2VAL(MqConfigGet ## K (MQCTX)); \
}

#define Set(K,T) \
static VALUE ConfigSet ## K (VALUE self, VALUE val) { \
  MqConfigSet ## K (MQCTX, VAL2 ## T (val)); \
  return Qnil; \
}
#define All(K,T) \
  Get(K,T) \
  Set(K,T)

#define MthBas(K,T,A) \
rb_define_method(cMqS, MQ_CPPXSTR(Config ## T ## K), Config ## T ## K, A); \

#define Mth(K) \
  MthBas(K,Get,0) \
  MthBas(K,Set,1) 

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

static VALUE ConfigSetIoTcp (VALUE self, VALUE host, VALUE port, VALUE myhost, VALUE myport) {
  MqConfigSetIoTcp (MQCTX, VAL2CST(host), VAL2CST(port), VAL2CST(myhost), VAL2CST(myport));
  return Qnil;
}

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
  //struct MqS * const mqctx = tmpl;
/*
  if (create == MQ_FACTORY_NEW_FORK) {
    //rb_thread_atfork();
  }
  if (create == MQ_FACTORY_NEW_THREAD) {
    //RUBY_INIT_STACK;
  }
*/
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

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Config_Init)(void) {
  Mth(Buffersize)
  Mth(Debug)
  Mth(Timeout)
  Mth(Name)
  Mth(SrvName)
  Mth(Ident)
  Mth(IsSilent)
  Mth(IsString)
  Mth(IsServer)
  Mth(IoUdsFile)
  Mth(IoPipeSocket)
  Mth(StartAs)

  MthBas(IoTcpHost,	Get, 0)
  MthBas(IoTcpPort,	Get, 0)
  MthBas(IoTcpMyHost,	Get, 0)
  MthBas(IoTcpMyPort,	Get, 0)

  rb_define_method(cMqS, "ConfigSetIoTcp", ConfigSetIoTcp, 4);

  MthBas(ServerSetup,	Set, 1)
  MthBas(ServerCleanup, Set, 1)
  MthBas(Factory,	Set, 1)
  MthBas(BgError,	Set, 1)
  MthBas(Event,		Set, 1)

  MthBas(IgnoreExit,	Set, 1)
}

