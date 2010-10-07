/**
 *  \file       theLink/rubymsgque/error_ruby.c
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

static VALUE ErrorGetText(VALUE self)
{ 
  return CST2VAL(MqErrorGetText(MQCTX));
} 

static VALUE ErrorGetNum(VALUE self)
{ 
  return INT2VAL(MqErrorGetNum(MQCTX));
} 

static VALUE ErrorC(VALUE self, VALUE prefix, VALUE num, VALUE text)
{ 
  MqErrorC(MQCTX, VAL2CST(prefix), VAL2INT(num), VAL2CST(text));
  return Qnil;
} 

static VALUE ErrorSet(VALUE self, VALUE ex)
{ 
  NS(MqSException_Set)(MQCTX, ex);
  return Qnil;
} 

static VALUE ErrorSetCONTINUE(VALUE self)
{ 
  MqErrorSetCONTINUE(MQCTX);
  return Qnil;
} 

static VALUE ErrorIsEXIT(VALUE self)
{ 
  return BOL2VAL(MqErrorIsEXIT(MQCTX));
} 

static VALUE ErrorReset(VALUE self)
{ 
  MqErrorReset(MQCTX);
  return Qnil;
} 

static VALUE ErrorRaise(VALUE self)
{ 
  NS(MqSException_Raise)(MQCTX);
  return Qnil;
} 

static VALUE ErrorPrint(VALUE self)
{ 
  MqErrorPrint(MQCTX);
  return Qnil;
} 

void NS(MqS_Error_Init)(void) {
  rb_define_method(cMqS, "ErrorGetText",      ErrorGetText,	0);
  rb_define_method(cMqS, "ErrorGetNum",	      ErrorGetNum,	0);
  rb_define_method(cMqS, "ErrorC",	      ErrorC,		3);
  rb_define_method(cMqS, "ErrorSet",	      ErrorSet,		1);
  rb_define_method(cMqS, "ErrorSetCONTINUE",  ErrorSetCONTINUE, 0);
  rb_define_method(cMqS, "ErrorIsEXIT",	      ErrorIsEXIT,	0);
  rb_define_method(cMqS, "ErrorReset",	      ErrorReset,	0);
  rb_define_method(cMqS, "ErrorRaise",	      ErrorRaise,	0);
  rb_define_method(cMqS, "ErrorPrint",	      ErrorPrint,	0);
}
