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
#include "zend_interfaces.h"
#include "php_PhpMsgque.h"
#include "msgque.h"
#include "debug.h"

extern zend_class_entry *PhpMsgque_MqS;

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

#define ID(name)	    #name,sizeof(#name)-1

/*
static inline void printVALex(zval *val TSRMLS_DC) {
  zend_eval_string("echo \"printVAL -> \", serialize($ctx), \"\\n\";", NULL, "11111" TSRMLS_CC);
}
*/

#define printVAL(val) php_var_dump(&val,2 TSRMLS_CC)

#define VAL2MqS(val)  (struct MqS*)Z_LVAL_P(zend_read_property(PhpMsgque_MqS, getThis(), ID(mqctx), 0 TSRMLS_CC))

/*
static inline struct MqS* VAL2MqS(zval* val TSRMLS_DC) {
  zval **rsrc;
M0
printP(val)
printVAL(val);
  if(zend_hash_find(HASH_OF(val), ID(_MqS_ptr), (void **) &rsrc) == SUCCESS) 
  {
M1
    return (struct MqS *) zend_fetch_resource(rsrc TSRMLS_CC, -1, "MqS", NULL, 1, le_MqS);
  }
M2
  zend_error(E_ERROR, "unable to extract 'struct MqS *' from 'zval *'");
M3
  return NULL;
}
*/

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
#define	CST2VAL(zval,nat)	    ZVAL_STRING(zval,nat,1)
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

/*****************************************************************************/
/*                                                                           */
/*                                  Misc's                                   */
/*                                                                           */
/*****************************************************************************/

void NS(MqSException_Raise) (struct MqS* TSRMLS_DC);
void NS(MqSException_Set)   (struct MqS*, zval* TSRMLS_DC);
/*
enum MqErrorE NS(ProcCall)  (struct MqS * const , MQ_PTR const);
void NS(ProcFree)	    (struct MqS const * const, MQ_PTR *);
enum MqErrorE NS(ProcCopy)  (struct MqS * const, MQ_PTR *);
MQ_BFL NS(argv2bufl)	    (struct MqBufferLS*, int, VALUE*);
enum MqErrorE NS(ProcInit)  (struct MqS*, VALUE, MqServiceCallbackF*, MQ_PTR*, MqTokenDataCopyF*);
VALUE NS(Rescue)	    (struct MqS * const, VALUE(*)(ANYARGS), VALUE);
*/

#define NIL_Check(v)	    if (NIL_P(v)) goto error;
