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

#define MQ_CONTEXT_S context

MQ_CST sGO = "GO";
MQ_CST sERROR = "Error";
MQ_CST sUNKNOWN = "UNKNOWN";

#define SETUP_data \
  GoInterface *data = (GoInterface*) MqSysMalloc(context, sizeof(GoInterface)); \
  *data = ifc;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
gomsgque_sServerSetup
 (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cServerSetup((int*)context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
gomsgque_sServerSetupFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  gomsgque_cServerSetupFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetServerSetup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerSetup(context, gomsgque_sServerSetup, (MQ_PTR)data, gomsgque_sServerSetupFree, NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
gomsgque_sServerCleanup (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cServerCleanup((int*)context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
gomsgque_sServerCleanupFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  gomsgque_cServerCleanupFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetServerCleanup (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetServerCleanup(context, gomsgque_sServerCleanup, (MQ_PTR)data, gomsgque_sServerCleanupFree, NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
gomsgque_sBgError (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cBgError((int*)context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
gomsgque_sBgErrorFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  gomsgque_cBgErrorFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetBgError (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetBgError(context, gomsgque_sBgError, (MQ_PTR)data, gomsgque_sBgErrorFree, NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
gomsgque_sEvent (
  struct MqS * const context,
  MQ_PTR const data
)
{
  gomsgque_cEvent((int*)context, (GoInterface*)data);
  return MqErrorStack(context);
}

static void
gomsgque_sEventFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  gomsgque_cEventFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetEvent (
  struct MqS * const context,
  void *data
)
{
  MqConfigSetEvent(context, gomsgque_sEvent, (MQ_PTR)data, gomsgque_sEventFree, NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static enum MqErrorE
gomsgque_sFactoryCreate (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR  data,
  struct MqS  ** contextP
)
{
  *contextP = (struct MqS*)gomsgque_cFactoryCreate((int*)tmpl, (GoInterface*)data);
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
  MQ_PTR chnp = context->threadData;
  MqContextDelete (&context);
  gomsgque_cFactoryDelete((int*)context, chnp);
}

static void
gomsgque_sFactoryFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  gomsgque_cFactoryFree((GoInterface*)(*dataP));
  *dataP = NULL;
}

void
gomsgque_ConfigSetFactory (
  struct MqS * const context,
  MQ_PTR data
)
{
  MqFactoryCreateF  fCreate = data != NULL ? gomsgque_sFactoryCreate : NULL;
  MqConfigSetFactory(context, 
    fCreate,		      data, gomsgque_sFactoryFree, NULL,
    gomsgque_sFactoryDelete,  NULL, NULL, NULL
  );
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

enum MqErrorE
gomsgque_ServiceCreate (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqServiceCreate(context, token, gomsgque_sService, data, gomsgque_sServiceFree);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
gomsgque_ServiceCreate2 (
  struct MqS * const context,
  MQ_TOK const token,
  MQ_PTR data
)
{
  return MqServiceCreate(context, token, gomsgque_sService2, (MQ_PTR)data, gomsgque_sService2Free);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void
sProcessExit (int num) {
  gomsgque_ProcessExit(num);
}

void
sThreadExit (int num) {
  gomsgque_ThreadExit(num);
}

void
gomsgque_ConfigSetSetup (
  struct MqS * const context
)
{
  return MqConfigSetSetup(context, MqLinkDefault, NULL, MqLinkDefault, 
	  NULL, sProcessExit, sThreadExit);
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

static enum MqErrorE gomsgque_SysServerThread (
  struct MqS * const context,
  struct MqFactoryS factory,
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
  ret = gomsgque_CreateThread((int*)argP);

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

static enum MqErrorE gomsgque_SysWait (
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
      gomsgque_WaitForThread((GoChan)idP->val);
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
  MqLal.SysServerThread	  =   gomsgque_SysServerThread;
  MqLal.SysWait		  =   gomsgque_SysWait;
}

