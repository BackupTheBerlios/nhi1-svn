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

#define MQ_CONTEXT_S mqctx

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
  MqServiceCallbackF procCall; \
  MQ_PTR procData; \
  CheckType(callback, rb_cMethod, "usage: ConfigSet" #T " Method-Type-Arg"); \
  NS(ProcInit) (self, callback, &procCall, &procData); \
  MqConfigSet ## T (MQCTX,  procCall, procData, NS(ProcFree), NS(ProcCopy)); \
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

static VALUE b_proc (VALUE proc) {
  return rb_proc_call_with_block(proc, 0, NULL, Qnil);
}

static VALUE r_proc (VALUE tmpl, VALUE ex) {
  NS(MqSException_Set) (VAL2MqS(tmpl), ex);
  return Qnil;
}

static enum MqErrorE
FactoryCreate(
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR data,
  struct MqS  ** mqctxP
)
{
  if (create == MQ_FACTORY_NEW_FORK) rb_thread_atfork();
  VALUE self = rb_rescue2(
    b_proc, PTR2VAL(data), r_proc, MqS2VAL(tmpl), rb_eException, (VALUE)0
  );
  if (self == Qnil) {
    *mqctxP = NULL;
    return MqErrorGetCode(tmpl);
  } else {
    struct MqS * const mqctx = *mqctxP = VAL2MqS(self);
    rb_gc_register_address(&self);
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
  SETUP_self
  MqContextFree (mqctx);
  if (doFactoryDelete) rb_gc_unregister_address(&self);
}


static VALUE ConfigSetFactory (VALUE self, VALUE proc)
{
  //rb_io_puts(1, &proc, rb_stdout);
  CheckType(proc, rb_cProc, "usage: ConfigSetFactory Proc-Type-Arg");
  rb_gc_register_address(&proc);
  MqConfigSetFactory(MQCTX,
    FactoryCreate,  VAL2PTR(proc),  NS(ProcFree), NS(ProcCopy),
    FactoryDelete,  NULL,	    NULL,         NULL
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

