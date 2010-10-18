/**
 *  \file       theLink/rubymsgque/send_ruby.c
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
extern VALUE cMqBufferS;

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

#define Send(T,M) static VALUE Send ## T (VALUE self, VALUE val) { \
  SETUP_mqctx \
  ErrorMqToRubyWithCheck(MqSend ## T (mqctx, VAL2 ## M (val))); \
  return Qnil; \
}

Send(Y,BYT)
Send(O,BOL)
Send(S,SRT)
Send(I,INT)
Send(W,WID)
Send(F,FLT)
Send(D,DBL)
Send(C,CST)

static VALUE SendB (VALUE self, VALUE val) {
  SETUP_mqctx
  StringValue(val);
  ErrorMqToRubyWithCheck(MqSendB(mqctx,VAL2BIN(val)));
  return Qnil;
}

static VALUE SendN (VALUE self, VALUE val) {
  SETUP_mqctx
  StringValue(val);
  ErrorMqToRubyWithCheck(MqSendN(mqctx,VAL2BIN(val)));
  return Qnil;
}

static VALUE SendBDY (VALUE self, VALUE val) {
  SETUP_mqctx
  StringValue(val);
  ErrorMqToRubyWithCheck(MqSendBDY(mqctx,VAL2BIN(val)));
  return Qnil;
}

static VALUE SendU (VALUE self, VALUE buf) {
  SETUP_mqctx
  //CheckType(buf, cMqBufferS, "usage: SendU MqBufferS-Type-Arg");
  ErrorMqToRubyWithCheck(MqSendU(mqctx,VAL2MqBufferS(buf)));
  return Qnil;
}

static VALUE SendSTART (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqSendSTART(mqctx));
  return Qnil;
}

static VALUE SendEND (VALUE self, VALUE token) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqSendEND(mqctx, VAL2CST(token)));
  return Qnil;
}

static VALUE SendEND_AND_WAIT (int argc, VALUE *argv, VALUE self) {
  MQ_INT timeout = MQ_TIMEOUT_USER;
  SETUP_mqctx
  if (argc < 1 || argc > 2) rb_raise(rb_eArgError,"usage: SendEND_AND_WAIT token ?timeout?");
  if (argc == 2) timeout = VAL2INT(argv[1]);
  ErrorMqToRubyWithCheck(MqSendEND_AND_WAIT(mqctx, VAL2CST(argv[0]), timeout));
  return Qnil;
}

static VALUE SendEND_AND_CALLBACK (VALUE self, VALUE token, VALUE callback) {
  SETUP_mqctx
  MqServiceCallbackF procCall;
  MQ_PTR procData;
  MqTokenDataCopyF procCopy;
  CheckType(callback, rb_cMethod, "usage SendEND_AND_CALLBACK Method-Type-Arg");
  ErrorMqToRubyWithCheck(NS(ProcInit) (mqctx, callback, &procCall, &procData, &procCopy));
  ErrorMqToRubyWithCheck(MqSendEND_AND_CALLBACK(mqctx, VAL2CST(token), 
    procCall, procData, NS(ProcFree))
  );
  return Qnil;
}

static VALUE SendL_START (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqSendL_START(mqctx));
  return Qnil;
}

static VALUE SendL_END (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqSendL_END(mqctx));
  return Qnil;
}

static VALUE SendT_START (VALUE self, VALUE token) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqSendT_START(mqctx, VAL2CST(token)));
  return Qnil;
}

static VALUE SendT_END (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqSendT_END(mqctx));
  return Qnil;
}

static VALUE SendRETURN (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqSendRETURN(mqctx));
  return Qnil;
}

static VALUE SendERROR (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqSendERROR(mqctx));
  return Qnil;
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Send_Init)(void) {
  rb_define_method(cMqS, "SendY",		  SendY,		1);
  rb_define_method(cMqS, "SendO",		  SendO,		1);
  rb_define_method(cMqS, "SendS",		  SendS,		1);
  rb_define_method(cMqS, "SendI",		  SendI,		1);
  rb_define_method(cMqS, "SendW",		  SendW,		1);
  rb_define_method(cMqS, "SendF",		  SendF,		1);
  rb_define_method(cMqS, "SendD",		  SendD,		1);
  rb_define_method(cMqS, "SendC",		  SendC,		1);
  rb_define_method(cMqS, "SendB",		  SendB,		1);
  rb_define_method(cMqS, "SendN",		  SendN,		1);
  rb_define_method(cMqS, "SendBDY",		  SendBDY,		1);
  rb_define_method(cMqS, "SendU",		  SendU,		1);
  rb_define_method(cMqS, "SendSTART",		  SendSTART,		0);
  rb_define_method(cMqS, "SendEND",		  SendEND,		1);
  rb_define_method(cMqS, "SendEND_AND_WAIT",	  SendEND_AND_WAIT,	-1);
  rb_define_method(cMqS, "SendEND_AND_CALLBACK",  SendEND_AND_CALLBACK, 2);
  rb_define_method(cMqS, "SendL_START",		  SendL_START,		0);
  rb_define_method(cMqS, "SendL_END",		  SendL_END,		0);
  rb_define_method(cMqS, "SendT_START",		  SendT_START,		1);
  rb_define_method(cMqS, "SendT_END",		  SendT_END,		0);
  rb_define_method(cMqS, "SendRETURN",		  SendRETURN,		0);
  rb_define_method(cMqS, "SendERROR",		  SendERROR,		0);
}

