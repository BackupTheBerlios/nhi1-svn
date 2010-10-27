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
M0
  SETUP_mqctx;
  //struct MqBufferLS * args = NS(argv2bufl)(NULL,argc,argv);
  struct MqBufferLS * args = NULL;

//printXULS(NULL, args);

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

