/**
 *  \file       theLink/rubymsgque/read_ruby.c
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

#define READ(T,M) \
static VALUE Read ## T (VALUE self) { \
  struct MqS * mqctx; \
  MQ_ ## M ret; \
  Data_Get_Struct(self, struct MqS, mqctx); \
  ErrorMqToRubyWithCheck(MqRead ## T (mqctx, &ret)); \
  return M ## 2VAL (ret); \
}

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

READ(Y,BYT);
READ(O,BOL);
READ(S,SRT);
READ(I,INT);
READ(W,WID);
READ(F,FLT);
READ(D,DBL);
READ(C,CST);

static VALUE ReadB (VALUE self) {
  MQ_BIN val;
  MQ_SIZE len;
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadB(mqctx, &val, &len));
  return BIN2VAL(val,len);
}

static VALUE ReadN (VALUE self) {
  MQ_CBI val;
  MQ_SIZE len;
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadN(mqctx, &val, &len));
  return BIN2VAL(val,len);
}

static VALUE ReadBDY (VALUE self) {
  MQ_BIN val;
  MQ_SIZE len;
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadBDY(mqctx, &val, &len));
  return BIN2VAL(val,len);
}

static VALUE ReadU (VALUE self) {
  MQ_BUF buf = NULL;
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadU(mqctx, &buf));
  return NS(MqBufferS_New) (buf);
}

static VALUE ReadL_START (int argc, VALUE *argv, VALUE self) {
  MQ_BUF MqBufferS_object = NULL;
  SETUP_mqctx
  if (argc != 1) goto error;
  if (rb_obj_is_kind_of(argv[0], cMqBufferS) == Qtrue) {
    MqBufferS_object = VAL2MqBufferS(argv[0]);
  } else {
    goto error;
  }
  ErrorMqToRubyWithCheck(MqReadL_START(mqctx, MqBufferS_object));
  return Qnil;
error:
  rb_raise(rb_eArgError,"usage: ReadL_START ?MqBufferS-Type-Arg?");
}

static VALUE ReadL_END (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadL_END(mqctx));
  return Qnil;
}

static VALUE ReadT_START (int argc, VALUE *argv, VALUE self) {
  MQ_BUF MqBufferS_object = NULL;
  SETUP_mqctx
  if (argc != 1) goto error;
  if (rb_obj_is_kind_of(argv[0], cMqBufferS) == Qtrue) {
    MqBufferS_object = VAL2MqBufferS(argv[0]);
  } else {
    goto error;
  }
  ErrorMqToRubyWithCheck(MqReadT_START(mqctx, MqBufferS_object));
  return Qnil;
error:
  rb_raise(rb_eArgError,"usage: ReadT_START ?MqBufferS-Type-Arg?");
}

static VALUE ReadT_END (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadT_END(mqctx));
  return Qnil;
}

static VALUE ReadProxy (VALUE self, VALUE mqs) {
  SETUP_mqctx
  CheckType(mqs, cMqS, usage: ReadProxy MqS-Type-Arg);
  ErrorMqToRubyWithCheck(MqReadProxy(mqctx, VAL2MqS(mqs)));
  return Qnil;
}

static VALUE ReadGetNumItems (VALUE self) {
  SETUP_mqctx
  return INT2VAL(MqReadGetNumItems(mqctx));
}

static VALUE ReadItemExists (VALUE self) {
  SETUP_mqctx
  return BOL2VAL(MqReadItemExists(mqctx));
}

static VALUE ReadUndo (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadUndo(mqctx));
  return Qnil;
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Read_Init)(void) {
  rb_define_method(cMqS, "ReadY",	      ReadY,		0);
  rb_define_method(cMqS, "ReadO",	      ReadO,		0);
  rb_define_method(cMqS, "ReadS",	      ReadS,		0);
  rb_define_method(cMqS, "ReadI",	      ReadI,		0);
  rb_define_method(cMqS, "ReadW",	      ReadW,		0);
  rb_define_method(cMqS, "ReadF",	      ReadF,		0);
  rb_define_method(cMqS, "ReadD",	      ReadD,		0);
  rb_define_method(cMqS, "ReadC",	      ReadC,		0);
  rb_define_method(cMqS, "ReadB",	      ReadB,		0);

  rb_define_method(cMqS, "ReadN",	      ReadN,		0);
  rb_define_method(cMqS, "ReadBDY",	      ReadBDY,		0);
  rb_define_method(cMqS, "ReadU",	      ReadU,		0);
  rb_define_method(cMqS, "ReadL_START",	      ReadL_START,	-1);
  rb_define_method(cMqS, "ReadL_END",	      ReadL_END,	0);
  rb_define_method(cMqS, "ReadT_START",	      ReadT_START,	0);
  rb_define_method(cMqS, "ReadT_END",	      ReadT_END,	0);
  rb_define_method(cMqS, "ReadProxy",	      ReadProxy,	1);
  rb_define_method(cMqS, "ReadGetNumItems",   ReadGetNumItems,	0);
  rb_define_method(cMqS, "ReadItemExists",    ReadItemExists,	0);
  rb_define_method(cMqS, "ReadUndo",	      ReadUndo,		0);
}

