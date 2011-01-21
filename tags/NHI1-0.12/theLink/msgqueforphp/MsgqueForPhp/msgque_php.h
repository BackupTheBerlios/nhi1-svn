/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/msgque_php.h
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
#include "ext/standard/php_var.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "php_MsgqueForPhp.h"
#include "msgque.h"
#include "debug.h"

/*****************************************************************************/
/*                                                                           */
/*                                 ref counting                              */
/*                                                                           */
/*****************************************************************************/

#define NS(n)               MsgqueForPhp_ ## n

extern zend_class_entry *NS(MqS);
extern zend_class_entry *NS(MqBufferS);

#define INCR_REG(val)	    zval_addref_p(val)
#define DECR_REG(val)	    zval_delref_p(val)

/*
#define INCR_REG(val) \
  MqDLogV(mqctx,0,"INCR_REG(%d): self<%p>, refCount<%d>\n", __LINE__, val, Z_REFCOUNT_P(val)); \
  zval_addref_p(val)
  
#define DECR_REG(val) \
  MqDLogV(mqctx,0,"DECR_REG(%d): self<%p>, refCount<%d>\n", __LINE__, val, Z_REFCOUNT_P(val)); \
  zval_delref_p(val)
*/

#define LIBMSGQUE_VERSION   "4.6"
#define SETUP_mqctx         struct MqS * mqctx; VAL2MqS(mqctx,getThis());
#define SELF                ((zval*)(mqctx)->self)
#define SETUP_self          zval * self = SELF
#define MQ_CONTEXT_S        mqctx
#define ID(name)	    #name,sizeof(#name)-1
#define ID2(name)	    #name,sizeof(#name)
#define printVAL(val)	    php_var_dump(&val,2 TSRMLS_CC);

/*****************************************************************************/
/*                                                                           */
/*                                 definition's                              */
/*                                                                           */
/*****************************************************************************/

#define ErrorMqToPhp() NS(MqSException_Raise)(mqctx TSRMLS_CC); return
#define ErrorMqToPhpWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorMqToPhp(); \
  }
#define PhpErrorCheck(val) if ((val) == FAILURE) goto error

#define RaiseError(msg)	    zend_throw_exception(zend_exception_get_default(TSRMLS_C),(MQ_STR)msg,1 TSRMLS_CC);
#define RETURN_ERROR(msg)   RaiseError(msg);return;

#define CheckType(val,typ) if (!instanceof_function(Z_OBJCE_P(val), typ TSRMLS_CC)) goto error;

#define VAL2BYT(val)	    (MQ_BYT)Z_LVAL_P(val)
#define VAL2BOL(val)	    (MQ_BOL)Z_BVAL_P(val)
#define VAL2SRT(val)	    (MQ_SRT)Z_LVAL_P(val)
#define VAL2INT(val)	    (MQ_INT)Z_LVAL_P(val)
#define VAL2WID(val)	    (MQ_WID)Z_LVAL_P(val)
#define VAL2FLT(val)	    (MQ_FLT)Z_DVAL_P(val)
#define VAL2DBL(val)	    (MQ_DBL)Z_DVAL_P(val)
#define VAL2CST(val)	    (MQ_CST)Z_STRVAL_P(val)
#define VAL2BIN(val)	    (MQ_CBI)Z_STRVAL_P(val),(MQ_SIZE)Z_STRLEN_P(val)
#define VAL2MqS2(val)	    (struct MqS*)Z_LVAL_P(zend_read_property(NS(MqS), val, ID(__ctx), 0 TSRMLS_CC))
#define VAL2MqBufferS2(val) (struct MqBufferS*)Z_LVAL_P(zend_read_property(NS(MqBufferS), val, ID(__buf), 0 TSRMLS_CC))

#define VAL2MqS(tgt, src) { \
  zval * zret = zend_read_property(NS(MqS), src, ID(__ctx), 0 TSRMLS_CC); \
  if (Z_TYPE_P(zret) == IS_NULL) { \
    RETURN_ERROR("MqS resource was not initialized, is the constructor not run ?"); \
  } else { \
    tgt = (struct MqS *) Z_RESVAL_P(zret); \
  } \
}

#define VAL2MqBufferS(tgt, src) { \
  zval * zret = zend_read_property(NS(MqBufferS), src, ID(__buf), 0 TSRMLS_CC); \
  if (Z_TYPE_P(zret) == IS_NULL) { \
    RETURN_ERROR("MqBufferS resource was not initialized, is the constructor not run ?"); \
  } else { \
    tgt = (struct MqBufferS *) Z_RESVAL_P(zret); \
  } \
}

#define	BYT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	BOL2VAL(zval,nat)	    ZVAL_BOOL(zval,nat)
#define	SRT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	INT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	WID2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	FLT2VAL(zval,nat)	    ZVAL_DOUBLE(zval,(double)nat)
#define	DBL2VAL(zval,nat)	    ZVAL_DOUBLE(zval,(double)nat)
#define	CST2VAL(zval,nat)	    ZVAL_STRING(zval,(nat?nat:""),1)
#define	BIN2VAL(zval,ptr,len)	    ZVAL_STRINGL(zval,ptr,len,1)
#define	MqS2VAL(val,nat)	    \
if (nat != NULL) { \
  zval *tmp = (zval*)nat->self; \
  ZVAL_ZVAL(val,tmp,1,0); \
} else { \
  ZVAL_NULL(val); \
}

#define ARG2INT(mth,val) \
long val;\
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) { \
  RETURN_ERROR("usage: " #mth "(integer:" #val ")"); \
  return; \
}
#define ARG2BYT(mth,val) ARG2INT(mth,val)
#define ARG2SRT(mth,val) ARG2INT(mth,val)
#define ARG2WID(mth,val) ARG2INT(mth,val)

#define ARG2DBL(mth,val) \
double val;\
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &val) == FAILURE) { \
  RETURN_ERROR("usage: " #mth "(double:" #val ")"); \
  return; \
}
#define ARG2FLT(mth,val) ARG2DBL(mth,val)


#define ARG2CST(mth,val) \
MQ_CST val; int val ## len;\
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &val, & val ## len) == FAILURE) { \
  RETURN_ERROR("usage: " #mth "(string:" #val ")"); \
}

#define ARG2BOL(mth,val) \
zend_bool val;\
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &val) == FAILURE) { \
  RETURN_ERROR("usage: " #mth "(boolean:" #val ")"); \
}

#define ARG2OBJ(mth,val) \
zval *val;\
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &val) == FAILURE) { \
  RETURN_ERROR("usage: " #mth "(object:" #val ")"); \
}

#define ARG2MqS(mth,val) \
struct MqS *val;\
zval *_val;\
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &_val) == FAILURE) { \
  RETURN_ERROR("usage: " #mth "(object:" #val ")"); \
} \
VAL2MqS(val,_val);

#define ARG2MqBufferS(mth,val) \
struct MqBufferS *val;\
zval *_val;\
if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &_val) == FAILURE) { \
  RETURN_ERROR("usage: " #mth "(object:" #val ")"); \
} \
VAL2MqBufferS(val,_val);

/*****************************************************************************/
/*                                                                           */
/*                                  Misc's                                   */
/*                                                                           */
/*****************************************************************************/

void NS(MqSException_Raise)	  (struct MqS* TSRMLS_DC);
void NS(MqSException_Set)	  (struct MqS*, zval* TSRMLS_DC);
MQ_BFL NS(Argument2MqBufferLS)	  (struct MqBufferLS *, int numArgs TSRMLS_DC);
void NS(MqBufferLAppendZVal)	  (MQ_BFL, zval* TSRMLS_DC);
void NS(MqBufferS_New)		  (zval *, MQ_BUF TSRMLS_DC);


/*****************************************************************************/
/*                                                                           */
/*                               Callback's                                  */
/*                                                                           */
/*****************************************************************************/

struct NS(ProcDataS) {
  HashTable	*function_table;
  zval		*ctor;
};

enum MqErrorE NS(ProcInit) (
  struct MqS * const  mqctx, 
  zval *		    callable,
  void **		    dataP,
  MqTokenF *		    tokenFP,
  MqDataFreeF *	    tokenDataFreeFP,
  MqDataCopyF *	    tokenDataCopyFP
  TSRMLS_DC
);

enum MqErrorE NS(ProcCall) (
  struct MqS * const		mqctx, 
  struct NS(ProcDataS) * const	data,
  zend_uint			param_count, 
  zval**			params[],
  zval**			resultP
);


#define NIL_Check(v)	    if (NIL_P(v)) goto error;









