/**
 *  \file       theLink/rubymsgque/msgque_ruby.h
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "ruby.h"
#include "msgque.h"
#include "debug.h"

/*****************************************************************************/
/*                                                                           */
/*                                 definition's                              */
/*                                                                           */
/*****************************************************************************/

#define NS(n)		    RubyMsgque_ ## n
#define MQCTX		    VAL2MqS(self)
#define SETUP_mqctx	    struct MqS * mqctx = VAL2MqS(self);
#define SELF		    MqS2VAL(mqctx)
#define SETUP_self	    VALUE self = SELF;
#define MQ_CONTEXT_S	    mqctx

#define INCR_REF(val)	    if ((val)!=Qnil) { \
  rb_gc_register_address(&val);\
  MLVA(MQ_CONTEXT_S, "%p", INCR_REF, (MQ_PTR)val); \
  /* MVA(MQ_FORMAT_C, val, RSTRING_PTR(rb_obj_as_string(val))); */ \
}

#define DECR_REF(val)	    if ((val)!=Qnil) { \
  MLVA(MQ_CONTEXT_S, "%p", DECR_REF, (MQ_PTR)val); \
  /* MVA(MQ_FORMAT_C, val, RSTRING_PTR(rb_obj_as_string(val))); */ \
  rb_gc_unregister_address(&val);\
}

#define ErrorMqToRuby() NS(MqSException_Raise)(mqctx)
#define ErrorMqToRubyWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorMqToRuby(); \
  }

#define VAL2BYT(val)	    (MQ_BYT)FIX2INT(val)
#define VAL2BOL(val)	    (MQ_BOL)(NIL_P(val)||(val)==Qfalse?0:1)
#define VAL2SRT(val)	    (MQ_SRT)FIX2INT(val)
#define VAL2INT(val)	    (MQ_INT)FIX2INT(val)
#define VAL2WID(val)	    (MQ_WID)NUM2LL(val)
#define VAL2FLT(val)	    (MQ_FLT)NUM2DBL(val)
#define VAL2DBL(val)	    (MQ_DBL)NUM2DBL(val)
#define VAL2CST(val)	    (MQ_CST)RSTRING_PTR(rb_obj_as_string(val))
#define VAL2PTR(val)	    (MQ_PTR)val
#define VALP2CST(valp)	    (MQ_CST)rb_string_value_cstr(valp)
#define VAL2BIN(val)	    (MQ_CBI)RSTRING_PTR(val),RSTRING_LEN(val)
#define VAL2MqBufferS(val)  (MQ_BUF)DATA_PTR(val)
#define VAL2MqS(val)	    ((struct MqS*)DATA_PTR(val))

#define	BYT2VAL(nat)	    INT2FIX((MQ_BYT)nat)
#define	BOL2VAL(nat)	    ((nat)?Qtrue:Qfalse)
#define	SRT2VAL(nat)	    INT2FIX((MQ_SRT)nat)
#define	INT2VAL(nat)	    INT2FIX((MQ_INT)nat)
#define	WID2VAL(nat)	    LL2NUM((MQ_WID)nat)
#define	FLT2VAL(nat)	    DBL2NUM((double)nat)
#define	DBL2VAL(nat)	    DBL2NUM((double)nat)
#define	CST2VAL(nat)	    (nat != NULL ? rb_str_new2(nat) : Qnil)
#define	PTR2VAL(nat)	    (nat != NULL ? (VALUE)(nat) : Qnil)
#define BIN2VAL(ptr,len)    rb_str_buf_cat(rb_str_buf_new(0),(char*)ptr,len)
#define	MqS2VAL(nat)	    (nat != NULL ? ((VALUE)(nat)->self) : Qnil)

#define CheckType(val,typ,err) \
  if (rb_obj_is_kind_of(val, typ) == Qfalse) rb_raise(rb_eTypeError, err);

#define printVAL(val) rb_io_puts(1, &val, rb_stderr); rb_io_flush(rb_stderr);

/*****************************************************************************/
/*                                                                           */
/*                                 Init's                                    */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Init)	    (void);
void NS(MqS_Error_Init)	    (void);
void NS(MqS_Read_Init)	    (void);
void NS(MqS_Send_Init)	    (void);
void NS(MqS_Config_Init)    (void);
void NS(MqS_Service_Init)   (void);
void NS(MqS_Link_Init)	    (void);
void NS(MqS_Slave_Init)	    (void);
void NS(MqS_Sys_Init)	    (void);
void NS(MqS_Api_Init)	    (void);

void NS(MqSException_Init)  (void);

void NS(MqBufferS_Init)	    (void);
VALUE NS(MqBufferS_New)	    (MQ_BUF);

/*****************************************************************************/
/*                                                                           */
/*                                  Misc's                                   */
/*                                                                           */
/*****************************************************************************/

void NS(MqSException_Raise) (struct MqS*);
void NS(MqSException_Set)   (struct MqS*, VALUE);
enum MqErrorE NS(ProcCall)  (struct MqS * const , MQ_PTR const);
void NS(ProcFree)	    (struct MqS const * const, MQ_CST, MQ_PTR *);
enum MqErrorE NS(ProcCopy)  (struct MqS * const, MQ_PTR *);
MQ_BFL NS(argv2bufl)	    (int argc, VALUE *argv);
enum MqErrorE NS(ProcInit)  (struct MqS*, VALUE, MqServiceCallbackF*, MQ_PTR*, MqTokenDataCopyF*);
VALUE NS(Rescue)	    (struct MqS * const, VALUE(*)(ANYARGS), VALUE);

#define NIL_Check(v)	    if (NIL_P(v)) goto error;
