/**
 *  \file       theLink/phpmsgque/PhpMsgque/link_php.c
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

static int argArray (zval **, MQ_BFL TSRMLS_DC);

static void MqBufferLAppendZVal(MQ_BFL bufL, zval* arg TSRMLS_DC) {
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
      zend_hash_apply_with_argument(Z_ARRVAL_P(arg), (apply_func_arg_t) argArray, bufL TSRMLS_CC);
      break;
  }
  ZEND_HASH_APPLY_KEEP;
}

static int argArray (zval **argP, MQ_BFL args TSRMLS_DC) {
  MqBufferLAppendZVal (args, *argP TSRMLS_CC);
}

PHP_METHOD(PhpMsgque_MqS, LinkCreate)
{
  int i;
  zval ***arguments;
  SETUP_mqctx;
  //struct MqBufferLS * args = NS(argv2bufl)(NULL,argc,argv);
  struct MqBufferLS * args = NULL;

  if (ZEND_NUM_ARGS() > 0) {
    args = MqBufferLCreate(ZEND_NUM_ARGS());
    arguments = emalloc(sizeof(zval**) * ZEND_NUM_ARGS());
    if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(),arguments)) {
      RETURN_NULL();
    }
    for (i=0; i<ZEND_NUM_ARGS(); i++) {
      MqBufferLAppendZVal (args, *arguments[i] TSRMLS_CC);
    }
  }

  // create Context
  ErrorMqToPhpWithCheck (MqLinkCreate(mqctx, &args));
}

PHP_METHOD(PhpMsgque_MqS, LinkCreateChild)
{
/*
  struct MqS * parent;
  struct MqBufferLS * args = NULL;
  SETUP_mqctx;

  // get parent
  if (argc < 1) rb_raise(rb_eArgError, "usage: LinkCreateChild parent ...");
  CheckType(argv[0], cMqS, "usage: LinkCreateChild MqS-Type-Arg ...");
  parent = VAL2MqS(argv[0]);
  argc--; argv++;

  // command-line arguments to MqBufferLS
  //args = NS(argv2bufl)(NULL,argc,argv);

  // create Context
  ErrorMqToPhpWithCheck (MqLinkCreateChild(mqctx, parent, &args));
*/
}

PHP_METHOD(PhpMsgque_MqS, LinkDelete)
{
  MqLinkDelete(MQCTX);
}

PHP_METHOD(PhpMsgque_MqS, LinkConnect)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck (MqLinkConnect (mqctx));
}

PHP_METHOD(PhpMsgque_MqS, LinkGetParent)
{
  struct MqS * const parent = MqLinkGetParentI(MQCTX);
  MqS2VAL(return_value, parent);
}

PHP_METHOD(PhpMsgque_MqS, LinkIsParent)
{
  RETURN_BOOL(MqLinkIsParent(MQCTX));
}

PHP_METHOD(PhpMsgque_MqS, LinkGetCtxId)
{
  RETURN_LONG(MqLinkGetCtxId(MQCTX));
}

PHP_METHOD(PhpMsgque_MqS, LinkIsConnected)
{
  RETURN_BOOL(MqLinkIsConnected(MQCTX));
}

PHP_METHOD(PhpMsgque_MqS, LinkGetTargetIdent)
{
  RETURN_STRING(MqLinkGetTargetIdent(MQCTX),1);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Link_Init)(TSRMLS_D) {
}

