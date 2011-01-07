/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/factory_php.c
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
#include "zend_exceptions.h"
#include "standard/php_string.h"

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
FactoryCreate(
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS * item,
  struct MqS  ** mqctxP
)
{

/*
  if (create == MQ_FACTORY_NEW_FORK) {
    //rb_thread_atfork();
  }
*/

/*
  //php seems NOT to support external thread creation

  void ***tsrm_ls;
  if (create == MQ_FACTORY_NEW_THREAD) {
    tsrm_set_interpreter_context(tsrm_new_interpreter_context());
    tsrm_ls = (void ***) ts_resource_ex(0, NULL);
  } else {
    tsrm_ls = (void ***) tmpl->threadData;
  }
  //TSRMLS_FETCH();
}
*/

  TSRMLS_FETCH_FROM_CTX(create == MQ_FACTORY_NEW_INIT ? tmpl : tmpl->threadData);
  zval *self;
  zend_class_entry *ce = (zend_class_entry*) item->Create.data;
  enum MqErrorE mqret = MQ_OK;
  struct MqS * mqctx = NULL;
  MQ_CST err = NULL;

  /* Object has constructor */
  zval *zctx = NULL, *ztmpl=NULL, *zdummy=NULL;
  zval **params[1];
  zend_fcall_info fci;
  zend_fcall_info_cache fcc;

  /* setup the template parameter */
  if (create == MQ_FACTORY_NEW_INIT) {
    MAKE_STD_ZVAL(ztmpl);
    ZVAL_NULL(ztmpl);
  } else {
    ztmpl = (zval*) tmpl->self;
  }
  zval_add_ref(&ztmpl);
  params[0] = &ztmpl;

  /* create object */
  MAKE_STD_ZVAL(zctx);
  if (object_init_ex(zctx, ce) == FAILURE) {
    err = "Unable to initialize the new object";
    goto end;
  }

  /* call constructor */
  fci.size = sizeof(fci);
  fci.function_table = EG(function_table);
  fci.function_name = NULL;
  fci.symbol_table = NULL;
  fci.object_ptr = zctx;
  fci.retval_ptr_ptr = &zdummy;
  fci.param_count = 1;
  fci.params = params;
  fci.no_separation = 1;

  fcc.initialized = 1;
  fcc.function_handler = ce->constructor;
  fcc.calling_scope = EG(scope);
  fcc.called_scope = Z_OBJCE_P(zctx);
  fcc.object_ptr = zctx;

  if (zend_call_function(&fci, &fcc TSRMLS_CC) == FAILURE) {
    err = "Unable to call constructor";
    goto end;
  }

  /* extract MqS object */
  mqctx = VAL2MqS2(zctx);
  if (mqctx == NULL) {
    err = "MqS resource is not initialized, constructor not run ?";
    goto end;
  }
  if (MqErrorCheckI (MqErrorGetCode (mqctx = VAL2MqS2(zctx)))) {
    if (create != MQ_FACTORY_NEW_INIT) {
      MqErrorCopy (tmpl, mqctx);
      mqret = MqErrorStack(tmpl);
      goto end;
    } else {
      err = MqErrorGetText(mqctx);
      goto end;
    }
  }

  if (create != MQ_FACTORY_NEW_INIT) {
    MqSetupDup(mqctx, tmpl);
  }

end:
  *mqctxP = mqctx;
  if (zctx) zval_ptr_dtor(&zctx);
  if (zdummy) zval_ptr_dtor(&zdummy);
  if (ztmpl) zval_ptr_dtor(&ztmpl);
  if (err) {
    if (create == MQ_FACTORY_NEW_INIT) {
      RaiseError(err);
      mqret = MQ_ERROR;
    } else {
      mqret = MqErrorC(tmpl, __func__, -1, err);
    }
  }
  return mqret;
}

static void
FactoryDelete(
  struct MqS * mqctx,
  MQ_BOL doFactoryDelete,
  struct MqFactoryS * const item
)
{ 
  MqContextFree (mqctx);
  if (doFactoryDelete && mqctx->self != NULL) {
    DECR_REG(mqctx->self);
    mqctx->self = NULL;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxIdentGet)
{
  SETUP_mqctx;
  CST2VAL(return_value, MqFactoryCtxIdentGet (mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxIdentSet)
{
  SETUP_mqctx;
  ARG2CST(FactoryCtxIdentSet,val);
  ErrorMqToPhpWithCheck(MqFactoryCtxIdentSet(mqctx, (MQ_CST) (val)));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxDefaultSet)
{
  SETUP_mqctx;
  ARG2CST(FactoryCtxIdentSet,val);
  ErrorMqToPhpWithCheck(MqFactoryCtxDefaultSet(mqctx, (MQ_CST) (val)));
  RETURN_NULL();
}

/*****************************************************************************/
/*                                                                           */
/*                                static                                     */
/*                                                                           */
/*****************************************************************************/

#define FactorySetup(N) \
  MQ_CST err = NULL; \
  struct MqS * mqctx = NULL; \
  enum MqFactoryReturnE ret; \
  zend_class_entry *ce; \
  zend_class_entry **cep; \
  char *class=NULL, *ident=NULL; \
  int classlen, identlen; \
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &ident, &identlen, &class, &classlen) == FAILURE) { \
    err = "usage: " #N "(?ident?, class)"; \
    goto end; \
  } \
  /* check/setup 'ident' */ \
  if (ident == NULL) { \
    err = MqFactoryErrorMsg(MQ_FACTORY_RETURN_INVALID_IDENT); \
    goto end; \
  } \
  /* check/setup 'class' */ \
  if (class == NULL) { \
    class = ident; \
    classlen = identlen; \
  } \
  /* class names are stored in lowercase */ \
  class = estrndup(class, classlen); \
  php_strtolower(class, classlen); \
  /* does the class names exists ? */ \
  if (zend_lookup_class(class, classlen, &cep TSRMLS_CC) == FAILURE) { \
    err = "Class does not exist."; goto end; \
  } \
  ce = *cep; \
  /* check if the constructor is available */ \
  if (ce->constructor == NULL) { \
    err = "Class constructor does not exist."; goto end; \
  } \

PHP_FUNCTION(FactoryAdd)
{
  FactorySetup(FactoryAdd);
  ret = MqFactoryAdd(ident, FactoryCreate, (MQ_PTR) ce, NULL, FactoryDelete, NULL, NULL);
  if (MqFactoryErrorCheckI(ret)) {
    err = (MQ_STR) MqFactoryErrorMsg(ret); goto end;
  }
end:
  if (class) efree(class);
  if (err) {
    RETURN_ERROR(err)
  } else {
    RETURN_NULL();
  }
}

PHP_FUNCTION(FactoryNew)
{
  FactorySetup(FactoryNew);
  ret = MqFactoryNew(ident, FactoryCreate, (MQ_PTR) ce, NULL, FactoryDelete, NULL, NULL, (MQ_PTR)tsrm_ls, &mqctx);
  if (MqFactoryErrorCheckI(ret)) {
    err = (MQ_STR) MqFactoryErrorMsg(ret); goto end;
  }
end:
  if (class) efree(class);
  if (err) {
    RETURN_ERROR(err)
  } else {
    MqS2VAL(return_value, mqctx);
    return;
  }
}

PHP_FUNCTION(FactoryCall)
{
  struct MqS * mqctx = NULL;
  enum MqFactoryReturnE ret;
  ARG2CST(FactoryCall,ident);
  ret = MqFactoryCall(ident, (MQ_PTR)tsrm_ls, &mqctx);
  if (MqFactoryErrorCheckI(ret)) {
    RETURN_ERROR(MqFactoryErrorMsg(ret));
  } else {
    MqS2VAL(return_value, mqctx);
    return;
  }
}

PHP_FUNCTION(FactoryDefault)
{
  FactorySetup(FactoryDefault);
  ret = MqFactoryDefault(ident, FactoryCreate, (MQ_PTR) ce, NULL, FactoryDelete, NULL, NULL);
  if (MqFactoryErrorCheckI(ret)) {
    err = (MQ_STR) MqFactoryErrorMsg(ret); goto end;
  }
end:
  if (class) efree(class);
  if (err) {
    RETURN_ERROR(err)
  } else {
    RETURN_NULL();
  }
}

PHP_FUNCTION(FactoryDefaultIdent)
{
  CST2VAL(return_value, MqFactoryDefaultIdent());
  return;
}

void NS(Factory_Init)(TSRMLS_D) {
  MQ_CST err = NULL;
  enum MqFactoryReturnE ret;
  ret = MqFactoryDefault("phpmsgque", FactoryCreate, (MQ_PTR) NS(MqS), NULL, FactoryDelete, NULL, NULL);
  if (MqFactoryErrorCheckI(ret)) {
    err = (MQ_STR) MqFactoryErrorMsg(ret); goto end;
  }
end:
  if (err) {
    RETURN_ERROR(err)
  } else {
    return;
  }
}



