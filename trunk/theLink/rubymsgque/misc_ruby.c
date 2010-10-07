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
  NS(MqSException_Set) (VAL2MqS(self), ex);
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

MQ_BFL NS(argv2bufl) (int argc, VALUE *argv)
{
  struct MqBufferLS * args = NULL;
  if (argc != 0) {
    int i;
    args = MqBufferLCreate (argc+1);
    MqBufferLAppendC (args, VAL2CST(rb_argv0));
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

