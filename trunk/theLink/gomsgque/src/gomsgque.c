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

static enum MqErrorE sServerSetup (
  struct MqS * const context,
  MQ_PTR const data
)
{
  cServerSetup((int*)context->self, data);
  return MqErrorStack(context);
}

void
gomsgque_ConfigSetServerSetup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerSetup(context, sServerSetup, (MQ_PTR)data, NULL, NULL);
}

/*
static enum MqErrorE sService (
  struct MqS * const context,
  MQ_PTR const data
)
{
  cService((int*)context->self, data);
  return MqErrorStack(context);
}
*/

void
gomsgque_ServiceCreate (
  struct MqS * const context,
  MQ_TOK const token,
  void *data
)
{
  MqServiceCreate(context, token, (MqTokenF)gomsgque_cService, (MQ_PTR)data, NULL);
}

