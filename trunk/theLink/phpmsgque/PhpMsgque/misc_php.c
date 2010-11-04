/**
 *  \file       theLink/phpmsgque/PhpMsgque/misc_php.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_php.h"

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

static int MqBufferLAppendZValArrayItem (zval **, MQ_BFL TSRMLS_DC);

void NS(MqBufferLAppendZVal)(MQ_BFL bufL, zval* arg TSRMLS_DC) {
  switch (Z_TYPE_P(arg)) {
    case IS_STRING:
      MqBufferLAppendC(bufL, Z_STRVAL_P(arg));
      break;
    case IS_BOOL:
      MqBufferLAppendO(bufL, Z_LVAL_P(arg) ? 1 : 0);
      break;
    case IS_LONG:
      MqBufferLAppendI(bufL, Z_LVAL_P(arg));
      break;
    case IS_DOUBLE:
      MqBufferLAppendD(bufL, Z_DVAL_P(arg));
      break;
    case IS_ARRAY:
      zend_hash_apply_with_argument(Z_ARRVAL_P(arg), 
	(apply_func_arg_t) MqBufferLAppendZValArrayItem, bufL TSRMLS_CC);
      break;
    default:
      RaiseError("invalid argument type");
      break;
  }
}

static int MqBufferLAppendZValArrayItem (zval **argP, MQ_BFL args TSRMLS_DC) {
  NS(MqBufferLAppendZVal) (args, *argP TSRMLS_CC);
  return ZEND_HASH_APPLY_KEEP;
}

MQ_BFL NS(Argument2MqBufferLS)(const int numArgs TSRMLS_DC)
{
  struct MqBufferLS * args = NULL;

  if (numArgs > 0) {
    int i;
    zval ***arguments;
    args = MqBufferLCreate(numArgs);
    arguments = emalloc(sizeof(zval**) * numArgs);
    if (zend_get_parameters_array_ex(numArgs,arguments)) {
      ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(args);
    }
    for (i=0; i<numArgs; i++) {
      NS(MqBufferLAppendZVal) (args, *arguments[i] TSRMLS_CC);
    }
    efree(arguments);
  }

  return args;
}

enum MqErrorE NS(ProcCall) (
  struct MqS * const		mqctx, 
  struct NS(ProcDataS) * const	data,
  zend_uint			param_count, 
  zval**			params[],
  zval**			resultP
)
{
  TSRMLS_FETCH_FROM_CTX(mqctx->threadData);
  //TSRMLS_FETCH();
  zval *result = NULL;
  if (resultP) *resultP = NULL;
  zend_clear_exception(TSRMLS_C);
  MQ_BOL bailout = MQ_NO;
  int ret;
  zend_error_handling  original_error_handling;
  // throw 'exception' on error
  zend_replace_error_handling(EH_THROW, zend_get_error_exception(TSRMLS_C), &original_error_handling TSRMLS_CC);
  // call function or method
  ret = call_user_function_ex(data->function_table, NULL, data->ctor, &result, 
    param_count, params, 1, NULL TSRMLS_CC);
  // clear or save result
  if (result) {
    if (resultP) {
      *resultP = result;
    } else {
      zval_dtor(result);
    }
  }
  // restore old exception handler
  zend_restore_error_handling(&original_error_handling TSRMLS_CC);
  // Error result ?
  PhpErrorCheck(ret);
  // does an 'Exception' happen?
  if (EG(exception)) {
    NS(MqSException_Set) (mqctx, EG(exception) TSRMLS_CC);
  }
  return MqErrorGetCode(mqctx);
error:
  return MqErrorC(mqctx, "PHP-Fatal-Error", -1, "unknown");
}

static enum MqErrorE ProcCallNoArg (struct MqS * const mqctx, struct NS(ProcDataS) * const data)
{
  return NS(ProcCall)(mqctx,data,0,NULL,NULL);
}

static enum MqErrorE ProcCallOneArg (struct MqS * const mqctx, struct NS(ProcDataS) * const data)
{
  zval *self = mqctx->self;
  zval_addref_p(self);
  zval **selfP = &self;
  enum MqErrorE ret = NS(ProcCall)(mqctx,data,1,&selfP,NULL);
  zval_delref_p(self);
  return ret;
}

static void ProcFree (struct MqS const * const mqctx, struct NS(ProcDataS) ** dataP)
{
  TSRMLS_FETCH_FROM_CTX(mqctx->threadData);
  //TSRMLS_FETCH();
  zval_ptr_dtor(&(*dataP)->ctor);
  efree(*dataP);
  *dataP = NULL;
}

enum MqErrorE NS(ProcInit) (
  struct MqS * const  mqctx, 
  zval *	      callable,
  void **	      dataP,
  MqTokenF *	      tokenFP,
  MqTokenDataFreeF *  tokenDataFreeFP,
  MqTokenDataCopyF *  tokenDataCopyFP
  TSRMLS_DC
)
{
  // INIT:
  // PHP_METHOD(PhpMsgque_MqS, __construct) set the callback's during construct -> allways
  // up-to-data -> no "copy" constructor needed
  char *error;
  struct NS(ProcDataS) *data = (*dataP) = emalloc(sizeof(struct NS(ProcDataS)));
  // 1. is the "callable" call-able?
  if (!zend_is_callable_ex(callable, NULL, 0, NULL, NULL, NULL, &error TSRMLS_CC)) {
    return MqErrorC(mqctx, __func__, -1, error);
  }
  // 2. is "callable" an method?
  if (Z_TYPE_P(callable) == IS_ARRAY) {
    // YES
    zval **obj;
    zend_class_entry *ce;
    // 3. get "object" from "method"
    if (zend_hash_index_find(Z_ARRVAL_P(callable), 0, (void**) &obj) == FAILURE) {
      return MqErrorC(mqctx, __func__, -1, "unable to extract object from callback");
    }
    // 4. belongs "object" to local "MqS" instance?
    ce = zend_get_class_entry(*obj TSRMLS_CC);
    if (instanceof_function(ce, NS(MqS) TSRMLS_CC) && VAL2MqS(*obj) == mqctx) {
      // YES, no "this" argument needed
      *tokenFP		= (MqTokenF) ProcCallNoArg;
      *tokenDataFreeFP	= (MqTokenDataFreeF) ProcFree;
      if (tokenDataCopyFP) *tokenDataCopyFP = (MqTokenDataCopyF) NULL;
    } else {
      // NO, "this" argument needed
      *tokenFP		= (MqTokenF) ProcCallOneArg;
      *tokenDataFreeFP	= (MqTokenDataFreeF) ProcFree;
      if (tokenDataCopyFP) *tokenDataCopyFP = (MqTokenDataCopyF) NULL;
    }
    data->function_table = &ce->function_table;
  } else {
    // NO
      *tokenFP		= (MqTokenF) ProcCallOneArg;
      *tokenDataFreeFP	= (MqTokenDataFreeF) ProcFree;
      if (tokenDataCopyFP) *tokenDataCopyFP = (MqTokenDataCopyF) NULL;
    data->function_table = CG(function_table);
  }
  zval_addref_p(callable);
  data->ctor = callable;
  return MQ_OK;
}


/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(Misc_Init)(TSRMLS_D) {
}

