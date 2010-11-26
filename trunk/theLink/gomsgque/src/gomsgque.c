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
MQ_CST sERROR = "Error";
MQ_CST sUNKNOWN = "UNKNOWN";

#define SETUP_data \
  GoInterface *data = (GoInterface*) MqSysMalloc(context, sizeof(GoInterface)); \
  *data = ifc;

static enum MqErrorE
gomsgque_sServerSetup
 (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cServerSetup((int*)context, *((GoInterface*)data));
  return MqErrorStack(context);
}

void
gomsgque_ConfigSetServerSetup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerSetup(context, gomsgque_sServerSetup, (MQ_PTR)data, NULL, NULL);
}

static enum MqErrorE
gomsgque_sServerCleanup (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cServerCleanup((int*)context, *((GoInterface*)data));
  return MqErrorStack(context);
}

void
gomsgque_ConfigSetServerCleanup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerCleanup(context, gomsgque_sServerCleanup, (MQ_PTR)data, NULL, NULL);
}

static enum MqErrorE
gomsgque_sBgError (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cBgError((int*)context, *((GoInterface*)data));
  return MqErrorStack(context);
}

void
gomsgque_ConfigSetBgError (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetBgError(context, gomsgque_sBgError, (MQ_PTR)data, NULL, NULL);
}

static enum MqErrorE
gomsgque_sEvent (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cEvent((int*)context, *((GoInterface*)data));
  return MqErrorStack(context);
}

void
gomsgque_ConfigSetEvent (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetEvent(context, gomsgque_sEvent, (MQ_PTR)data, NULL, NULL);
}

static enum MqErrorE
gomsgque_sFactoryCreate (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR  data,
  struct MqS  ** contextP
)
{
  *contextP = (struct MqS*)gomsgque_cFactoryCreate((int*)tmpl, *((GoInterface*)data));
  if (*contextP == NULL) return MqErrorStack(tmpl);
  MqConfigDup (*contextP, tmpl);
  MqErrorCheck(MqSetupDup (*contextP, tmpl));
  return MQ_OK;
error:
  MqErrorCopy(tmpl, *contextP);
  return MqErrorStack(tmpl);
}

// only necessary if we have to "unlock" an object
static void
gomsgque_sFactoryDelete (
  struct MqS  * context,
  MQ_BOL doFactoryCleanup,
  MQ_PTR data
)
{
  gomsgque_cFactoryDelete((int*)context);
  MqContextDelete (&context);
}

void
gomsgque_ConfigSetFactory (
  struct MqS * const context,
  void *data
)
{
  MqFactoryCreateF  fCreate = data != NULL ? gomsgque_sFactoryCreate : NULL;
  MqConfigSetFactory(context, 
    fCreate,		      (MQ_PTR)data, NULL, NULL,
    gomsgque_sFactoryDelete,  NULL,	    NULL, NULL
  );
}

static enum MqErrorE
gomsgque_sService (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cService((int*)context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
gomsgque_sServiceFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  gomsgque_decrServiceRef((GoInterface*)(*dataP));
  *dataP = NULL;
}

static enum MqErrorE
gomsgque_sService2 (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cService2((int*)context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
gomsgque_sService2Free (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  gomsgque_decrService2Ref((GoInterface*)(*dataP));
  *dataP = NULL;
}

enum MqErrorE
gomsgque_ServiceCreate (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqServiceCreate(context, token, gomsgque_sService, data, gomsgque_sService2Free);
}

enum MqErrorE
gomsgque_ServiceCreate2 (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqServiceCreate(context, token, gomsgque_sService2, (MQ_PTR)data, gomsgque_sServiceFree);
}

enum MqErrorE
gomsgque_SendEND_AND_CALLBACK (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqSendEND_AND_CALLBACK(context, token, gomsgque_sService, (MQ_PTR)data, gomsgque_sServiceFree);
}

enum MqErrorE
gomsgque_SendEND_AND_CALLBACK2 (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqSendEND_AND_CALLBACK(context, token, gomsgque_sService2, (MQ_PTR)data, gomsgque_sService2Free);
}

void
gomsgque_ConfigSetSetup (
  struct MqS * const context
)
{
  return MqConfigSetSetup(context, MqLinkDefault, NULL, MqLinkDefault, NULL, NULL, NULL);
}


