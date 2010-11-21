/**
 *  \file       theLink/gomsgque/src/gomsgque.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "gomsgque.h"
#include "_cgo_export.h"

MQ_CST sGO = "GO";
MQ_CST sUNKNOWN = "UNKNOWN";

void
gomsgque_ConfigSetServerSetup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerSetup(context, (MqTokenF)gomsgque_cServerSetup, (MQ_PTR)data, NULL, NULL);
}

void
gomsgque_ConfigSetServerCleanup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerCleanup(context, (MqTokenF)gomsgque_cServerCleanup, (MQ_PTR)data, NULL, NULL);
}

void
gomsgque_ServiceCreate (
  struct MqS * const context,
  MQ_TOK const token,
  void *data
)
{
  MqServiceCreate(context, token, (MqTokenF)gomsgque_cService, (MQ_PTR)data, NULL);
}

