/**
 *  \file       theLink/phpmsgque/PhpMsgque/msgque_php.h
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_PhpMsgque.h"
#include "msgque.h"
#include "debug.h"

extern int le_MqS;

/*****************************************************************************/
/*                                                                           */
/*                                 ref counting                              */
/*                                                                           */
/*****************************************************************************/

#define LIBMSGQUE_VERSION   "4.6"
#define NS(n)               PhpMsgque_ ## n
#define MQCTX               VAL2MqS(getThis() TSRMLS_CC)
#define SETUP_mqctx         struct MqS * mqctx = VAL2MqS(this_ptr TSRMLS_CC)
#define SELF                MqS2VAL(mqctx)
#define SETUP_self          zval * this_ptr = SELF
#define MQ_CONTEXT_S        mqctx


#define INCR_REG(val) 
#define DECR_REG(val)

static inline struct MqS* VAL2MqS(zval* val TSRMLS_DC) {
  zval **rsrc;
  if(zend_hash_find(HASH_OF(val), "_MqS_ptr", sizeof("_MqS_ptr"), (void **) &rsrc) == SUCCESS) 
  {
    return (struct MqS *) zend_fetch_resource(rsrc TSRMLS_CC, -1, "MqS", NULL, 1, le_MqS);
  }
  return NULL;
}

/*****************************************************************************/
/*                                                                           */
/*                                 definition's                              */
/*                                                                           */
/*****************************************************************************/

#define ErrorMqToPhp() NS(MqSException_Raise)(mqctx TSRMLS_CC)
#define ErrorMqToPhpWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorMqToPhp(); \
  }
#define ErrorCheck(val) if ((val) == FAILURE) goto error

#define VAL2BYT(val)	    (MQ_BYT)Z_LVAL_P(val)
#define VAL2BOL(val)	    (MQ_BOL)Z_BVAL_P(val)
#define VAL2SRT(val)	    (MQ_SRT)Z_LVAL_P(val)
#define VAL2INT(val)	    (MQ_INT)Z_LVAL_P(val)
#define VAL2WID(val)	    (MQ_WID)Z_LVAL_P(val)
#define VAL2FLT(val)	    (MQ_FLT)Z_DVAL_P(val)
#define VAL2DBL(val)	    (MQ_DBL)Z_DVAL_P(val)
#define VAL2CST(val)	    (MQ_CST)Z_STRVAL_P(val)
#define VAL2PTR(val)	    (MQ_PTR)(val)
#define VALP2CST(valp)	    (MQ_CST)rb_string_value_cstr(valp)
#define VAL2BIN(val)	    (MQ_CBI)Z_STRVAL_P(val),Z_STRLEN_P(val)
#define VAL2MqBufferS(val)  (MQ_BUF)DATA_PTR(val)

#define	BYT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	BOL2VAL(zval,nat)	    ZVAL_BOOL(zval,nat)
#define	SRT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	INT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	WID2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	FLT2VAL(zval,nat)	    ZVAL_DOUBLE(zval,(double)nat)
#define	DBL2VAL(zval,nat)	    ZVAL_STRING(zval,(double)nat)
#define	CST2VAL(zval,nat)	    ZVAL_STRING(zval,nat,TRUE)
#define	PTR2VAL(zval,nat)	    (nat != NULL ? ZVAL_RESOURCE(zval,nat) : ZVAL_NULL(zval))
#define BIN2VAL(zval,ptr,len)	    rb_str_buf_cat(rb_str_buf_new(0),(char*)ptr,len)
#define	MqS2VAL(val,nat)	    \
if (nat != NULL) { \
  zval *tmp = (zval*)(nat)->self; \
  ZVAL_ZVAL(val,tmp,0,0); \
} else { \
  ZVAL_NULL(val); \
}

#define CheckType(val,typ,err) \
  if (rb_obj_is_kind_of(val, typ) == Qfalse) rb_raise(rb_eTypeError, err);

#define printVAL(val) {VALUE tmp=(val); rb_io_puts(1, &tmp, rb_stderr); rb_io_flush(rb_stderr);}

/*****************************************************************************/
/*                                                                           */
/*                                 Init's                                    */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Init)	    (TSRMLS_D);
void NS(MqS_Error_Init)	    (TSRMLS_D);
void NS(MqS_Read_Init)	    (TSRMLS_D);
void NS(MqS_Send_Init)	    (TSRMLS_D);
void NS(MqS_Config_Init)    (TSRMLS_D);
void NS(MqS_Service_Init)   (TSRMLS_D);
void NS(MqS_Link_Init)	    (TSRMLS_D);
void NS(MqS_Slave_Init)	    (TSRMLS_D);
void NS(MqS_Sys_Init)	    (TSRMLS_D);

void NS(MqSException_Init)  (TSRMLS_D);

void NS(MqBufferS_Init)	    (TSRMLS_D);
//VALUE NS(MqBufferS_New)	    (MQ_BUF);

/*****************************************************************************/
/*                                                                           */
/*                                  Misc's                                   */
/*                                                                           */
/*****************************************************************************/

void NS(MqSException_Raise) (struct MqS* TSRMLS_DC);
/*
void NS(MqSException_Set)   (struct MqS*, VALUE);
enum MqErrorE NS(ProcCall)  (struct MqS * const , MQ_PTR const);
void NS(ProcFree)	    (struct MqS const * const, MQ_PTR *);
enum MqErrorE NS(ProcCopy)  (struct MqS * const, MQ_PTR *);
MQ_BFL NS(argv2bufl)	    (struct MqBufferLS*, int, VALUE*);
enum MqErrorE NS(ProcInit)  (struct MqS*, VALUE, MqServiceCallbackF*, MQ_PTR*, MqTokenDataCopyF*);
VALUE NS(Rescue)	    (struct MqS * const, VALUE(*)(ANYARGS), VALUE);
*/

#define NIL_Check(v)	    if (NIL_P(v)) goto error;
