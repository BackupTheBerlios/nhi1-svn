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

MQ_CST sGO = "GO";
MQ_CST sUNKNOWN = "UNKNOWN";

static enum MqErrorE sServerSetup (
  struct MqS * const context,
  MQ_PTR const data
)
{
  MqLogC(context,"sServerSetup",0,"INFO\n");
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

static enum MqErrorE sService (
  struct MqS * const context,
  MQ_PTR const data
)
{
  MqLogC(context,"sService",0,"INFO\n");
  cService((int*)context->self, data);
  return MqErrorStack(context);
}

void
gomsgque_ServiceCreate (
  struct MqS * const context,
  MQ_TOK const token,
  void *data
)
{
  MqServiceCreate(context, token, sService, (MQ_PTR)data, NULL);
}

