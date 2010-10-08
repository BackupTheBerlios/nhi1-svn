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

extern VALUE id_receiver;

static VALUE r_proc (VALUE self, VALUE ex) {
  NS(MqSException_Set) (VAL2MqS(self), ex);
  return Qnil;
}

// ==========================================================================
// Method belongs to calling object

static VALUE b_proc (VALUE method) {
  return rb_method_call(0, NULL, method);
}

static enum MqErrorE ProcCall (
  struct MqS * const mqctx,
  MQ_PTR const dataP
) 
{ 
  rb_rescue2( b_proc, PTR2VAL(dataP), r_proc, SELF, rb_eException, (VALUE)0);
  return MqErrorGetCode(mqctx);
} 

// ==========================================================================
// Method belongs NOT to calling object

static VALUE b_proc2 (VALUE array) {
  VALUE *valP = RARRAY_PTR(array);
  return rb_method_call(1, valP, valP[1]);
}

static enum MqErrorE ProcCall2 (
  struct MqS * const mqctx,
  MQ_PTR const dataP
) 
{ 
  rb_rescue2( b_proc2, PTR2VAL(dataP), r_proc, SELF, rb_eException, (VALUE)0);
  return MqErrorGetCode(mqctx);
} 

// ==========================================================================

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

void NS(ProcInit) (
  VALUE		      self, 
  VALUE		      method, 
  MqServiceCallbackF  *procCall, 
  MQ_PTR	      *procData
) {
  VALUE dataVal;
  if (rb_equal(self,rb_funcall(method,id_receiver,0,NULL)) == Qtrue) {
    // method belongs to calling object, NO argument is required
    *procCall = ProcCall;
    dataVal   = method;
  } else {
    // method belongs NOT to calling object, argument is required
    *procCall = ProcCall2;
    dataVal   = rb_ary_new3(2,self,method);
  }
  *procData = VAL2PTR(dataVal);
  rb_gc_register_address(&dataVal);
}

MQ_BFL NS(argv2bufl) (int argc, VALUE *argv)
{
  struct MqBufferLS * args = NULL;
  if (argc != 0) {
    int i;
    args = MqBufferLCreate (argc);
    for (i = 0; i < argc; i++, argv++) {
      const VALUE argv2 = *argv;
      if (rb_type(argv2) == T_ARRAY) {
	VALUE arg;
	while ((arg = rb_ary_shift(argv2)) != Qnil) {
	  MqBufferLAppendC (args, VAL2CST(arg));
	}
      } else {
	MqBufferLAppendC (args, VALP2CST(argv));
      }
    }
  }
  return args;
}

// ==========================================================================

