/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/slave_php.c
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

PHP_METHOD(MsgqueForPhp_MqS, SlaveWorker)
{
  SETUP_mqctx;
  struct MqBufferLS * args = NULL;
  MQ_INT id = 0;
  int argc = ZEND_NUM_ARGS();
  zval ***argv;

  // get id
  if (argc < 1) goto error;
  argv = emalloc(sizeof(zval*) * argc);
  zend_get_parameters_array_ex(argc, argv);
  convert_to_long(*argv[0]);
  id = VAL2INT(*argv[0]);

  // read other args
  if (argc > 1) {
    int i;
    args = MqBufferLCreate(argc-1);
    for (i=1; i<argc; i++) {
      NS(MqBufferLAppendZVal) (args, *argv[i] TSRMLS_CC);
    }
  }
  efree(argv);

  // create Worker
  ErrorMqToPhpWithCheck (MqSlaveWorker(mqctx, id, &args));
  RETURN_NULL();
error:
  RaiseError("usage: SlaveWorker(integer: id, ...)");
  return;
}

PHP_METHOD(MsgqueForPhp_MqS, SlaveCreate)
{
  SETUP_mqctx;
  long id;
  zval *mqs;
  PhpErrorCheck(zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "lo", &id, &mqs));
  CheckType(mqs, NS(MqS))
  ErrorMqToPhpWithCheck (MqSlaveCreate(mqctx, id, VAL2MqS(mqs)));
  RETURN_NULL();
error:
  RaiseError("usage: SlaveCreate(integer: id, MqS: ctx)");
}

PHP_METHOD(MsgqueForPhp_MqS, SlaveDelete)
{
  SETUP_mqctx;
  ARG2INT(SlaveDelete,id);
  ErrorMqToPhpWithCheck (MqSlaveDelete(mqctx, id));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, SlaveGet)
{
  ARG2INT(SlaveGet,id);
  MqS2VAL(return_value, MqSlaveGet(MQCTX,id));
}

PHP_METHOD(MsgqueForPhp_MqS, SlaveGetMaster)
{
  MqS2VAL(return_value, MqSlaveGetMaster(MQCTX));
}

PHP_METHOD(MsgqueForPhp_MqS, SlaveIs)
{
  RETURN_BOOL(MqSlaveIs(MQCTX));
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Slave_Init)(TSRMLS_D) {
}




