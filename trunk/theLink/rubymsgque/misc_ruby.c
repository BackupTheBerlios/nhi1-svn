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
extern VALUE id_clone;
extern VALUE id_unbind;
extern VALUE id_bind;

static VALUE sRescueError (VALUE self, VALUE ex) {
  NS(MqSException_Set) (VAL2MqS(self), ex);
  return Qnil;
}

VALUE NS(Rescue) (
  struct MqS * const  mqctx,
  VALUE(*proc)(ANYARGS),
  VALUE data
)
{ 
  VALUE val = rb_rescue2(proc, data, sRescueError, SELF, rb_eException, (VALUE)0);
  return val;
}

// ==========================================================================
// call just a Proc

static VALUE ProcCallCall (VALUE proc) {
  return rb_proc_call_with_block(proc, 0, NULL, Qnil);
}

static enum MqErrorE ProcCall (
  struct MqS * const mqctx,
  MQ_CST prefix,
  MQ_PTR const dataP
) 
{ 
  NS(Rescue)(mqctx,ProcCallCall,PTR2VAL(dataP));
  return MqErrorGetCode(mqctx);
} 

// ==========================================================================
// Method belongs to calling object

static VALUE ProcCallMethodCall (VALUE method) {
  return rb_method_call(0, NULL, method);
}

static enum MqErrorE ProcCallMethod (
  struct MqS * const mqctx,
  MQ_CST prefix,
  MQ_PTR const dataP
) 
{ 
  NS(Rescue)(mqctx,ProcCallMethodCall,PTR2VAL(dataP));
  return MqErrorGetCode(mqctx);
} 

// ==========================================================================
// Method belongs NOT to calling object

static VALUE ProcCallMethodWithArgCall (VALUE array) {
  VALUE *valP = RARRAY_PTR(array);
  return rb_method_call(1, valP, valP[1]);
}

static enum MqErrorE ProcCallMethodWithArg (
  struct MqS * const mqctx,
  MQ_CST prefix,
  MQ_PTR const dataP
) 
{ 
  NS(Rescue)(mqctx,ProcCallMethodWithArgCall,PTR2VAL(dataP));
  return MqErrorGetCode(mqctx);
} 

// ==========================================================================

void NS(ProcFree) (
  struct MqS const * const mqctx,
  MQ_CST prefix,
  MQ_PTR *dataP
)
{
  VALUE val = PTR2VAL(dataP);
  DECR_REF(val);
  *dataP = NULL;
}

static enum MqErrorE ProcCopyProc (
  struct MqS * const mqctx,
  MQ_CST prefix,
  MQ_PTR *dataP
)
{
  VALUE val = PTR2VAL(*dataP);
  val = rb_funcall(val,id_clone,0,NULL);
  INCR_REF(val);
  *dataP = VAL2PTR(val);
  return MQ_OK;
}

static enum MqErrorE ProcCopyMethod (
  struct MqS * const mqctx,
  MQ_CST prefix,
  MQ_PTR *dataP
)
{
  SETUP_self;
  VALUE val = PTR2VAL(*dataP);
  val = rb_funcall(val,id_clone,0,NULL);
  val = rb_funcall(val,id_unbind,0,NULL);
  val = rb_funcall(val,id_bind,1,self);
  INCR_REF(val);
  *dataP = VAL2PTR(val);
  return MQ_OK;
}

static enum MqErrorE ProcCopyMethodWithArg (
  struct MqS * const mqctx,
  MQ_CST prefix,
  MQ_PTR *dataP
)
{
  VALUE ary = PTR2VAL(*dataP);
  VALUE mth = rb_ary_entry(ary,1);
  ary = rb_ary_new3(2,SELF,rb_funcall(mth,id_clone,0,NULL));
  INCR_REF(ary);
  *dataP = VAL2PTR(ary);
  return MQ_OK;
}

enum MqErrorE NS(ProcInit) (
  struct MqS	      *mqctx,
  VALUE		      val, 
  MqServiceCallbackF  *procCall, 
  MQ_PTR	      *procData,
  MqTokenDataCopyF    *procCopy
) {
  SETUP_self
  VALUE dataVal;
  if (rb_obj_is_kind_of(val, rb_cProc) == Qtrue) {
      *procCall = ProcCall;
      *procCopy = ProcCopyProc;
      dataVal   = val;
  } else if (rb_obj_is_kind_of(val, rb_cMethod) == Qtrue) {
    if (rb_equal(self,rb_funcall(val,id_receiver,0,NULL)) == Qtrue) {
      // val belongs to calling object, NO argument is required
      *procCall = ProcCallMethod;
      *procCopy = ProcCopyMethod;
      dataVal   = val;
    } else {
      // val belongs NOT to calling object, argument is required
      *procCall = ProcCallMethodWithArg;
      *procCopy = ProcCopyMethodWithArg;
      dataVal   = rb_ary_new3(2,self,val);
    }
  } else {
    return MqErrorC(mqctx,__func__,1,"expect 'proc' or 'method' argument");
  }
  *procData = VAL2PTR(dataVal);
  INCR_REF(dataVal);
  return MQ_OK;
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


