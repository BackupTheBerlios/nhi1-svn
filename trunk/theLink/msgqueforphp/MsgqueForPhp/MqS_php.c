/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/MqS_php.c
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
zend_class_entry *NS(iBgError);
zend_class_entry *NS(iEvent);

int le_MqS;

static
PHP_METHOD(MsgqueForPhp_MqS, Exit)
{
  SETUP_mqctx;
  MqExit(mqctx);
  RETURN_NULL();
}

static
PHP_METHOD(MsgqueForPhp_MqS, Delete)
{
  SETUP_mqctx;
  MqContextFree(mqctx);
  RETURN_NULL();
}

static
PHP_METHOD(MsgqueForPhp_MqS, LogC)
{
  SETUP_mqctx;
  char* prefix; int prefixlen;
  char* msg; int msglen;
  long level;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sls", 
	&prefix, &prefixlen, &level, &msg, &msglen) == FAILURE) {
    RETURN_ERROR("usage: LogC(string: prefix, integer: level, string: message)");
  }
  MqLogC(mqctx, prefix, level, msg);
}

#ifdef _DEBUG
static
PHP_METHOD(MsgqueForPhp_MqS, Info)
{
  SETUP_mqctx;
  ARG2CST(Info,prefix);
  MqLogData(mqctx, prefix);
  MqLogV(mqctx,prefix,0,"refCount = %d\n", Z_REFCOUNT_P(mqctx->self));
}
#endif

static
PHP_METHOD(MsgqueForPhp_MqS, Init)
{
  NS(Argument2MqBufferLS)(MqInitArg0(), ZEND_NUM_ARGS() TSRMLS_CC) ;
}


#define CB(name) \
  if (instanceof_function(Z_OBJCE_P(getThis()), NS(i ## name) TSRMLS_CC)) { \
    zval *ctor; \
    void *data; \
    MqTokenF tokenF; \
    MqDataFreeF tokenDataFreeF; \
    MqDataCopyF tokenDataCopyF; \
    MAKE_STD_ZVAL(ctor); \
    array_init(ctor); \
    INCR_REG(mqctx->self); \
    add_next_index_zval(ctor, mqctx->self); \
    add_next_index_string(ctor, #name, 1); \
    ErrorMqToPhpWithCheck ( \
      NS(ProcInit) (mqctx, ctor, &data, &tokenF, &tokenDataFreeF, &tokenDataCopyF TSRMLS_CC) \
    ); \
    MqConfigSet ## name(mqctx, tokenF, data, tokenDataFreeF, tokenDataCopyF); \
  }


static
PHP_METHOD(MsgqueForPhp_MqS, __construct)
{
  zval *objvar = getThis();
  struct MqS *mqctx;
  struct MqS *tmpl;
  zval *ztmpl = NULL;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &ztmpl) == FAILURE) {
    RETURN_ERROR("usage: MqS(?tmpl?)");
  }
  if (ztmpl == NULL || Z_TYPE_P(ztmpl) == IS_NULL) {
    tmpl = NULL;
  } else if (instanceof_function(Z_OBJCE_P(ztmpl), NS(MqS) TSRMLS_CC)) {
    VAL2MqS(tmpl, ztmpl);
  } else {
    RETURN_ERROR("usage: MqS(NULL|MqS tmpl)");
  }

  // create context
  mqctx = (struct MqS *) MqContextCreate(sizeof (*mqctx), tmpl);

  // set configuration data
  mqctx->setup.Child.fCreate   = MqLinkDefault;
  mqctx->setup.Parent.fCreate  = MqLinkDefault;
  MqConfigSetIgnoreThread(mqctx, MQ_YES);
  TSRMLS_SET_CTX(mqctx->threadData) ;

  // register resources
  add_property_resource_ex(objvar, ID2(__ctx), (long) mqctx TSRMLS_CC);

  // create link between 'php' and 'MqS'
  mqctx->self = (MQ_PTR) objvar;

  //refcount++
  INCR_REG(mqctx->self);

  // add callback's
  CB(ServerSetup)
  CB(ServerCleanup)
  CB(BgError)
  CB(Event)
}

static
PHP_METHOD(MsgqueForPhp_MqS, __destruct)
{
  SETUP_mqctx;
  if (!mqctx) return;
  if (mqctx->self) {
    zend_update_property_long(NS(MqDumpS), mqctx->self, ID(__ctx), 0L TSRMLS_CC);
    DECR_REG(mqctx->self);
    mqctx->self = NULL;
  }
  mqctx->setup.factory = NULL;
  mqctx->setup.Event.fCall = NULL;
  MqContextDelete(&mqctx);
}

PHP_METHOD(MsgqueForPhp_MqS, ReadY);
PHP_METHOD(MsgqueForPhp_MqS, ReadO);
PHP_METHOD(MsgqueForPhp_MqS, ReadS);
PHP_METHOD(MsgqueForPhp_MqS, ReadI);
PHP_METHOD(MsgqueForPhp_MqS, ReadF);
PHP_METHOD(MsgqueForPhp_MqS, ReadW);
PHP_METHOD(MsgqueForPhp_MqS, ReadD);
PHP_METHOD(MsgqueForPhp_MqS, ReadC);
PHP_METHOD(MsgqueForPhp_MqS, ReadB);
PHP_METHOD(MsgqueForPhp_MqS, ReadN);
PHP_METHOD(MsgqueForPhp_MqS, ReadDUMP);
PHP_METHOD(MsgqueForPhp_MqS, ReadLOAD);
PHP_METHOD(MsgqueForPhp_MqS, ReadForward);
PHP_METHOD(MsgqueForPhp_MqS, ReadU);
PHP_METHOD(MsgqueForPhp_MqS, ReadL_START);
PHP_METHOD(MsgqueForPhp_MqS, ReadL_END);
PHP_METHOD(MsgqueForPhp_MqS, ReadT_START);
PHP_METHOD(MsgqueForPhp_MqS, ReadT_END);
PHP_METHOD(MsgqueForPhp_MqS, ReadGetNumItems);
PHP_METHOD(MsgqueForPhp_MqS, ReadItemExists);
PHP_METHOD(MsgqueForPhp_MqS, ReadUndo);
PHP_METHOD(MsgqueForPhp_MqS, ReadALL);
PHP_METHOD(MsgqueForPhp_MqS, ReadProxy);

PHP_METHOD(MsgqueForPhp_MqS, SendSTART);
PHP_METHOD(MsgqueForPhp_MqS, SendEND);
PHP_METHOD(MsgqueForPhp_MqS, SendEND_AND_WAIT);
PHP_METHOD(MsgqueForPhp_MqS, SendEND_AND_CALLBACK);
PHP_METHOD(MsgqueForPhp_MqS, SendRETURN);
PHP_METHOD(MsgqueForPhp_MqS, SendERROR);
PHP_METHOD(MsgqueForPhp_MqS, SendY);
PHP_METHOD(MsgqueForPhp_MqS, SendO);
PHP_METHOD(MsgqueForPhp_MqS, SendS);
PHP_METHOD(MsgqueForPhp_MqS, SendI);
PHP_METHOD(MsgqueForPhp_MqS, SendF);
PHP_METHOD(MsgqueForPhp_MqS, SendW);
PHP_METHOD(MsgqueForPhp_MqS, SendD);
PHP_METHOD(MsgqueForPhp_MqS, SendC);
PHP_METHOD(MsgqueForPhp_MqS, SendB);
PHP_METHOD(MsgqueForPhp_MqS, SendN);
PHP_METHOD(MsgqueForPhp_MqS, SendU);
PHP_METHOD(MsgqueForPhp_MqS, SendL_START);
PHP_METHOD(MsgqueForPhp_MqS, SendL_END);
PHP_METHOD(MsgqueForPhp_MqS, SendT_START);
PHP_METHOD(MsgqueForPhp_MqS, SendT_END);
PHP_METHOD(MsgqueForPhp_MqS, SendAll);

PHP_METHOD(MsgqueForPhp_MqS, ConfigReset);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetBuffersize);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetDebug);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetTimeout);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetName);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetSrvName);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetStorage);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetIsSilent);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetIsServer);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetIsString);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetIgnoreExit);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetEvent);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetServerSetup);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetServerCleanup);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetBgError);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetFactory);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetIoUdsFile);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetIoTcp);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetIoPipeSocket);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetStartAs);
PHP_METHOD(MsgqueForPhp_MqS, ConfigSetDaemon);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIsString);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIsSilent);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIsServer);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetDebug);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetBuffersize);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetTimeout);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetName);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetSrvName);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetStorage);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIoUdsFile);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIoTcpHost);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIoTcpPort);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIoTcpMyHost);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIoTcpMyPort);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetIoPipeSocket);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetStartAs);
PHP_METHOD(MsgqueForPhp_MqS, ConfigGetStatusIs);

PHP_METHOD(MsgqueForPhp_MqS, LinkIsParent);
PHP_METHOD(MsgqueForPhp_MqS, LinkIsConnected);
PHP_METHOD(MsgqueForPhp_MqS, LinkGetCtxId);
PHP_METHOD(MsgqueForPhp_MqS, LinkGetParent);
PHP_METHOD(MsgqueForPhp_MqS, LinkGetTargetIdent);
PHP_METHOD(MsgqueForPhp_MqS, LinkCreate);
PHP_METHOD(MsgqueForPhp_MqS, LinkCreateChild);
PHP_METHOD(MsgqueForPhp_MqS, LinkDelete);
PHP_METHOD(MsgqueForPhp_MqS, LinkConnect);

PHP_METHOD(MsgqueForPhp_MqS, ServiceGetToken);
PHP_METHOD(MsgqueForPhp_MqS, ServiceIsTransaction);
PHP_METHOD(MsgqueForPhp_MqS, ServiceGetFilter);
PHP_METHOD(MsgqueForPhp_MqS, ServiceProxy);
PHP_METHOD(MsgqueForPhp_MqS, ServiceStorage);
PHP_METHOD(MsgqueForPhp_MqS, ServiceCreate);
PHP_METHOD(MsgqueForPhp_MqS, ServiceDelete);
PHP_METHOD(MsgqueForPhp_MqS, ProcessEvent);

PHP_METHOD(MsgqueForPhp_MqS, ErrorC);
PHP_METHOD(MsgqueForPhp_MqS, ErrorSet);
PHP_METHOD(MsgqueForPhp_MqS, ErrorSetCONTINUE);
PHP_METHOD(MsgqueForPhp_MqS, ErrorSetEXIT);
PHP_METHOD(MsgqueForPhp_MqS, ErrorIsEXIT);
PHP_METHOD(MsgqueForPhp_MqS, ErrorGetText);
PHP_METHOD(MsgqueForPhp_MqS, ErrorGetNum);
PHP_METHOD(MsgqueForPhp_MqS, ErrorGetCode);
PHP_METHOD(MsgqueForPhp_MqS, ErrorReset);
PHP_METHOD(MsgqueForPhp_MqS, ErrorPrint);

PHP_METHOD(MsgqueForPhp_MqS, SlaveWorker);
PHP_METHOD(MsgqueForPhp_MqS, SlaveCreate);
PHP_METHOD(MsgqueForPhp_MqS, SlaveDelete);
PHP_METHOD(MsgqueForPhp_MqS, SlaveGet);
PHP_METHOD(MsgqueForPhp_MqS, SlaveGetMaster);
PHP_METHOD(MsgqueForPhp_MqS, SlaveIs);

PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxGet);
PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxSet);
PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxIdentGet);
PHP_METHOD(MsgqueForPhp_MqS, FactoryCtxIdentSet);

PHP_METHOD(MsgqueForPhp_MqS, StorageOpen);
PHP_METHOD(MsgqueForPhp_MqS, StorageClose);
PHP_METHOD(MsgqueForPhp_MqS, StorageInsert);
PHP_METHOD(MsgqueForPhp_MqS, StorageSelect);
PHP_METHOD(MsgqueForPhp_MqS, StorageDelete);
PHP_METHOD(MsgqueForPhp_MqS, StorageCount);

/* {{{ MsgqueForPhp_MqS_functions[]
 *
 * Every class method must have an entry in MsgqueForPhp_MqS_functions[].
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

ZEND_BEGIN_ARG_INFO_EX(MqDumpS_arg, 0, 0, 1)
  ZEND_ARG_OBJ_INFO(0, dump, MqDumpS, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(SlaveCreate_arg, 0, 0, 2)
  ZEND_ARG_INFO(0, "id")
  ZEND_ARG_OBJ_INFO(0, slave, MqS, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ServiceCreate_arg, 0, 0, 2)
  ZEND_ARG_INFO(0, "token")
  ZEND_ARG_INFO(0, "callback")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(id_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "id")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(LogC_arg, 0, 0, 3)
  ZEND_ARG_INFO(0, "prefix")
  ZEND_ARG_INFO(0, "level")
  ZEND_ARG_INFO(0, "message")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(token_arg, 0, 0, 1)
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

ZEND_BEGIN_ARG_INFO_EX(MqFactoryS_arg, 0, 0, 1)
  ZEND_ARG_OBJ_INFO(0, factory, MqFactoryS, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(ident_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "ident")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(storageFile_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "storageFile")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(transLId_arg, 0, 0, 1)
  ZEND_ARG_INFO(0, "transLId")
ZEND_END_ARG_INFO()

static const zend_function_entry NS(MqS_functions)[] = {
  PHP_ME(MsgqueForPhp_MqS, __construct,		    NULL,		  ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(MsgqueForPhp_MqS, __destruct,		    NULL,		  ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
  PHP_ME(MsgqueForPhp_MqS, Exit,		    no_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, Delete,		    no_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LogC,		    LogC_arg,		  ZEND_ACC_PUBLIC)
#ifdef _DEBUG
  PHP_ME(MsgqueForPhp_MqS, Info,		    val_arg,		  ZEND_ACC_PUBLIC)
#endif
  PHP_ME(MsgqueForPhp_MqS, Init,		    NULL,		  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)

  PHP_ME(MsgqueForPhp_MqS, FactoryCtxGet,	    no_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, FactoryCtxSet,	    MqFactoryS_arg,	  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, FactoryCtxIdentGet,	    no_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, FactoryCtxIdentSet,	    ident_arg,		  ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqS, ReadY,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadO,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadS,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadI,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadF,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadW,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadD,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadC,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadB,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadN,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadDUMP,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadLOAD,		    MqDumpS_arg,          ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadForward,		    NULL,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadU,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadL_START,		    NULL,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadL_END,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadT_START,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadT_END,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadGetNumItems,	    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadItemExists,	    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ReadUndo,		    no_arg,               ZEND_ACC_PUBLIC)
/*
  PHP_ME(MsgqueForPhp_MqS, ReadALL,		    NULL,                 ZEND_ACC_PUBLIC)
*/
  PHP_ME(MsgqueForPhp_MqS, ReadProxy,		    MqS_arg,              ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqS, SendSTART,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendEND,		    token_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendEND_AND_WAIT,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendEND_AND_CALLBACK,    SendEND_A_CB_arg,     ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendRETURN,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendERROR,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendY,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendO,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendS,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendI,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendF,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendW,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendD,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendC,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendB,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendN,		    value_arg,            ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendU,		    MqBufferS_arg,        ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendL_START,		    no_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendL_END,		    no_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendT_START,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SendT_END,		    value_arg,            ZEND_ACC_PUBLIC)
//  PHP_ME(MsgqueForPhp_MqS, SendAll,		    NULL,                 ZEND_ACC_PUBLIC)

//  PHP_ME(MsgqueForPhp_MqS, ConfigReset,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetBuffersize,	    val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetDebug,	    val_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetTimeout,	    val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetName,	    val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetSrvName,	    val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetStorage,	    storageFile_arg,      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetIsSilent,	    val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetIsServer,	    val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetIsString,	    val_arg,              ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetIgnoreExit,	    val_arg,              ZEND_ACC_PUBLIC)
/*
  PHP_ME(MsgqueForPhp_MqS, ConfigSetEvent,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetServerSetup,    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetServerCleanup,  NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetBgError,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetFactory,	    NULL,                 ZEND_ACC_PUBLIC)
*/
  PHP_ME(MsgqueForPhp_MqS, ConfigSetIoUdsFile,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetIoTcp,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetIoPipeSocket,   NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigSetStartAs,	    NULL,                 ZEND_ACC_PUBLIC)
//  PHP_ME(MsgqueForPhp_MqS, ConfigSetDaemon,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIsString,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIsSilent,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIsServer,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetDebug,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetBuffersize,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetTimeout,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetName,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetSrvName,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetStorage,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIoUdsFile,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIoTcpHost,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIoTcpPort,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIoTcpMyHost,    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIoTcpMyPort,    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetIoPipeSocket,   NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetStartAs,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ConfigGetStatusIs,	    NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqS, LinkIsParent,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LinkIsConnected,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LinkGetCtxId,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LinkGetParent,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LinkGetTargetIdent,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LinkCreate,		    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LinkCreateChild,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LinkDelete,		    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, LinkConnect,		    NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqS, ServiceGetToken,	    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ServiceIsTransaction,    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ServiceGetFilter,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ServiceProxy,	    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ServiceStorage,	    token_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ServiceCreate,	    ServiceCreate_arg,    ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ServiceDelete,	    token_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ProcessEvent,	    NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqS, ErrorC,		    ErrorC_arg,           ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ErrorSet,		    Exception_arg,        ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ErrorSetCONTINUE,	    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ErrorSetEXIT,	    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ErrorIsEXIT,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ErrorGetText,	    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ErrorGetNum,		    no_arg,               ZEND_ACC_PUBLIC)
/*
  PHP_ME(MsgqueForPhp_MqS, ErrorGetCode,	    no_arg,               ZEND_ACC_PUBLIC)
*/
  PHP_ME(MsgqueForPhp_MqS, ErrorReset,		    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, ErrorPrint,		    no_arg,               ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqS, SlaveWorker,		    NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SlaveCreate,		    SlaveCreate_arg,      ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SlaveDelete,		    id_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SlaveGet,		    id_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SlaveGetMaster,	    no_arg,               ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, SlaveIs,		    no_arg,               ZEND_ACC_PUBLIC)

  PHP_ME(MsgqueForPhp_MqS, StorageOpen,		    storageFile_arg,	  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, StorageClose,	    no_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, StorageInsert,	    no_arg,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, StorageSelect,	    NULL,		  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, StorageDelete,	    transLId_arg,	  ZEND_ACC_PUBLIC)
  PHP_ME(MsgqueForPhp_MqS, StorageCount,	    no_arg,		  ZEND_ACC_PUBLIC)

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
  zend_class_entry iBgError;
  zend_class_entry iEvent;

  // initialize the other files
  NS(MqS_Config_Init)(TSRMLS_C);
  NS(MqS_Link_Init)(TSRMLS_C);

  // create class "MqS"
  INIT_CLASS_ENTRY(cMqS,"MqS", NS(MqS_functions));
  NS(MqS) = zend_register_internal_class(&cMqS TSRMLS_CC);

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
  zend_declare_class_constant_long(NS(MqS), ID(START_SPAWN),	  3 TSRMLS_CC);

  // create interface "iServerSetup"
  INIT_CLASS_ENTRY(iServerSetup,"iServerSetup", NS(iServerSetup_functions));
  NS(iServerSetup) = zend_register_internal_interface(&iServerSetup TSRMLS_CC);

  // create interface "iServerCleanup"
  INIT_CLASS_ENTRY(iServerCleanup,"iServerCleanup", NS(iServerCleanup_functions));
  NS(iServerCleanup) = zend_register_internal_interface(&iServerCleanup TSRMLS_CC);

  // create interface "iBgError"
  INIT_CLASS_ENTRY(iBgError,"iBgError", NS(iBgError_functions));
  NS(iBgError) = zend_register_internal_interface(&iBgError TSRMLS_CC);

  // create interface "iEvent"
  INIT_CLASS_ENTRY(iEvent,"iEvent", NS(iEvent_functions));
  NS(iEvent) = zend_register_internal_interface(&iEvent TSRMLS_CC);
}

