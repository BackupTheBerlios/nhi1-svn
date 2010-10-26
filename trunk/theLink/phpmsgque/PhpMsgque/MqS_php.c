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

static zend_class_entry *PhpMsgque_MqS;

int le_MqS;

static
PHP_METHOD(PhpMsgque_MqS, __construct)
{
M0
  long MqS_id;
  struct MqS * mqctx = (struct MqS *) MqContextCreate(sizeof (*mqctx), NULL);
printP(mqctx)

  // create ruby command
  mqctx->self = (void*) this_ptr;

  // set configuration data
  mqctx->setup.Child.fCreate   = MqLinkDefault;
  mqctx->setup.Parent.fCreate  = MqLinkDefault;

  // register resources
  MqS_id = zend_register_resource(NULL, mqctx, le_MqS);
  zend_list_addref(MqS_id);
  add_property_resource(this_ptr, "_MqS_ptr", MqS_id);
}

static
PHP_METHOD(PhpMsgque_MqS, __destruct)
{
M0
  SETUP_mqctx;
printP(mqctx)
  mqctx->setup.Factory.Delete.fCall = NULL;
  mqctx->setup.Event.fCall = NULL;
  mqctx->self = NULL;
  MqContextDelete(&mqctx);
}

PHP_METHOD(PhpMsgque_MqS, LinkCreate);
PHP_METHOD(PhpMsgque_MqS, LinkCreateChild);
PHP_METHOD(PhpMsgque_MqS, LinkDelete);
PHP_METHOD(PhpMsgque_MqS, LinkConnect);
PHP_METHOD(PhpMsgque_MqS, LinkGetParent);
PHP_METHOD(PhpMsgque_MqS, LinkIsParent);
PHP_METHOD(PhpMsgque_MqS, LinkGetCtxId);
PHP_METHOD(PhpMsgque_MqS, LinkIsConnected);
PHP_METHOD(PhpMsgque_MqS, LinkGetTargetIdent);

/* {{{ PhpMsgque_MqS_functions[]
 *
 * Every class method must have an entry in PhpMsgque_MqS_functions[].
 */
static const zend_function_entry MqS_functions[] = {
  PHP_ME(PhpMsgque_MqS, __construct,	    NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(PhpMsgque_MqS, __destruct,	    NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  PHP_ME(PhpMsgque_MqS, LinkCreate,	    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkCreateChild,    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkDelete,	    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkConnect,	    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkGetParent,	    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkIsParent,	    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkGetCtxId,	    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkIsConnected,    NULL, ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqS, LinkGetTargetIdent, NULL, ZEND_ACC_PUBLIC)
  {NULL, NULL, NULL}
};
/* }}} */

void NS(MqS_Init) (TSRMLS_D)
{
  zend_class_entry ce;
  INIT_CLASS_ENTRY(ce,"MqS", MqS_functions);
  PhpMsgque_MqS = zend_register_internal_class(&ce TSRMLS_CC);
}
