/**
 *  \file       theLink/rubymsgque/misc_ruby.c
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

static VALUE b_proc (VALUE method) {
  return rb_method_call(0, NULL, method);
}

static VALUE r_proc (VALUE self, VALUE ex) {
  NS(MqSException_Set) (VAL2MqS(self), "ProcCall", ex);
  return Qnil;
}

enum MqErrorE NS(ProcCall) (
  struct MqS * const mqctx,
  MQ_PTR const dataP
) 
{ 
  MqErrorReset(mqctx);
  rb_rescue2( b_proc, PTR2VAL(dataP), r_proc, SELF, rb_eException, (VALUE)0);
  return MqErrorGetCode(mqctx);
} 

void NS(ProcFree) (
  struct MqS const * const mqctx,
  MQ_PTR *dataP
)
{
  VALUE val = (VALUE) dataP;
  rb_gc_unregister_address(&val);
  *dataP = NULL;
}

enum MqErrorE NS(ProcCopy) (
  struct MqS * const mqctx,
  MQ_PTR *dataP
)
{
  VALUE val = rb_obj_dup((VALUE) dataP);
  rb_gc_register_address(&val);
  *dataP = (MQ_PTR) val;
  return MQ_OK;
}


