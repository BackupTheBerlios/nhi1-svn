/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/link_php.c
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

PHP_METHOD(MsgqueForPhp_MqS, LinkCreate)
{
  SETUP_mqctx;
  struct MqBufferLS * args = NS(Argument2MqBufferLS)(NULL, ZEND_NUM_ARGS() TSRMLS_CC) ;
  ErrorMqToPhpWithCheck (MqLinkCreate(mqctx, &args));
}

PHP_METHOD(MsgqueForPhp_MqS, LinkCreateChild)
{
  SETUP_mqctx;
  struct MqBufferLS * args = NULL;
  struct MqS * parent = NULL;
  int argc = ZEND_NUM_ARGS();
  zval ***argv;

  // get parent
  if (argc < 1) goto error;
  argv = emalloc(sizeof(zval**) * argc);
  zend_get_parameters_array_ex(argc, argv);
  CheckType(*argv[0], NS(MqS));
  VAL2MqS(parent, *argv[0]);

  // read other args
  if (argc > 1) {
    int i;
    args = MqBufferLCreate(argc-1);
    for (i=1; i<argc; i++) {
      NS(MqBufferLAppendZVal) (args, *argv[i] TSRMLS_CC);
    }
  }
  efree(argv);

  // create Context
  ErrorMqToPhpWithCheck (MqLinkCreateChild(mqctx, parent, &args));
  RETURN_NULL();
error:
  RETURN_ERROR("usage: LinkCreateChild(MqS: parent, ...)");
}

PHP_METHOD(MsgqueForPhp_MqS, LinkDelete)
{
  SETUP_mqctx;
  MqLinkDelete(mqctx);
}

PHP_METHOD(MsgqueForPhp_MqS, LinkConnect)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck (MqLinkConnect (mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, LinkGetParent)
{
  SETUP_mqctx;
  struct MqS * const parent = MqLinkGetParentI(mqctx);
  MqS2VAL(return_value, parent);
}

PHP_METHOD(MsgqueForPhp_MqS, LinkIsParent)
{
  SETUP_mqctx;
  RETURN_BOOL(MqLinkIsParent(mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, LinkGetCtxId)
{
  SETUP_mqctx;
  RETURN_LONG(MqLinkGetCtxId(mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, LinkIsConnected)
{
  SETUP_mqctx;
  RETURN_BOOL(MqLinkIsConnected(mqctx));
}

PHP_METHOD(MsgqueForPhp_MqS, LinkGetTargetIdent)
{
  SETUP_mqctx;
  RETURN_STRING(MqLinkGetTargetIdent(mqctx),1);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Link_Init)(TSRMLS_D) {
}





