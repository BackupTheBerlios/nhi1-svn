/**
 *  \file       theLink/rubymsgque/MqSException_ruby.c
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

static VALUE cMqSException;

static ID id_new;
static ID id_num;
static ID id_code;
static ID id_txt;

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/
  
static VALUE initialize(VALUE self, VALUE num, VALUE code, VALUE txt)
{
  rb_ivar_set(self, id_num,  num);
  rb_ivar_set(self, id_code, code);
  rb_call_super(1, &txt);
  return self;
}

static VALUE GetNum(VALUE self)
{
  return rb_ivar_get(self, id_num);
}

static VALUE GetCode(VALUE self)
{
  return rb_ivar_get(self, id_code);
}

static VALUE GetTxt(VALUE self)
{
  return rb_funcall(self, id_txt, 0);
}

inline static MQ_INT GetNumN(VALUE self)
{
  return VAL2INT(GetNum(self));
}

inline static enum MqErrorE GetCodeN(VALUE self)
{
  return VAL2INT(GetCode(self));
}

inline static MQ_CST GetTxtN(VALUE self)
{
  VALUE str = GetTxt(self);
  return VAL2CST(str);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqSException_Set) (struct MqS* mqctx, MQ_CST prefix, VALUE ex) {
  if (rb_obj_is_kind_of(ex, cMqSException) == Qtrue) {
    MqErrorSet (mqctx, GetNumN(ex), GetCodeN(ex), GetTxtN(ex), NULL);
  } else if (rb_obj_is_kind_of(ex, rb_eException) == Qtrue) {
    MqErrorC(mqctx, prefix, -1, GetTxtN(ex));
  } else {
    rb_raise(rb_eTypeError, "expect 'Exception' type as argument");
  }
}

void NS(MqSException_Raise) (struct MqS* mqctx) {
  VALUE args[3] = {
    INT2VAL	(MqErrorGetNumI  (mqctx)), 
    INT2VAL	(MqErrorGetCodeI (mqctx)), 
    CST2VAL	(MqErrorGetText  (mqctx))
  };
  rb_exc_raise(rb_class_new_instance(3,args,cMqSException));
}

void NS(MqSException_Init) (void) {
  cMqSException = rb_define_class("MqSException", rb_eException);

  rb_define_method(cMqSException, "initialize", initialize, 3);

  rb_define_method(cMqSException, "GetNum", GetNum, 0);
  rb_define_method(cMqSException, "GetTxt", GetTxt, 0);

  id_new  = rb_intern("new");
  id_num  = rb_intern("@num");
  id_code = rb_intern("@code");
  id_txt  = rb_intern("message");
}

