/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/MqFactoryS_php.c
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

zend_class_entry *NS(MqFactoryS);

#define SETUP_factory struct MqFactoryS *factory; VAL2MqFactoryS(factory, getThis());
#define ErrorFactoryToPhpWithCheck(PROC) if (unlikely((PROC) == NULL)) { RETURN_ERROR("MqFactoryS exception"); }

//  if (unlikely((PROC) == NULL)) { zend_throw_exception(NULL,"MqFactoryS exception" TSRMLS_CC); }

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

PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxGet)
{
  SETUP_mqctx;
  MqFactoryS2VAL(return_value, MqFactoryCtxGet (mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxSet)
{
  SETUP_mqctx;
  ARG2MqFactoryS(FactoryCtxSet,val);
  ErrorMqToPhpWithCheck(MqFactoryCtxSet(mqctx, val));
  RETURN_NULL();
}

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

/*****************************************************************************/
/*                                                                           */
/*                                static                                     */
/*                                                                           */
/*****************************************************************************/

#define FactorySetup(N) \
  zend_class_entry *ce; \
  zend_class_entry **cep; \
  char *class=NULL, *ident=NULL; \
  int classlen, identlen; \
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &ident, &identlen, &class, &classlen) == FAILURE) { \
    RETURN_ERROR("usage: " #N "(?ident?, class)"); \
  } \
  /* check/setup 'class' */ \
  if (class == NULL) { \
    class = ident; \
    classlen = identlen; \
  } \
  /* class names are stored in lowercase */ \
  class = MqSysStrNDup(MQ_ERROR_PANIC, class, classlen); \
  php_strtolower(class, classlen); \
  /* does the class names exists ? */ \
  if (zend_lookup_class(class, classlen, &cep TSRMLS_CC) == FAILURE) { \
    if (class) MqSysFree(class); \
    RETURN_ERROR("Class does not exist."); \
  } \
  ce = *cep; \
  /* check if the constructor is available */ \
  if (ce->constructor == NULL) { \
    if (class) MqSysFree(class); \
    RETURN_ERROR("Class constructor does not exist."); \
  }

PHP_FUNCTION(FactoryAdd)
{
  FactorySetup(FactoryAdd);
  MqFactoryS2VAL(return_value,
    MqFactoryAdd(MQ_ERROR_PRINT, ident, 
      FactoryCreate, (MQ_PTR) ce, NULL, NULL, FactoryDelete, NULL, NULL, NULL)
  );
}

PHP_FUNCTION(FactoryDefault)
{
  FactorySetup(FactoryDefault);
  MqFactoryS2VAL(return_value,
    MqFactoryDefault(MQ_ERROR_PRINT, ident, 
      FactoryCreate, (MQ_PTR) ce, NULL, NULL, FactoryDelete, NULL, NULL, NULL)
  );
}

PHP_FUNCTION(FactoryDefaultIdent)
{
  CST2VAL(return_value, MqFactoryDefaultIdent());
}

PHP_FUNCTION(FactoryGet)
{
  ARG2CST_OPT(ident, NULL);
  MqFactoryS2VAL(return_value, MqFactoryGet(ident));
}

PHP_FUNCTION(FactoryGetCalled)
{
  ARG2CST_OPT(ident, NULL);
  MqFactoryS2VAL(return_value, MqFactoryGetCalled(ident));
}

/*****************************************************************************/
/*                                                                           */
/*                                instance                                   */
/*                                                                           */
/*****************************************************************************/

PHP_METHOD(MsgqueForPhp_MqFactoryS, New)
{
#ifdef ZTS
  MQ_PTR data = (MQ_PTR)tsrm_ls;
#else
  MQ_PTR data = NULL;
#endif
  SETUP_factory;
  MqS2VAL(return_value, MqFactoryNew(MQ_ERROR_PRINT, data, factory));
}

PHP_METHOD(MsgqueForPhp_MqFactoryS, Copy)
{
  SETUP_factory;
  ARG2CST(Copy,ident);
  MqFactoryS2VAL(return_value, MqFactoryCopy(factory, ident));
}

/*****************************************************************************/
/*                                                                           */
/*                                  class                                    */
/*                                                                           */
/*****************************************************************************/

static
PHP_METHOD(MsgqueForPhp_MqFactoryS, __construct)
{
  RETURN_ERROR("it is not allowed to create an instance og 'MqFactoryS'.");
}

ZEND_BEGIN_ARG_INFO_EX(no_arg, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ident_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "ident")
ZEND_END_ARG_INFO()

static const zend_function_entry NS(MqFactoryS_functions)[] = {
  PHP_ME(MsgqueForPhp_MqFactoryS, __construct,	NULL,	      ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
  PHP_ME(MsgqueForPhp_MqFactoryS, New,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqFactoryS, Copy,		ident_arg,    ZEND_ACC_PUBLIC)

  {NULL, NULL, NULL}
};

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqFactoryS_New) (zval *return_value, struct MqFactoryS *factory TSRMLS_DC)
{
  if (factory == NULL) {
    RETURN_ERROR("unable to create a empty 'MqFactoryS' instance.");
  } else {
    // convert to an object instance
    if (object_init_ex(return_value, NS(MqFactoryS)) == FAILURE) {
      RETURN_ERROR("unable to create an 'MqFactoryS' instance.");
    }
    // link 'buf' with the object instance
    add_property_resource_ex(return_value, ID2(__factory), (long) factory TSRMLS_CC);
  }
}

void NS(MqFactoryS_Init) (TSRMLS_D) {
  struct MqFactoryS *item;
  zend_class_entry me_ce;

  // create class and make depend on "Exception"
  INIT_CLASS_ENTRY(me_ce,"MqFactoryS", NS(MqFactoryS_functions));
  NS(MqFactoryS) = zend_register_internal_class(&me_ce TSRMLS_CC);

  // add default factory
  if (!strcmp(MqFactoryDefaultIdent(),"libmsgque"))
    if (MqFactoryDefault(MQ_ERROR_PRINT, "phpmsgque", 
	  FactoryCreate, (MQ_PTR) NS(MqS), NULL, NULL, FactoryDelete, NULL, NULL, NULL) == NULL) {
      RETURN_ERROR("MqFactoryS exception");
    }
}

