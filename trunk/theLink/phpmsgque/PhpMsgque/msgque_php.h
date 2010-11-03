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
#include "ext/standard/php_var.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "php_PhpMsgque.h"
#include "msgque.h"
#include "debug.h"

/*****************************************************************************/
/*                                                                           */
/*                                 ref counting                              */
/*                                                                           */
/*****************************************************************************/

#define NS(n)               PhpMsgque_ ## n

extern zend_class_entry *NS(MqS);
extern zend_class_entry *NS(MqBufferS);

#define LIBMSGQUE_VERSION   "4.6"
#define MQCTX               VAL2MqS(getThis())
#define SETUP_mqctx         struct MqS * mqctx = MQCTX
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

#define ErrorMqToPhp() NS(MqSException_Raise)(mqctx TSRMLS_CC)
#define ErrorMqToPhpWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorMqToPhp(); \
  }
#define PhpErrorCheck(val) if ((val) == FAILURE) goto error

#define RaiseError(msg)	    zend_throw_exception(zend_exception_get_default(TSRMLS_C),msg,1 TSRMLS_CC);
//#define RaiseError(msg)	    zend_throw_exception(zend_get_error_exception(TSRMLS_C),msg,1 TSRMLS_CC);

#define CheckType(val,typ,err) \
  if (instanceof_function(Z_OBJCE_P(val), typ TSRMLS_CC)) RaiseError(err)

#define VAL2BYT(val)	    (MQ_BYT)Z_LVAL_P(val)
#define VAL2BOL(val)	    (MQ_BOL)Z_BVAL_P(val)
#define VAL2SRT(val)	    (MQ_SRT)Z_LVAL_P(val)
#define VAL2INT(val)	    (MQ_INT)Z_LVAL_P(val)
#define VAL2WID(val)	    (MQ_WID)Z_LVAL_P(val)
#define VAL2FLT(val)	    (MQ_FLT)Z_DVAL_P(val)
#define VAL2DBL(val)	    (MQ_DBL)Z_DVAL_P(val)
#define VAL2CST(val)	    (MQ_CST)Z_STRVAL_P(val)
#define VAL2BIN(val)	    (MQ_CBI)Z_STRVAL_P(val),(MQ_SIZE)Z_STRLEN_P(val)
#define VAL2MqS(val)	    (struct MqS*)Z_LVAL_P(zend_read_property(NS(MqS), val, ID(__ctx), 0 TSRMLS_CC))
#define VAL2MqBufferS(val)  (struct MqBufferS*)Z_LVAL_P(zend_read_property(NS(MqBufferS), val, ID(__buf), 0 TSRMLS_CC))


#define	BYT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	BOL2VAL(zval,nat)	    ZVAL_BOOL(zval,nat)
#define	SRT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	INT2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	WID2VAL(zval,nat)	    ZVAL_LONG(zval,(long)nat)
#define	FLT2VAL(zval,nat)	    ZVAL_DOUBLE(zval,(double)nat)
#define	DBL2VAL(zval,nat)	    ZVAL_DOUBLE(zval,(double)nat)
#define	CST2VAL(zval,nat)	    ZVAL_STRING(zval,nat,1)
#define	BIN2VAL(zval,ptr,len)	    ZVAL_STRINGL(zval,ptr,len,1)
#define	MqS2VAL(val,nat)	    \
if (nat != NULL) { \
  zval *tmp = (zval*)nat->self; \
  ZVAL_ZVAL(val,tmp,0,0); \
} else { \
  ZVAL_NULL(val); \
}

#define ARG2INT(mth,val) \
long val;\
if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) { \
  RaiseError("usage: " #mth "(long:" #val ")"); \
  return; \
}
#define ARG2BYT(mth,val) ARG2INT(mth,val)
#define ARG2SRT(mth,val) ARG2INT(mth,val)
#define ARG2WID(mth,val) ARG2INT(mth,val)

#define ARG2DBL(mth,val) \
double val;\
if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "d", &val) == FAILURE) { \
  RaiseError("usage: " #mth "(double:" #val ")"); \
  return; \
}
#define ARG2FLT(mth,val) ARG2DBL(mth,val)


#define ARG2CST(mth,val) \
MQ_CST val; int val ## len;\
if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &val, & val ## len) == FAILURE) { \
  RaiseError("usage: " #mth "(string:" #val ")"); \
  return; \
}

#define ARG2BOL(mth,val) \
zend_bool val;\
if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "b", &val) == FAILURE) { \
  RaiseError("usage: " #mth "(boolean:" #val ")"); \
  return; \
}

#define ARG2OBJ(mth,val) \
zval *val;\
if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "o", &val) == FAILURE) { \
  RaiseError("usage: " #mth "(object:" #val ")"); \
  return; \
}

/*****************************************************************************/
/*                                                                           */
/*                                  Misc's                                   */
/*                                                                           */
/*****************************************************************************/

void NS(MqSException_Raise)	  (struct MqS* TSRMLS_DC);
void NS(MqSException_Set)	  (struct MqS*, zval* TSRMLS_DC);
MQ_BFL NS(Argument2MqBufferLS)	  (int numArgs TSRMLS_DC);
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
  MqTokenDataFreeF *	    tokenDataFreeFP,
  MqTokenDataCopyF *	    tokenDataCopyFP
  TSRMLS_DC
);

#define NIL_Check(v)	    if (NIL_P(v)) goto error;
