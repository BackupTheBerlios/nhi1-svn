/**
 *  \file       theLink/gomsgque/src/gomsgque/gomsgque.c
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

#define MQ_CONTEXT_S context

MQ_CST sGO = "GO";
MQ_CST sERROR = "Error";

#define SETUP_data \
  GoInterface *data = (GoInterface*) MqSysMalloc(context, sizeof(GoInterface)); \
  *data = ifc;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
sServerSetup
 (
  struct MqS * const context,
  MQ_PTR const data
)
{
  cServerSetup(context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
sServerSetupFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  cServerSetupFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetServerSetup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerSetup(context, sServerSetup, (MQ_PTR)data, sServerSetupFree, NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
sServerCleanup (
  struct MqS * const context,
  MQ_PTR const data
)
{
  cServerCleanup(context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
sServerCleanupFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  cServerCleanupFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetServerCleanup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerCleanup(context, sServerCleanup, (MQ_PTR)data, sServerCleanupFree, NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
sBgError (
  struct MqS * const context,
  MQ_PTR const data
)
{
  cBgError(context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
sBgErrorFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  cBgErrorFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetBgError (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetBgError(context, sBgError, (MQ_PTR)data, sBgErrorFree, NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
sEvent (
  struct MqS * const context,
  MQ_PTR const data
)
{
  cEvent(context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
sEventFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  cEventFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetEvent (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetEvent(context, sEvent, (MQ_PTR)data, sEventFree, NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
sFactoryCreate (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS * const item,
  struct MqS  ** contextP
)
{
  struct MqS * mqctx = (struct MqS*)cFactoryCall(tmpl, (void**)item->Create.data);
  if (mqctx == NULL) return MqErrorStack(tmpl);

  // copy setup data and initialize "setup" data
  if (create != MQ_FACTORY_NEW_INIT) {
    MqSetupDup (mqctx, tmpl);
  }

  *contextP = mqctx;
  return MQ_OK;
}

// only necessary if we have to "unlock" an object
static void
sFactoryDelete (
  struct MqS  * context,
  MQ_BOL doFactoryCleanup,
  struct MqFactoryS* const item
)
{
  MQ_PTR chnp = context->threadData;
  MqContextDelete (&context);
  cFactoryDelete(context, chnp);
}

static void
sFactoryFree (
  MQ_PTR *dataP
)
{
  decrFactoryRef((void**)(*dataP));
  *dataP = NULL;
}

static void
sFactoryCopy (
  MQ_PTR *dataP
)
{
  incrFactoryRef((void**)(*dataP));
}

struct MqFactoryS * 
gomsgque_FactoryAdd (
  MQ_CST const ident,
  MQ_PTR data
)
{
  return MqFactoryAdd(MQ_ERROR_PRINT, ident,
    sFactoryCreate, data, sFactoryFree, sFactoryCopy,
    sFactoryDelete, NULL, NULL, NULL
  );
}

struct MqFactoryS * 
gomsgque_FactoryDefault (
  MQ_CST const ident,
  MQ_PTR data
)
{
  return MqFactoryDefault(MQ_ERROR_PRINT, ident,
    sFactoryCreate, data, sFactoryFree, sFactoryCopy,
    sFactoryDelete, NULL, NULL, NULL
  );
}

struct MqS *
gomsgque_FactoryNew (
  struct MqFactoryS * const item
)
{
  return MqFactoryNew(MQ_ERROR_PRINT, NULL, item);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
sService (
  struct MqS * const context,
  MQ_PTR const data
)
{
  cService(context, (GoInterface*)data);
  return MqErrorGetCode(context);
}

static void
sServiceFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  decrServiceRef((GoInterface*)(*dataP));
  *dataP = NULL;
}

enum MqErrorE
gomsgque_ServiceCreate (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqServiceCreate(context, token, sService, data, sServiceFree);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
sService2 (
  struct MqS * const context,
  MQ_PTR const data
)
{
  cService2(context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
sService2Free (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  decrService2Ref((GoInterface*)(*dataP));
  *dataP = NULL;
}

enum MqErrorE
gomsgque_ServiceCreate2 (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqServiceCreate(context, token, sService2, (MQ_PTR)data, sService2Free);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

enum MqErrorE
gomsgque_SendEND_AND_CALLBACK (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqSendEND_AND_CALLBACK(context, token, sService, (MQ_PTR)data, sServiceFree);
}

enum MqErrorE
gomsgque_SendEND_AND_CALLBACK2 (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqSendEND_AND_CALLBACK(context, token, sService2, (MQ_PTR)data, sService2Free);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void
sProcessExit (int num) {
  ProcessExit(num);
}

void
sThreadExit (int num) {
  ThreadExit(num);
}

void
gomsgque_ConfigSetSetup (
  struct MqS * const context
)
{
  return MqConfigSetSetup(context, MqLinkDefault, NULL, MqLinkDefault, NULL, sProcessExit, sThreadExit);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include <pthread.h>
#include <errno.h>
typedef GoChan mqthread_t;

#define mqthread_ret_t void*
#define mqthread_stdcall

void
gomsgque_SysServerThreadMain (
  struct MqSysServerThreadMainS *data,
  MQ_PTR chn
)
{
  MqBufferLAppendC(data->argv, "---threadData");
  MqBufferLAppendW(data->argv, (MQ_WID)chn);
  MqSysServerThreadMain(data);
}

static enum MqErrorE SysServerThread (
  struct MqS * const context,
  struct MqFactoryS *factory,
  struct MqBufferLS ** argvP,
  struct MqBufferLS ** alfaP,
  MQ_CST  name,
  int state,   
  struct MqIdS * idP
)
{
  mqthread_t threadId;
  mqthread_t ret;  

  // fill thread data
  struct MqSysServerThreadMainS * argP = (struct MqSysServerThreadMainS *) MqSysMalloc(MQ_ERROR_PANIC,sizeof(*argP));
  argP->factory = factory;
  argP->argv = *argvP;
  argP->alfa = *alfaP;
  argP->tmpl = context; 

  // pointers are owned by SysServerThread
  *argvP = NULL;
  *alfaP = NULL;

  // after a "thread" no "fork" is possible
  MqConfigSetIgnoreFork (context, MQ_YES);

  // start thread as "go" routine
  ret = CreateThread(argP);

  // save tid
  (*idP).val = (MQ_IDNT)ret;
  (*idP).type = MQ_ID_THREAD;

  return MQ_OK;

error:
  MqBufferLDelete (&argP->argv);
  MqBufferLDelete (&argP->alfa);
  MqSysFree (argP);
  return MQ_ERROR;
}

static enum MqErrorE SysWait (
  struct MqS * const context,
  const struct MqIdS *idP
)
{
  errno = 0;
  switch (idP->type) {  
    case MQ_ID_PROCESS: { 
      break;
    }
    case MQ_ID_THREAD: {
      WaitForThread((GoChan)idP->val);
      break;
    }
    case MQ_ID_UNUSED: {
      break;
    }
  }
  return MQ_OK;
}

void
gomsgque_Init() {
  MqLal.SysServerThread	  =   SysServerThread;
  MqLal.SysWait		  =   SysWait;

  if (!strcmp(MqFactoryDefaultIdent(),"libmsgque"))
    MqFactoryDefault(MQ_ERROR_PANIC, "gomsgque", 
      sFactoryCreate, NULL, NULL, NULL, sFactoryDelete, NULL, NULL, NULL);
}

