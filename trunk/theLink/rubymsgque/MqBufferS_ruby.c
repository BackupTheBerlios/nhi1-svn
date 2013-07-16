/**
 *  \file       theLink/rubymsgque/MqBufferS_ruby.c
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

VALUE cMqBufferS;

#define BUF	      VAL2MqBufferS(self)
#define SETUP_buf     MQ_BUF buf = BUF;
#define ErrorBufToRuby() NS(MqSException_Raise)(buf->context)
#define ErrorBufToRubyWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorBufToRuby(); \
  }

#define Mth(T) \
rb_define_method(cMqBufferS, MQ_CPPXSTR(Get ## T), Get ## T, 0); \
rb_define_method(cMqBufferS, MQ_CPPXSTR(Set ## T), Set ## T, 1); \

#define Mth2(T) \
rb_define_method(cMqBufferS, MQ_CPPXSTR(Get ## T), Get ## T, 0);

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

#define GET(T,M) \
static VALUE Get ## T (VALUE self) { \
  SETUP_buf \
  MQ_ ## M ret; \
  ErrorBufToRubyWithCheck(MqBufferGet ## T (buf, &ret)); \
  return M ## 2VAL (ret); \
}

GET(Y,BYT)
GET(O,BOL)
GET(S,SRT)
GET(I,INT)
GET(W,WID)
GET(F,FLT)
GET(D,DBL)
GET(C,CST)

static VALUE GetB (VALUE self) {
  MQ_BIN val;
  MQ_SIZE len;
  SETUP_buf
  ErrorBufToRubyWithCheck(MqBufferGetB(buf, &val, &len));
  return BIN2VAL(val,len);
}

#define SET(T,M) static VALUE Set ## T (VALUE self, VALUE val) { \
  SETUP_buf \
  return NS(MqBufferS_New) (MqBufferSet ## T (buf, VAL2 ## M (val))); \
}

SET(Y,BYT)
SET(O,BOL)
SET(S,SRT)
SET(I,INT)
SET(W,WID)
SET(F,FLT)
SET(D,DBL)
SET(C,CST)

static VALUE SetB (VALUE self, VALUE val) {
  SETUP_buf
  StringValue(val);
  return NS(MqBufferS_New) (MqBufferSetB (buf, VAL2BIN (val)));
}

static VALUE GetType (VALUE self) {
  MQ_STRB const str[2] = {MqBufferGetType(BUF), '\0'};
  return CST2VAL(str);
}

static VALUE Dup (VALUE self) {
  return MqBufferS2VAL(MqBufferDup(BUF));
}

static VALUE Delete (VALUE self) {
  SETUP_buf
  MqBufferDelete(&buf);
  return Qnil;
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

VALUE NS(MqBufferS_New) (MQ_BUF buf) {
  return Data_Wrap_Struct(cMqBufferS, NULL, NULL, buf);
}

void NS(MqBufferS_Init) (void) {
  cMqBufferS = rb_define_class("MqBufferS", rb_cObject);

  // make class note create-able
  rb_funcall(cMqBufferS, rb_intern("private_class_method"), 1, CST2VAL("new"));

  rb_define_method(cMqBufferS, "Dup",	  Dup,	  0);
  rb_define_method(cMqBufferS, "Delete",  Delete, 0);

  Mth(Y)  
  Mth(O)  
  Mth(S)  
  Mth(I)  
  Mth(W)  
  Mth(F)  
  Mth(D)  
  Mth(C)  
  Mth(B)  
  Mth2(Type)  
}

