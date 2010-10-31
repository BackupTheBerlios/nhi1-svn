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

PHP_METHOD(PhpMsgque_MqS, LinkCreate)
{
  SETUP_mqctx;
  struct MqBufferLS * args = NS(Argument2MqBufferLS)(ZEND_NUM_ARGS() TSRMLS_CC) ;
  ErrorMqToPhpWithCheck (MqLinkCreate(mqctx, &args));
}

PHP_METHOD(PhpMsgque_MqS, LinkCreateChild)
{
  SETUP_mqctx;
  struct MqBufferLS * args = NULL;
  struct MqS * parent = NULL;
  int argc = ZEND_NUM_ARGS();
  zval ***argv;

  // get parent
  if (argc < 1) RaiseError("usage: LinkCreateChild parent ...");
  argv = emalloc(sizeof(zval**) * argc);
  zend_get_parameters_array_ex(argc, argv);
  CheckType(*argv[0], NS(MqS), "usage: LinkCreateChild MqS-Type-Arg ...");
  parent = VAL2MqS(*argv[0]);
  argc--; argv++;

  // read other args
  if (argc > 0) {
    int i;
    args = MqBufferLCreate(argc);
    for (i=0; i<argc; i++) {
      NS(MqBufferLAppendZVal) (args, *argv[i] TSRMLS_CC);
    }
  }

  efree(argv);

  // create Context
  ErrorMqToPhpWithCheck (MqLinkCreateChild(mqctx, parent, &args));
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

