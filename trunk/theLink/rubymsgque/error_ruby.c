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
  return self;
} 

static VALUE ErrorGetNum(VALUE self)
{ 
  return self;
} 

static VALUE ErrorC(VALUE self, VALUE prefix, VALUE num, VALUE text)
{ 
  SETUP_mqctx
  MqErrorC(mqctx, VAL2CST(prefix), VAL2INT(num), VAL2CST(text));
  return Qnil;
} 

static VALUE ErrorSet(VALUE self, VALUE ex)
{ 
  SETUP_mqctx
  NS(MqSException_Set)(mqctx, __func__, ex);
  return Qnil;
} 

static VALUE ErrorSetCONTINUE(VALUE self)
{ 
  return self;
} 

static VALUE ErrorIsExit(VALUE self)
{ 
  return self;
} 

static VALUE ErrorReset(VALUE self)
{ 
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqErrorReset(mqctx));
  return self;
} 

static VALUE ErrorRaise(VALUE self)
{ 
  SETUP_mqctx
  NS(MqSException_Raise)(mqctx);
  return Qnil;
} 

static VALUE ErrorPrint(VALUE self)
{ 
  return self;
} 

void NS(MqS_Error_Init)(void) {
  rb_define_method(cMqS, "ErrorGetText",      ErrorGetText,	0);
  rb_define_method(cMqS, "ErrorGetNum",	      ErrorGetNum,	0);
  rb_define_method(cMqS, "ErrorC",	      ErrorC,		3);
  rb_define_method(cMqS, "ErrorSet",	      ErrorSet,		1);
  rb_define_method(cMqS, "ErrorSetCONTINUE",  ErrorSetCONTINUE, 0);
  rb_define_method(cMqS, "ErrorIsExit",	      ErrorIsExit,	0);
  rb_define_method(cMqS, "ErrorReset",	      ErrorReset,	0);
  rb_define_method(cMqS, "ErrorRaise",	      ErrorRaise,	0);
  rb_define_method(cMqS, "ErrorPrint",	      ErrorPrint,	0);
}
