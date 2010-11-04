/**
 *  \file       theLink/phpmsgque/PhpMsgque/MqS_php.c
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

#define MQ_CONTEXT_S mqctx

zend_class_entry *NS(MqS);
zend_class_entry *NS(iServerSetup);
zend_class_entry *NS(iServerCleanup);
zend_class_entry *NS(iFactory);
zend_class_entry *NS(iBgError);
zend_class_entry *NS(iEvent);

int le_MqS;

static zend_object_value NS(MqS_new)(zend_class_entry *ce TSRMLS_DC)
{
  struct MqS * mqctx = (struct MqS *) MqContextCreate(sizeof (*mqctx), NULL);
  zend_object *object;
  zval retval;

  /* Reuse Zend's generic object creator */
  Z_OBJVAL(retval) = zend_objects_new(&object, ce TSRMLS_CC);
  /* When overriding create_object,
   * properties must be manually initialized */
  ALLOC_HASHTABLE(object->properties);
  zend_hash_init(object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
  /* Other object initialization may occur here */

  // set configuration data
  mqctx->setup.Child.fCreate   = MqLinkDefault;
  mqctx->setup.Parent.fCreate  = MqLinkDefault;
  TSRMLS_SET_CTX(mqctx->threadData) ;

  // register resources
  zend_update_property_long(NS(MqS), &retval, ID(__ctx), (long) mqctx TSRMLS_CC);
  return Z_OBJVAL(retval);
}

static
PHP_METHOD(PhpMsgque_MqS, Exit)
{
  MqExit(MQCTX);
  RETURN_NULL();
}

static
PHP_METHOD(PhpMsgque_MqS, Delete)
{
  MqContextFree(MQCTX);
  RETURN_NULL();
}

static
PHP_METHOD(PhpMsgque_MqS, LogC)
{
  char* prefix; int prefixlen;
  char* msg; int msglen;
  long level;
  if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sls", 
	&prefix, &prefixlen, &level, &msg, &msglen) == FAILURE) {
    RaiseError("usage: LogC(string: prefix, long: level, string: message)");
    return;
  }
  MqLogC(MQCTX, prefix, level, msg);
}


#define CB(name) \
  if (instanceof_function(Z_OBJCE_P(getThis()), NS(i ## name) TSRMLS_CC)) { \
    zval *ctor; \
    void *data; \
    MqTokenF tokenF; \
    MqTokenDataFreeF tokenDataFreeF; \
    MqTokenDataCopyF tokenDataCopyF; \
    MAKE_STD_ZVAL(ctor); \
    array_init(ctor); \
    zval_addref_p(mqctx->self); \
    add_next_index_zval(ctor, mqctx->self); \
    add_next_index_string(ctor, #name, 1); \
    ErrorMqToPhpWithCheck ( \
      NS(ProcInit) (mqctx, ctor, &data, &tokenF, &tokenDataFreeF, &tokenDataCopyF TSRMLS_CC) \
    ); \
    MqConfigSet ## name(mqctx, tokenF, data, tokenDataFreeF, tokenDataCopyF); \
  }


static enum MqErrorE
FactoryCreate(
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR data,
  struct MqS  ** mqctxP
)
{
/*
  if (create == MQ_FACTORY_NEW_FORK) {
    //rb_thread_atfork();
  }
  if (create == MQ_FACTORY_NEW_THREAD) {
    //RUBY_INIT_STACK;
  }
*/

  TSRMLS_FETCH_FROM_CTX(tmpl->threadData);
  //TSRMLS_FETCH();
  zval *self;
  enum MqErrorE ret = NS(ProcCall) (tmpl, (struct NS(ProcDataS) * const) data, 0, NULL, &self);

  if (MqErrorCheckI(ret) || self == NULL || Z_TYPE_P(self) == IS_NULL) {
    *mqctxP = NULL;
    return MqErrorGetCode(tmpl);
  } else {
    struct MqS * const mqctx = *mqctxP = VAL2MqS(self);
    zval_addref_p(self);
    MqConfigDup(mqctx, tmpl);
    MqErrorCheck(MqSetupDup(mqctx, tmpl));
    return MQ_OK;
error:
    *mqctxP = NULL;
    MqErrorCopy (tmpl, mqctx);
    return MqErrorStack(tmpl);
  }
  return MQ_OK;
}

static void
FactoryDelete(
  struct MqS * mqctx,
  MQ_BOL doFactoryDelete,
  MQ_PTR data
)
{ 
  MqContextFree (mqctx);
  if (doFactoryDelete && mqctx->self != NULL) {
    zval_delref_p(mqctx->self);
    mqctx->self = NULL;
  }
}

static
PHP_METHOD(PhpMsgque_MqS, __construct)
{
  SETUP_mqctx;

  // create link between 'php' and 'MqS'
  mqctx->self = (void*) getThis();

  //refcount++
  zval_addref_p(getThis());
  
  // add callback's
  CB(ServerSetup)
  CB(ServerCleanup)
  CB(BgError)
  CB(Event)
  if (instanceof_function(Z_OBJCE_P(getThis()), NS(iFactory) TSRMLS_CC)) {
    zval *ctor;
    void *data;
    MqTokenF tokenF;
    MqTokenDataFreeF tokenDataFreeF;
    MqTokenDataCopyF tokenDataCopyF;
    MAKE_STD_ZVAL(ctor);
    array_init(ctor);
    zval_addref_p(mqctx->self);
    add_next_index_zval(ctor, mqctx->self);
    add_next_index_string(ctor, "Factory", 1);
    ErrorMqToPhpWithCheck (
      NS(ProcInit) (mqctx, ctor, &data, &tokenF, &tokenDataFreeF, &tokenDataCopyF TSRMLS_CC)
    );
    MqConfigSetFactory(mqctx, 
      FactoryCreate, data, tokenDataFreeF, tokenDataCopyF,
      FactoryDelete, NULL,           NULL,           NULL
    );
  }
}

static
PHP_METHOD(PhpMsgque_MqS, __destruct)
{
  SETUP_mqctx;
  mqctx->setup.Factory.Delete.fCall = NULL;
  mqctx->setup.Event.fCall = NULL;
  mqctx->self = NULL;
  MqContextDelete(&mqctx);
}

PHP_METHOD(PhpMsgque_MqS, ReadY);
PHP_METHOD(PhpMsgque_MqS, ReadO);
PHP_METHOD(PhpMsgque_MqS, ReadS);
PHP_METHOD(PhpMsgque_MqS, ReadI);
PHP_METHOD(PhpMsgque_MqS, ReadF);
PHP_METHOD(PhpMsgque_MqS, ReadW);
PHP_METHOD(PhpMsgque_MqS, ReadD);
PHP_METHOD(PhpMsgque_MqS, ReadC);
PHP_METHOD(PhpMsgque_MqS, ReadB);
PHP_METHOD(PhpMsgque_MqS, ReadN);
PHP_METHOD(PhpMsgque_MqS, ReadBDY);
PHP_METHOD(PhpMsgque_MqS, ReadU);
PHP_METHOD(PhpMsgque_MqS, ReadL_START);
PHP_METHOD(PhpMsgque_MqS, ReadL_END);
PHP_METHOD(PhpMsgque_MqS, ReadT_START);
PHP_METHOD(PhpMsgque_MqS, ReadT_END);
PHP_METHOD(PhpMsgque_MqS, ReadGetNumItems);
PHP_METHOD(PhpMsgque_MqS, ReadItemExists);
PHP_METHOD(PhpMsgque_MqS, ReadUndo);
PHP_METHOD(PhpMsgque_MqS, ReadALL);
PHP_METHOD(PhpMsgque_MqS, ReadProxy);

PHP_METHOD(PhpMsgque_MqS, SendSTART);
PHP_METHOD(PhpMsgque_MqS, SendEND);
PHP_METHOD(PhpMsgque_MqS, SendEND_AND_WAIT);
PHP_METHOD(PhpMsgque_MqS, SendEND_AND_CALLBACK);
PHP_METHOD(PhpMsgque_MqS, SendRETURN);
PHP_METHOD(PhpMsgque_MqS, SendERROR);
PHP_METHOD(PhpMsgque_MqS, SendY);
PHP_METHOD(PhpMsgque_MqS, SendO);
PHP_METHOD(PhpMsgque_MqS, SendS);
PHP_METHOD(PhpMsgque_MqS, SendI);
PHP_METHOD(PhpMsgque_MqS, SendF);
PHP_METHOD(PhpMsgque_MqS, SendW);
PHP_METHOD(PhpMsgque_MqS, SendD);
PHP_METHOD(PhpMsgque_MqS, SendC);
PHP_METHOD(PhpMsgque_MqS, SendB);
PHP_METHOD(PhpMsgque_MqS, SendN);
PHP_METHOD(PhpMsgque_MqS, SendBDY);
PHP_METHOD(PhpMsgque_MqS, SendU);
PHP_METHOD(PhpMsgque_MqS, SendL_START);
PHP_METHOD(PhpMsgque_MqS, SendL_END);
PHP_METHOD(PhpMsgque_MqS, SendT_START);
PHP_METHOD(PhpMsgque_MqS, SendT_END);
PHP_METHOD(PhpMsgque_MqS, SendAll);

PHP_METHOD(PhpMsgque_MqS, ConfigReset);
PHP_METHOD(PhpMsgque_MqS, ConfigSetBuffersize);
PHP_METHOD(PhpMsgque_MqS, ConfigSetDebug);
PHP_METHOD(PhpMsgque_MqS, ConfigSetTimeout);
PHP_METHOD(PhpMsgque_MqS, ConfigSetName);
PHP_METHOD(PhpMsgque_MqS, ConfigSetSrvName);
PHP_METHOD(PhpMsgque_MqS, ConfigSetIdent);
PHP_METHOD(PhpMsgque_MqS, ConfigSetIsSilent);
PHP_METHOD(PhpMsgque_MqS, ConfigSetIsServer);
PHP_METHOD(PhpMsgque_MqS, ConfigSetIsString);
PHP_METHOD(PhpMsgque_MqS, ConfigSetIgnoreExit);
PHP_METHOD(PhpMsgque_MqS, ConfigSetEvent);
PHP_METHOD(PhpMsgque_MqS, ConfigSetServerSetup);
PHP_METHOD(PhpMsgque_MqS, ConfigSetServerCleanup);
PHP_METHOD(PhpMsgque_MqS, ConfigSetBgError);
PHP_METHOD(PhpMsgque_MqS, ConfigSetFactory);
PHP_METHOD(PhpMsgque_MqS, ConfigSetIoUdsFile);
PHP_METHOD(PhpMsgque_MqS, ConfigSetIoTcp);
PHP_METHOD(PhpMsgque_MqS, ConfigSetIoPipeSocket);
PHP_METHOD(PhpMsgque_MqS, ConfigSetStartAs);
PHP_METHOD(PhpMsgque_MqS, ConfigSetDaemon);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIsString);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIsSilent);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIsServer);
PHP_METHOD(PhpMsgque_MqS, ConfigGetDebug);
PHP_METHOD(PhpMsgque_MqS, ConfigGetBuffersize);
PHP_METHOD(PhpMsgque_MqS, ConfigGetTimeout);
PHP_METHOD(PhpMsgque_MqS, ConfigGetName);
PHP_METHOD(PhpMsgque_MqS, ConfigGetSrvName);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIdent);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIoUdsFile);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIoTcpHost);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIoTcpPort);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIoTcpMyHost);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIoTcpMyPort);
PHP_METHOD(PhpMsgque_MqS, ConfigGetIoPipeSocket);
PHP_METHOD(PhpMsgque_MqS, ConfigGetStartAs);

PHP_METHOD(PhpMsgque_MqS, LinkIsParent);
PHP_METHOD(PhpMsgque_MqS, LinkIsConnected);
PHP_METHOD(PhpMsgque_MqS, LinkGetCtxId);
PHP_METHOD(PhpMsgque_MqS, LinkGetParent);
PHP_METHOD(PhpMsgque_MqS, LinkGetTargetIdent);
PHP_METHOD(PhpMsgque_MqS, LinkCreate);
PHP_METHOD(PhpMsgque_MqS, LinkCreateChild);
PHP_METHOD(PhpMsgque_MqS, LinkDelete);
PHP_METHOD(PhpMsgque_MqS, LinkConnect);

PHP_METHOD(PhpMsgque_MqS, ServiceGetToken);
PHP_METHOD(PhpMsgque_MqS, ServiceIsTransaction);
PHP_METHOD(PhpMsgque_MqS, ServiceGetFilter);
PHP_METHOD(PhpMsgque_MqS, ServiceProxy);
PHP_METHOD(PhpMsgque_MqS, ServiceCreate);
PHP_METHOD(PhpMsgque_MqS, ServiceDelete);
PHP_METHOD(PhpMsgque_MqS, ProcessEvent);

PHP_METHOD(PhpMsgque_MqS, ErrorC);
PHP_METHOD(PhpMsgque_MqS, ErrorSet);
PHP_METHOD(PhpMsgque_MqS, ErrorSetCONTINUE);
PHP_METHOD(PhpMsgque_MqS, ErrorIsEXIT);
PHP_METHOD(PhpMsgque_MqS, ErrorGetText);
PHP_METHOD(PhpMsgque_MqS, ErrorGetNum);
PHP_METHOD(PhpMsgque_MqS, ErrorGetCode);
PHP_METHOD(PhpMsgque_MqS, ErrorReset);
PHP_METHOD(PhpMsgque_MqS, ErrorPrint);

PHP_METHOD(PhpMsgque_MqS, SlaveWorker);
PHP_METHOD(PhpMsgque_MqS, SlaveCreate);
PHP_METHOD(PhpMsgque_MqS, SlaveDelete);
PHP_METHOD(PhpMsgque_MqS, SlaveGet);
PHP_METHOD(PhpMsgque_MqS, SlaveGetMaster);
PHP_METHOD(PhpMsgque_MqS, SlaveIs);


/* {{{ PhpMsgque_MqS_functions[]
 *
 * Every class method must have an entry in PhpMsgque_MqS_functions[].
 */

ZEND_BEGIN_ARG_INFO_EX(no_arg, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(val_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "val")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Exception_arg, 0, 0, 1)
  ZEND_ARG_OBJ_INFO(0, ex, Exception, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MqBufferS_arg, 0, 0, 1)
  ZEND_ARG_OBJ_INFO(0, buf, MqBufferS, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MqS_arg, 0, 0, 1)
  ZEND_ARG_OBJ_INFO(0, mqs, MqS, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(SlaveCreate_arg, 0, 0, 2)
  ZEND_ARG_INFO(0, "id")
  ZEND_ARG_OBJ_INFO(0, slave, NS(MqS), 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ServiceCreate_arg, 0, 0, 2)
  ZEND_ARG_INFO(0, "token")
  ZEND_ARG_INFO(0, "callback")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ServiceDelete_arg, 0, 0, 2)
  ZEND_ARG_INFO(0, "token")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(id_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "id")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(LogC_arg, 0, 0, 3)
  ZEND_ARG_INFO(0, "prefix")
  ZEND_ARG_INFO(0, "level")
  ZEND_ARG_INFO(0, "message")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(SendEND_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "token")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(SendEND_A_CB_arg, 0, 0, 2)
  ZEND_ARG_INFO(0, "token")
  ZEND_ARG_INFO(0, "callback")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(value_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "value")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ErrorC_arg, 0, 0, 3)
  ZEND_ARG_INFO(0, "prefix")
  ZEND_ARG_INFO(0, "errnum")
  ZEND_ARG_INFO(0, "message")
ZEND_END_ARG_INFO()

static const zend_function_entry NS(MqS_functions)[] = {
  PHP_ME(PhpMsgque_MqS, __construct,		NULL,		      ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(PhpMsgque_MqS, __destruct,		NULL,		      ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
  PHP_ME(PhpMsgque_MqS, Exit,			no_arg,		      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, Delete,			no_arg,		      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LogC,			LogC_arg,	      ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, ReadY,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadO,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadS,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadI,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadF,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadW,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadD,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadC,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadB,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadN,			no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadBDY,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadU,			no_arg,               ZEND_ACC_PUBLIC)
/*
  PHP_ME(PhpMsgque_MqS, ReadL_START,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadL_END,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadT_START,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadT_END,		NULL,                 ZEND_ACC_PUBLIC)
*/
  PHP_ME(PhpMsgque_MqS, ReadGetNumItems,	no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadItemExists,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadUndo,		no_arg,               ZEND_ACC_PUBLIC)
/*
  PHP_ME(PhpMsgque_MqS, ReadALL,		NULL,                 ZEND_ACC_PUBLIC)
*/
  PHP_ME(PhpMsgque_MqS, ReadProxy,		MqS_arg,              ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, SendSTART,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendEND,		SendEND_arg,          ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendEND_AND_WAIT,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendEND_AND_CALLBACK,	SendEND_A_CB_arg,     ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendRETURN,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendERROR,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendY,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendO,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendS,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendI,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendF,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendW,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendD,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendC,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendB,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendN,			value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendBDY,		value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendU,			MqBufferS_arg,        ZEND_ACC_PUBLIC)
/*
  PHP_ME(PhpMsgque_MqS, SendL_START,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendL_END,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendT_START,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendT_END,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendAll,		NULL,                 ZEND_ACC_PUBLIC)
*/

//  PHP_ME(PhpMsgque_MqS, ConfigReset,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetBuffersize,	val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetDebug,		val_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetTimeout,	val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetName,		val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetSrvName,	val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIdent,		val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIsSilent,	val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIsServer,	val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIsString,	val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIgnoreExit,	val_arg,              ZEND_ACC_PUBLIC)
/*
  PHP_ME(PhpMsgque_MqS, ConfigSetEvent,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetServerSetup,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetServerCleanup, NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetBgError,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetFactory,	NULL,                 ZEND_ACC_PUBLIC)
*/
  PHP_ME(PhpMsgque_MqS, ConfigSetIoUdsFile,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIoTcp,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIoPipeSocket,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetStartAs,	NULL,                 ZEND_ACC_PUBLIC)
//  PHP_ME(PhpMsgque_MqS, ConfigSetDaemon,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIsString,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIsSilent,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIsServer,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetDebug,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetBuffersize,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetTimeout,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetName,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetSrvName,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIdent,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIoUdsFile,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIoTcpHost,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIoTcpPort,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIoTcpMyHost,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIoTcpMyPort,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetIoPipeSocket,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigGetStartAs,	NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, LinkIsParent,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkIsConnected,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkGetCtxId,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkGetParent,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkGetTargetIdent,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkCreate,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkCreateChild,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkDelete,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkConnect,		NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, ServiceGetToken,	no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceIsTransaction,	no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceGetFilter,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceProxy,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceCreate,		ServiceCreate_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceDelete,		ServiceDelete_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ProcessEvent,		NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, ErrorC,			ErrorC_arg,           ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorSet,		Exception_arg,        ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorSetCONTINUE,	no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorIsEXIT,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorGetText,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorGetNum,		no_arg,               ZEND_ACC_PUBLIC)
/*
  PHP_ME(PhpMsgque_MqS, ErrorGetCode,		no_arg,               ZEND_ACC_PUBLIC)
*/
  PHP_ME(PhpMsgque_MqS, ErrorReset,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorPrint,		no_arg,               ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, SlaveWorker,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveCreate,		SlaveCreate_arg,      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveDelete,		id_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveGet,		id_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveGetMaster,		no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveIs,		no_arg,               ZEND_ACC_PUBLIC)

  {NULL, NULL, NULL}
};
/* }}} */

const zend_function_entry NS(iServerSetup_functions)[] = {
  ZEND_ABSTRACT_ME(NS(iServerSetup), ServerSetup, no_arg)
  {NULL, NULL, NULL}
};

const zend_function_entry NS(iServerCleanup_functions)[] = {
  ZEND_ABSTRACT_ME(NS(iServerCleanup), ServerCleanup, no_arg)
  {NULL, NULL, NULL}
};

const zend_function_entry NS(iFactory_functions)[] = {
  ZEND_ABSTRACT_ME(NS(iFactory), Factory, no_arg)
  {NULL, NULL, NULL}
};

const zend_function_entry NS(iBgError_functions)[] = {
  ZEND_ABSTRACT_ME(NS(iBgError), BgError, no_arg)
  {NULL, NULL, NULL}
};

const zend_function_entry NS(iEvent_functions)[] = {
  ZEND_ABSTRACT_ME(NS(iEvent), Event, no_arg)
  {NULL, NULL, NULL}
};


void NS(MqS_Error_Init)	    (TSRMLS_D);
void NS(MqS_Read_Init)	    (TSRMLS_D);
void NS(MqS_Send_Init)	    (TSRMLS_D);
void NS(MqS_Config_Init)    (TSRMLS_D);
void NS(MqS_Service_Init)   (TSRMLS_D);
void NS(MqS_Link_Init)	    (TSRMLS_D);
void NS(MqS_Slave_Init)	    (TSRMLS_D);
void NS(MqS_Sys_Init)	    (TSRMLS_D);

void NS(MqS_Init) (TSRMLS_D)
{
  zend_class_entry cMqS;
  zend_class_entry iServerSetup;
  zend_class_entry iServerCleanup;
  zend_class_entry iFactory;
  zend_class_entry iBgError;
  zend_class_entry iEvent;

  // initialize the other files
  NS(MqS_Config_Init)(TSRMLS_C);
  NS(MqS_Link_Init)(TSRMLS_C);

  // create class "MqS"
  INIT_CLASS_ENTRY(cMqS,"MqS", NS(MqS_functions));
  NS(MqS) = zend_register_internal_class(&cMqS TSRMLS_CC);
  NS(MqS)->create_object = NS(MqS_new);

  // define additional properties "mqctx" to save the "struct MqS *" pointer
  zend_declare_property_null(NS(MqS), ID(__ctx), ZEND_ACC_PRIVATE TSRMLS_CC);

  // create enum "MqS_WAIT"
  zend_declare_class_constant_long(NS(MqS), ID(WAIT_NO),	  0 TSRMLS_CC);
  zend_declare_class_constant_long(NS(MqS), ID(WAIT_ONCE),	  1 TSRMLS_CC);
  zend_declare_class_constant_long(NS(MqS), ID(WAIT_FOREVER),	  2 TSRMLS_CC);

  // create enum "MqS_TIMEOUT"
  zend_declare_class_constant_long(NS(MqS), ID(TIMEOUT_DEFAULT),  -1 TSRMLS_CC);
  zend_declare_class_constant_long(NS(MqS), ID(TIMEOUT_USER),	  -2 TSRMLS_CC);
  zend_declare_class_constant_long(NS(MqS), ID(TIMEOUT_MAX),	  -3 TSRMLS_CC);

  // create enum "MqS_START"
  zend_declare_class_constant_long(NS(MqS), ID(START_DEFAULT),	  0 TSRMLS_CC);
  zend_declare_class_constant_long(NS(MqS), ID(START_FORK),	  1 TSRMLS_CC);
  zend_declare_class_constant_long(NS(MqS), ID(START_THREAD),	  2 TSRMLS_CC);
  zend_declare_class_constant_long(NS(MqS), ID(START_THREAD),	  3 TSRMLS_CC);

  // create interface "iServerSetup"
  INIT_CLASS_ENTRY(iServerSetup,"iServerSetup", NS(iServerSetup_functions));
  NS(iServerSetup) = zend_register_internal_interface(&iServerSetup TSRMLS_CC);

  // create interface "iServerCleanup"
  INIT_CLASS_ENTRY(iServerCleanup,"iServerCleanup", NS(iServerCleanup_functions));
  NS(iServerCleanup) = zend_register_internal_interface(&iServerCleanup TSRMLS_CC);

  // create interface "iFactory"
  INIT_CLASS_ENTRY(iFactory,"iFactory", NS(iFactory_functions));
  NS(iFactory) = zend_register_internal_interface(&iFactory TSRMLS_CC);

  // create interface "iBgError"
  INIT_CLASS_ENTRY(iBgError,"iBgError", NS(iBgError_functions));
  NS(iBgError) = zend_register_internal_interface(&iBgError TSRMLS_CC);

  // create interface "iEvent"
  INIT_CLASS_ENTRY(iEvent,"iEvent", NS(iEvent_functions));
  NS(iEvent) = zend_register_internal_interface(&iEvent TSRMLS_CC);
}
