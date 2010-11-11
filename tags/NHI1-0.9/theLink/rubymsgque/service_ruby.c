/**
 *  \file       theLink/rubymsgque/service_ruby.c
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

static VALUE ServiceGetToken (VALUE self) {
  return CST2VAL(MqServiceGetToken(MQCTX));
}

static VALUE ServiceIsTransaction (VALUE self) {
  return BOL2VAL(MqServiceIsTransaction(MQCTX));
}

static VALUE ServiceGetFilter (int argc, VALUE *argv, VALUE self) {
  MQ_INT id=0;
  SETUP_mqctx
  struct MqS * ctx;
  if (argc < 0 || argc > 1) rb_raise(rb_eArgError, "usage: ServiceGetFilter ?id=0?");
  if (argc == 1) id = VAL2INT(argv[0]);
  ErrorMqToRubyWithCheck(MqServiceGetFilter(mqctx, id, &ctx));
  return MqS2VAL(ctx);
}

static VALUE ServiceCreate (VALUE self, VALUE token, VALUE callback) {
  SETUP_mqctx
  MqServiceCallbackF procCall;
  MQ_PTR procData;
  MqTokenDataCopyF procCopy;
  CheckType(callback, rb_cMethod, "usage ServiceCreate token Method-Type-Arg");
  ErrorMqToRubyWithCheck(NS(ProcInit) (mqctx, callback, &procCall, &procData, &procCopy));
  ErrorMqToRubyWithCheck(MqServiceCreate(mqctx, VAL2CST(token), 
    procCall, procData, NS(ProcFree))
  );
  return Qnil;
}

static VALUE ServiceDelete (VALUE self, VALUE token) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqServiceDelete(mqctx, VAL2CST(token)));
  return Qnil;
}

static VALUE ServiceProxy (int argc, VALUE *argv, VALUE self) {
  SETUP_mqctx
  MQ_INT id = 0;
  if (argc < 1 || argc > 2) rb_raise(rb_eArgError, "usage: ServiceProxy token ?id?");
  if (argc == 2) id = VAL2INT(argv[1]);
  ErrorMqToRubyWithCheck(MqServiceProxy(mqctx, VAL2CST(argv[0]), id));
  return Qnil;
}

static VALUE ProcessEvent (int argc, VALUE *argv, VALUE self) {
  SETUP_mqctx
  MQ_TIME_T timeout = MQ_TIMEOUT_DEFAULT;
  enum MqWaitOnEventE wait = MQ_WAIT_NO;
  if (argc < 0 || argc > 2) rb_raise(rb_eArgError, "usage: ProcessEvent ?timeout? ?wait?");
  if (argc == 1) {
    wait = VAL2INT(argv[0]);
  }
  if (argc == 2) {
    timeout = VAL2INT(argv[0]);
    wait = VAL2INT(argv[1]);
  }
  ErrorMqToRubyWithCheck(MqProcessEvent(mqctx, timeout, wait));
  return Qnil;
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Service_Init)(void) {

  // Methods
  rb_define_method(cMqS, "ServiceGetToken",	  ServiceGetToken,	0);
  rb_define_method(cMqS, "ServiceIsTransaction",  ServiceIsTransaction, 0);
  rb_define_method(cMqS, "ServiceGetFilter",	  ServiceGetFilter,	-1);
  rb_define_method(cMqS, "ServiceCreate",	  ServiceCreate,	2);
  rb_define_method(cMqS, "ServiceDelete",	  ServiceDelete,	1);
  rb_define_method(cMqS, "ServiceProxy",	  ServiceProxy,		-1);
  rb_define_method(cMqS, "ProcessEvent",	  ProcessEvent,		-1);
}

