/**
 *  \file       theLink/phpmsgque/PhpMsgque/read_php.c
 *  \brief      \$Id: LbMain 265 2010-11-01 17:39:21Z aotto1968 $
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 265 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_php.h"

#define READ(T,M) \
PHP_METHOD(PhpMsgque_MqS, Read ## T) \
{ \
  SETUP_mqctx; \
  MQ_ ## M ret; \
  ErrorMqToPhpWithCheck(MqRead ## T (mqctx, &ret)); \
  M ## 2VAL (return_value, ret); \
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

/*
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
  VALUE ret;
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadBDY(mqctx, &val, &len));
  ret = BIN2VAL(val,len);
  return ret;
}

static VALUE ReadU (VALUE self) {
  MQ_BUF buf = NULL;
  VALUE ret;
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadU(mqctx, &buf));
  ret = NS(MqBufferS_New) (buf);
  return ret;
}

static VALUE ReadL_START (int argc, VALUE *argv, VALUE self) {
  MQ_BUF MqBufferS_object = NULL;
  SETUP_mqctx
  if (argc < 0 || argc > 2) {
    goto error;
  } else if (argc == 1) {
    if (rb_obj_is_kind_of(argv[0], cMqBufferS) == Qfalse) goto error;
    MqBufferS_object = VAL2MqBufferS(argv[0]);
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
  if (argc < 0 || argc > 1)
    rb_raise(rb_eArgError,"usage: ReadT_START ?MqBufferS-Type-Arg?");
  if (argc == 1) {
    CheckType(argv[0],cMqBufferS,"usage: ReadT_START ?MqBufferS-Type-Arg?")
    MqBufferS_object = VAL2MqBufferS(argv[0]);
  }
  ErrorMqToRubyWithCheck(MqReadT_START(mqctx, MqBufferS_object));
  return Qnil;
}

static VALUE ReadT_END (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqReadT_END(mqctx));
  return Qnil;
}
*/

/*
static VALUE ReadProxy (VALUE self, VALUE mqs) {
  SETUP_mqctx
  CheckType(mqs, cMqS, "usage: ReadProxy MqS-Type-Arg");
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
*/

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Read_Init)(void) {
}

