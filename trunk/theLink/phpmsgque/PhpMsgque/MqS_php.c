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

zend_class_entry *PhpMsgque_MqS;

int le_MqS;

static
PHP_METHOD(PhpMsgque_MqS, __construct)
{
  long MqS_id;
  struct MqS * mqctx = (struct MqS *) MqContextCreate(sizeof (*mqctx), NULL);

  // create ruby command
  mqctx->self = (void*) this_ptr;

  // set configuration data
  mqctx->setup.Child.fCreate   = MqLinkDefault;
  mqctx->setup.Parent.fCreate  = MqLinkDefault;

  // register resources
  zend_update_property_long(PhpMsgque_MqS, getThis(), ID(mqctx), (long) mqctx TSRMLS_CC);
}

static
PHP_METHOD(PhpMsgque_MqS, Exit)
{
  MqExit(MQCTX);
}

static
PHP_METHOD(PhpMsgque_MqS, ErrorSet)
{
  zval *ex;
  zend_get_parameters(0, 1, &ex);
  NS(MqSException_Set)(MQCTX, ex TSRMLS_CC);
  RETURN_NULL();
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

ZEND_BEGIN_ARG_INFO_EX(Exception_arg, 0, 0, 1)
  ZEND_ARG_OBJ_INFO(0, ex, Exception, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry PhpMsgque_MqS_functions[] = {
  PHP_ME(PhpMsgque_MqS, __construct,		NULL,		      ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(PhpMsgque_MqS, __destruct,		NULL,		      ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(PhpMsgque_MqS, Exit,			no_arg,		      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorSet,		Exception_arg,	      ZEND_ACC_PUBLIC)
/*
  PHP_ME(PhpMsgque_MqS, ReadY,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadO,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadS,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadI,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadF,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadW,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadD,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadC,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadB,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadN,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadBDY,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadU,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadL_START,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadL_END,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadT_START,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadT_END,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadGetNumItems,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadItemExists,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadUndo,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadALL,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ReadProxy,		NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, SendSTART,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendEND,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendEND_AND_WAIT,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendEND_AND_CALLBACK,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendRETURN,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendERROR,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendY,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendO,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendS,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendI,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendF,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendW,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendD,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendC,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendB,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendN,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendBDY,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendU,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendL_START,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendL_END,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendT_START,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendT_END,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SendAll,		NULL,                 ZEND_ACC_PUBLIC)
*/
//  PHP_ME(PhpMsgque_MqS, ConfigReset,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetBuffersize,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetDebug,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetTimeout,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetName,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetSrvName,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIdent,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIsSilent,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIsServer,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIsString,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ConfigSetIgnoreExit,	NULL,                 ZEND_ACC_PUBLIC)
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
/*
  PHP_ME(PhpMsgque_MqS, ServiceGetToken,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceIsTransaction,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceGetFilter,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceProxy,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceCreate,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ServiceDelete,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ProcessEvent,		NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, ErrorC,			NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorSet,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorSetCONTINUE,	NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorIsEXIT,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorGetText,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorGetNum,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorGetCode,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorReset,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, ErrorPrint,		NULL,                 ZEND_ACC_PUBLIC)

  PHP_ME(PhpMsgque_MqS, SlaveWorker,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveCreate,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveDelete,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveGet,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveGetMaster,		NULL,                 ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, SlaveIs,		NULL,                 ZEND_ACC_PUBLIC)
*/

  {NULL, NULL, NULL}
};
/* }}} */

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
  zend_class_entry ce;

  // initialize the other files
  NS(MqS_Config_Init)(TSRMLS_C);
  NS(MqS_Link_Init)(TSRMLS_C);

  // create class "MqS"
  INIT_CLASS_ENTRY(ce,"MqS", PhpMsgque_MqS_functions);
  PhpMsgque_MqS = zend_register_internal_class(&ce TSRMLS_CC);

  // define additional properties "mqctx" to save the "struct MqS *" pointer
  zend_declare_property_null(PhpMsgque_MqS, ID(mqctx), ZEND_ACC_PRIVATE TSRMLS_CC);
}
