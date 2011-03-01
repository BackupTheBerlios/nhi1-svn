/**
 *  \file       theLink/msgqueforphp/MsgqueForPhp/storage_php.c
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
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

PHP_METHOD(MsgqueForPhp_MqS, StorageOpen)
{
  SETUP_mqctx;
  ARG2CST(StorageOpen,storageFile);
  ErrorMqToPhpWithCheck(MqStorageOpen(mqctx,storageFile));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, StorageClose)
{
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqStorageClose(mqctx));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, StorageInsert)
{
  MQ_TRA transLId;
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqStorageInsert(mqctx, &transLId));
  WID2VAL(return_value, transLId);
}

PHP_METHOD(MsgqueForPhp_MqS, StorageSelect)
{
  SETUP_mqctx;
  ARG2WID_OPT(StorageInsert,transLId);
  ErrorMqToPhpWithCheck(MqStorageSelect(mqctx, &transLId));
  WID2VAL(return_value, transLId);
}

PHP_METHOD(MsgqueForPhp_MqS, StorageDelete)
{
  SETUP_mqctx;
  ARG2WID(StorageDelete,transLId);
  ErrorMqToPhpWithCheck(MqStorageDelete(mqctx, transLId));
  RETURN_NULL();
}

PHP_METHOD(MsgqueForPhp_MqS, StorageCount)
{
  MQ_TRA count;
  SETUP_mqctx;
  ErrorMqToPhpWithCheck(MqStorageCount(mqctx, &count));
  WID2VAL(return_value, count);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Storage_Init)(void) {
}

