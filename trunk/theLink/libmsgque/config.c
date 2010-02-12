/**
 *  \file       theLink/libmsgque/config.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

// make MqS::config "write-able"
#include "main.h"
#include "error.h"
#include "token.h"
#include "sys.h"

BEGIN_C_DECLS

#define MQ_CONTEXT_S context

/*****************************************************************************/
/*                                                                           */
/*                                  GC                                       */
/*                                                                           */
/*****************************************************************************/

/// \brief everything \e io need for local storage
struct MqGcS {
#if defined (MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
  pthread_t thread;		///< the thread gc link to
# else
  DWORD thread;			///< the thread gc link to
# endif
#endif
  struct MqS ** DataL;          ///< list of #MqS objects
  MQ_SIZE DataLNum;             ///< size of \e DataL
  MQ_SIZE DataLCur;             ///< first free position in \e DataL, <TT>DataLCur <= DataLNum</TT>
};

void GcCreate (void) __attribute__ ((constructor)); 
void GcDelete (void) __attribute__ ((destructor)); 

#if defined (MQ_HAS_THREAD)
static MqThreadKeyType gc_key = MqThreadKeyNULL;
#else
static struct MqGcS * sysgc = NULL;
#endif

static void
sGcDeleteAll (
  struct MqGcS * const gc
)
{
  MQ_SIZE i;
  for (i=0; i<gc->DataLCur; i++) {
    struct MqS *context = gc->DataL[i];
    if (context != NULL) {
      MqDLogC(context,8,"FORCE delete\n");
      context->bits.onExit = MQ_YES;
      MqContextDelete (&context);
    }
  }
}

void
GcRun (
  struct MqS * const context
)
{
#if defined (MQ_HAS_THREAD)
  struct MqGcS * sysgc = (struct MqGcS *) MqThreadGetTLS(gc_key);
#endif
  if (sysgc != NULL) {
    MQ_INT MqSetDebugLevel(context);
    MQ_SIZE i;
    MqDLogCL(context,8,"START\n");
    for (i=0; i<sysgc->DataLCur; i++) {
      struct MqS *ctx = sysgc->DataL[i];
      if (ctx != NULL && ctx->refCount == 0) {
	MqDLogC(ctx,8,"CLEANUP\n");
	MqContextDelete (&ctx);
      }
    }
    MqDLogCL(context,8,"END\n");
  }
}

void
GcCreate(void)
{
#if defined (MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
    if (pthread_key_create(&gc_key, NULL) != 0) {
      MqPanicC(MQ_ERROR_PANIC,__func__,-1,"unable to 'pthread_key_create'");
    }
# else
    if ((gc_key = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
      MqPanicC(MQ_ERROR_PANIC,__func__,-1,"unable to 'TlsAlloc'");
    }
# endif
#endif
}

void
GcDelete(void)
{
#if defined (MQ_HAS_THREAD)
  if (gc_key == MqThreadKeyNULL) {
    return;
  } else {
    struct MqGcS * sysgc = (struct MqGcS *) MqThreadGetTLS(gc_key);
    if (sysgc != NULL) {
      MqThreadSetTLS(gc_key, NULL);
      sGcDeleteAll(sysgc);
      MqSysFree(sysgc->DataL);
      MqSysFree(sysgc);
    }
  }
#else
  if (sysgc != NULL) {
    sGcDeleteAll(sysgc);
    MqSysFree(sysgc->DataL);
    MqSysFree(sysgc);
  }
#endif
}

static struct MqGcS*
sGcAlloc(
  struct MqS * const context
)
{
#if defined (MQ_HAS_THREAD)
  struct MqGcS * sysgc = NULL;
  MqErrorReset(context);
  if ((sysgc = (struct MqGcS *) MqThreadGetTLS(gc_key)) == NULL) {
    sysgc = (struct MqGcS *) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*sysgc));
    sysgc->thread = MqThreadSelf();
    if (MqThreadSetTLSCheck(gc_key,sysgc)) {
      MqSysFree(sysgc);
      MqPanicC (context, __func__, -1, "unable to alloc Thread-Local-Storage data");
    }
  }
#else
  MqErrorReset(context);
  if (sysgc == NULL) {
    sysgc = (struct MqGcS*) MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof (*sysgc));
  }
#endif
  //MqDLogV(MqErrorGetMsgque(context), 4, "create EVENT<%p>\n", sysgc);
  return sysgc;
}

void
sGcReset(
  struct MqGcS * gc
)
{
  MqSysFree (gc->DataL);
  gc->DataLNum = 0;
  gc->DataLCur = 0;
}

static void
pGcCreate (
  struct MqS * const context
) {
  if (context->gc != NULL) {
    return;
  } else {
    struct MqGcS *sysgc = sGcAlloc(context);

    // if the context was created by a "fork" cleanup gcs first
    if (MQ_IS_PARENT(context) && context->statusIs & MQ_STATUS_IS_FORK) {
      struct MqS **start = sysgc->DataL;
      struct MqS **end   = sysgc->DataL + sysgc->DataLCur;
      // make all "old" context undelete-able
      // reason: perl GarbageCollection will delete these "old" context on
      // exit and will trigger a "full" transaction delete -> "test: slave-Y-1-"
      for (; start < end; start++) {
	// block all "open" context
	(*start)->bits.MqContextDelete_LOCK = MQ_YES;
      }
      // now reset the gc data
      sGcReset(sysgc);
    }

    // check if enough space is available
    if (sysgc->DataLCur >= sysgc->DataLNum) {
      sysgc->DataLNum += 10;
      sysgc->DataL = (struct MqS **) MqSysRealloc (MQ_ERROR_PANIC, sysgc->DataL,
	  sizeof (*sysgc->DataL) * sysgc->DataLNum);
    }
    // ATTENTION: we don't check on double insert !!
    sysgc->DataL[sysgc->DataLCur] = context;

    MqDLogV(context,4,"TSD-Id<%p>, DataLCur<%i>\n", sysgc, sysgc->DataLCur);

    // report gc back
    context->gc = sysgc;
    context->gcid = sysgc->DataLCur;

    sysgc->DataLCur++;
  }
}

static void
pGcDelete (
  struct MqS * const context
) {
  if (context->gc == NULL) {
    return;
  } else {
    struct MqGcS * const sysgc = context->gc;
    sysgc->DataL[context->gcid] = NULL;
    context->gcid = 0;
    context->gc = NULL;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

static void
pConfigInit (
  struct MqS * const context
)
{
  if (context->config.io.buffersize == 0) 
    context->config.io.buffersize = 4096;
  if (context->config.io.timeout == 0) 
    context->config.io.timeout = MQ_TIMEOUT10;
  context->config.io.com = MQ_IO_PIPE;
  context->config.io.uds.file = NULL;
  context->config.io.tcp.host = NULL;
  context->config.io.tcp.port = NULL;
  context->config.io.tcp.myhost = NULL;
  context->config.io.tcp.myport = NULL;
  context->config.io.pipe.socket[0] = -1;
  context->config.io.pipe.socket[1] = -1;
#if !defined(MQ_HAS_THREAD)
  context->config.ignoreThread = MQ_YES;
#endif
#if !defined(HAVE_FORK)
  context->config.ignoreFork = MQ_YES;
#endif
}

void
MqContextInit (
  struct MqS       * const context,
  MQ_SIZE		   size,
  struct MqS const * const tmpl
) {
  if (size == 0) size = sizeof(*context);
  memset(context,0,size);
  pConfigInit (context);
  context->temp = MqBufferCreate(context, 250);
  MqConfigDup(context, tmpl);
  pErrorReset(context);
}

void
MqContextFree (
  struct MqS * const context
)
{
  if (context == NULL || context->bits.MqContextFree_LOCK == MQ_YES) {
    return;
  } else {
    context->bits.MqContextFree_LOCK = MQ_YES;
    MqLinkDelete(context);

    // cleanup setup data entries
    if (context->setup.Event.data && context->setup.Event.fFree) {
      (*context->setup.Event.fFree) (context, &context->setup.Event.data);
    }
    if (context->setup.ServerSetup.data && context->setup.ServerSetup.fFree) {
      (*context->setup.ServerSetup.fFree) (context, &context->setup.ServerSetup.data);
    }
    if (context->setup.ServerCleanup.data && context->setup.ServerCleanup.fFree) {
      (*context->setup.ServerCleanup.fFree) (context, &context->setup.ServerCleanup.data);
    }
    if (context->setup.BgError.data && context->setup.BgError.fFree) {
      (*context->setup.BgError.fFree) (context, &context->setup.BgError.data);
    }
    if (context->setup.Factory.Create.data && context->setup.Factory.Create.fFree) {
      (*context->setup.Factory.Create.fFree) (context, &context->setup.Factory.Create.data);
    }
    if (context->setup.Factory.Delete.data && context->setup.Factory.Delete.fFree) {
      (*context->setup.Factory.Delete.fFree) (context, &context->setup.Factory.Delete.data);
    }

    pErrorCleanup(context);
    MqBufferDelete(&context->temp);

    MqSysFree(context->config.name);
    MqSysFree(context->config.srvname);
    MqSysFree(context->setup.ident);

    MqBufferDelete (&context->config.io.tcp.host);
    MqBufferDelete (&context->config.io.tcp.port);
    MqBufferDelete (&context->config.io.tcp.myhost);
    MqBufferDelete (&context->config.io.tcp.myport);
    MqBufferDelete (&context->config.io.uds.file);

    pGcDelete(context);
  }
}

void
MqContextDelete (
  struct MqS ** contextP
) {
  struct MqS * context = *contextP;
  *contextP = NULL;
  if (context == NULL || context->bits.MqContextDelete_LOCK == MQ_YES) {
    return;
  } else if (context->refCount && context->bits.onExit == MQ_NO) {
    // check on "bits.deleteProtection"
    MqDLogC(context,3,"DELETE protection\n");
    pGcCreate (context);
  } else if (context->setup.Factory.Delete.fCall) {
    MqFactoryDeleteF fCall = context->setup.Factory.Delete.fCall;
    MQ_BOL doFactoryCleanup = context->link.bits.doFactoryCleanup;
    context->link.bits.doFactoryCleanup = MQ_NO;
    context->setup.Factory.Delete.fCall = NULL;
    (*fCall) (context, doFactoryCleanup, context->setup.Factory.Delete.data);
  } else {
    // set this because "setup.Factory.Delete.fCall" is !not! required
    context->link.bits.doFactoryCleanup = MQ_NO;
    context->bits.MqContextDelete_LOCK = MQ_YES;
    MqContextFree (context);
    if (context->contextsize > 0) {
      context->signature = 0;
      MqSysFree(context);
    }
  }
}

struct MqS *
MqContextCreate (
  MQ_SIZE size,
  struct MqS const * const tmpl
) {
  struct MqS * context;
  if (size == 0) size = (tmpl != NULL ? tmpl->contextsize : sizeof(struct MqS));
  context = (struct MqS *) MqSysMalloc(MQ_ERROR_PANIC,size);
  MqContextInit (context, size, tmpl);
  context->statusIs = (tmpl == NULL ? MQ_STATUS_IS_INITIAL : MQ_STATUS_IS_DUP);
  context->contextsize = size;
  context->signature = MQ_MqS_SIGNATURE;
  return context;
}

void
MqConfigReset (
  struct MqS * const context
)
{
  MqSysFree(context->config.name);
  MqSysFree(context->config.srvname);
  MqBufferDelete (&context->config.io.tcp.host);
  MqBufferDelete (&context->config.io.tcp.port);
  MqBufferDelete (&context->config.io.tcp.myhost);
  MqBufferDelete (&context->config.io.tcp.myport);
  MqBufferDelete (&context->config.io.uds.file);
  memset (&context->config, 0, sizeof(context->config));
  pConfigInit (context);
  context->statusIs = MQ_STATUS_IS_INITIAL;
}

void
MqConfigDup (
  struct MqS * const to,
  struct MqS const * const from
)
{
  if (from == NULL) return;
  MqSysFree(to->config.name);
  MqSysFree(to->config.srvname);
  MqBufferDelete (&to->config.io.tcp.host);
  MqBufferDelete (&to->config.io.tcp.port);
  MqBufferDelete (&to->config.io.tcp.myhost);
  MqBufferDelete (&to->config.io.tcp.myport);
  MqBufferDelete (&to->config.io.uds.file);
  to->config = from->config;
  to->config.name    = mq_strdup_save(from->config.name);
  to->config.srvname = mq_strdup_save(from->config.srvname);
  to->config.parent = NULL;
  to->config.master = NULL;
  to->config.master_id = 0;
  to->statusIs = MQ_STATUS_IS_DUP;
  pConfigInit (to);
}

static mq_inline enum MqErrorE
sSetupDupHelper (
  struct MqS * const  context,
  struct MqCallbackS  *cb,
  MQ_PTR	      oldData
)
{
  if (oldData != NULL && cb->data == NULL) {
    // if set in "Step 1", callback set but not needed
    if (cb->fFree) {
      (*cb->fFree) (context, &oldData);
    }
    cb->fFunc = NULL;
    cb->data  = NULL;
  } else if (oldData != NULL && cb->data != NULL) {
    // if set in "Step 2", callback set and needed
    cb->data = oldData;
  } else if (cb->data != NULL && cb->fCopy != NULL) {
    // if set in "Step 3", callback NOT set but needed (copy constructor)
    return (*cb->fCopy) (context, &cb->data);
  } else {
    // if set in "Step 4", callback NOT set but needed (NO copy constructor)
    // -> already done
  }
  return MQ_OK;
}

enum MqErrorE
MqSetupDup (
  struct MqS * const context,
  struct MqS const * const from
)
{
  // Step 1, save all "data" entries because these are already set by the 
  // Class-Constructor proper
  MQ_PTR Event = context->setup.Event.data;
  MQ_PTR ServerSetup = context->setup.ServerSetup.data;
  MQ_PTR ServerCleanup = context->setup.ServerCleanup.data;
  MQ_PTR BgError = context->setup.BgError.data;
  MQ_PTR FactoryC = context->setup.Factory.Create.data;
  MQ_PTR FactoryD = context->setup.Factory.Delete.data;
  enum MqFactoryE FactoryType = context->setup.Factory.type;

  // Step 1,  copy "setup" 
  MqSysFree(context->setup.ident);
  context->setup = from->setup;
  context->setup.ident = mq_strdup_save(from->setup.ident);
  context->setup.Factory.type = FactoryType;

  // reinitialize "data" entries which were !not! set by the class constructor
  MqErrorCheck (sSetupDupHelper (context, &context->setup.Event,	  Event));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.ServerSetup,	  ServerSetup));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.ServerCleanup,  ServerCleanup));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.BgError,	  BgError));
  MqErrorCheck (sSetupDupHelper (context, (struct MqCallbackS*) &context->setup.Factory.Create, FactoryC));
  MqErrorCheck (sSetupDupHelper (context, (struct MqCallbackS*) &context->setup.Factory.Delete, FactoryD));

  return MQ_OK;

error:
  return MqErrorStack(context);
  
}

static enum MqErrorE
sDefaultFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR  data,
  struct MqS  ** contextP
)
{
  *contextP = MqContextCreate (0, tmpl);
  MqErrorCheck (MqSetupDup (*contextP, tmpl));
  return MQ_OK;
error:
  MqErrorCopy(tmpl, *contextP);
  MqContextDelete(contextP);
  return MqErrorGetCodeI (tmpl);
}

enum MqErrorE 
pCallFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS factory,
  struct MqS ** contextP
)
{
  *contextP = NULL;

  // get latest values
  if (factory.Create.fCall == NULL) {
    return MqErrorDbV2(tmpl, MQ_ERROR_CONFIGURATION_REQUIRED, "Factory", "MqSetupS::Factory.Create.fCall");
  }

  // call the factory
  MqErrorCheck((factory.Create.fCall)(tmpl, create, factory.Create.data, contextP));

  // set the factory
  (*contextP)->link.bits.doFactoryCleanup = MQ_YES;
  (*contextP)->setup.Factory.type = create;
  // child inherit "ignoreExit" from "template"
  if (tmpl != NULL && create == MQ_FACTORY_NEW_CHILD)
    (*contextP)->setup.ignoreExit = tmpl->setup.ignoreExit;

  return MQ_OK;

error:
  if (*contextP != NULL) {
    MqErrorCopy(tmpl, *contextP);
    MqContextDelete(contextP);
  }
  return MqErrorGetCodeI (tmpl);
}

/*****************************************************************************/
/*                                                                           */
/*                                 init                                      */
/*                                                                           */
/*****************************************************************************/

// "MqConfigSetName" without duplicate -> for internal use ONLY
void 
pConfigSetName (
  struct MqS * const context,
  MQ_STR  data
) {
  MqSysFree(context->config.name);
  context->config.name = data;
}

void 
MqConfigSetName (
  struct MqS * const context,
  MQ_CST  data
) {
  MqSysFree(context->config.name);
  context->config.name = mq_strdup_save(data);
}

void 
MqConfigSetIdent (
  struct MqS * const context,
  MQ_CST  data
) {
  MqSysFree(context->setup.ident);
  context->setup.ident = mq_strdup_save(data);
}

void 
MqConfigSetSrvName (
  struct MqS * const context,
  MQ_CST  data
) {
  MqSysFree(context->config.srvname);
  context->config.srvname = mq_strdup_save(data);
}

void 
MqConfigSetBuffersize (
  struct MqS * const context,
  MQ_INT  data
) {
  context->config.io.buffersize = data;
}

void 
MqConfigSetDebug (
  struct MqS * const context,
  MQ_INT  data
) {
  context->config.debug = data;
}

void 
MqConfigSetTimeout (
  struct MqS * const context,
  MQ_TIME_T data
) {
  context->config.io.timeout = data;
}

void 
MqConfigSetIsSilent (
  struct MqS * const context,
  MQ_BOL data
) {
  context->config.isSilent = data;
}

void 
MqConfigSetIsServer (
  struct MqS * const context,
  MQ_BOL data
) {
  context->setup.isServer = data;
}

void 
MqConfigSetIsString (
  struct MqS * const context,
  MQ_BOL data
) {
  context->config.isString = data;
}

void 
MqConfigSetIgnoreExit (
  struct MqS * const context,
  MQ_BOL data
) {
  context->setup.ignoreExit = data;
}

void 
pConfigSetParent (
  struct MqS * const context,
  struct MqS * const parent
) {
  context->config.parent = parent;
}

void 
pConfigSetMaster (
  struct MqS * const context,
  struct MqS * const master,
  int master_id
) {
  context->setup.isServer = MQ_NO;
  context->config.master = master;
  context->config.master_id = master_id;
}

void 
MqConfigSetFactory (
  struct MqS * const context,
  MqFactoryCreateF  fCreate,
  MQ_PTR	    CreateData,
  MqTokenDataFreeF  fCreateFree,
  MqTokenDataCopyF  fCreateCopy,
  MqFactoryDeleteF  fDelete,
  MQ_PTR	    DeleteData,
  MqTokenDataFreeF  fDeleteFree,
  MqTokenDataCopyF  fDeleteCopy
) {
//MqDLogV(context,__func__,0,"data<%p>\n", data);
  if (context->setup.Factory.Create.data && context->setup.Factory.Create.fFree) {
    (*context->setup.Factory.Create.fFree) (context, &context->setup.Factory.Create.data);
  }
  if (context->setup.Factory.Delete.data && context->setup.Factory.Delete.fFree) {
    (*context->setup.Factory.Delete.fFree) (context, &context->setup.Factory.Delete.data);
  }
  context->setup.Factory.Create.fCall = fCreate;
  context->setup.Factory.Create.data  = CreateData;
  context->setup.Factory.Create.fFree = fCreateFree;
  context->setup.Factory.Create.fCopy = fCreateCopy;
  context->setup.Factory.Delete.fCall = fDelete;
  context->setup.Factory.Delete.data  = DeleteData;
  context->setup.Factory.Delete.fFree = fDeleteFree;
  context->setup.Factory.Delete.fCopy = fDeleteCopy;
}

void 
MqConfigSetDefaultFactory (
  struct MqS * const context
) {
  context->setup.Factory.Create.fCall = sDefaultFactory;
}

void
MqConfigSetIgnoreFork (
  struct MqS * const context,
  MQ_BOL data
)
{
  context->config.ignoreFork = data;
}

void
MqConfigSetIgnoreThread (
  struct MqS * const context,
  MQ_BOL data
)
{
  context->config.ignoreThread = data;
}

void
MqConfigSetStartAs (
  struct MqS * const context,
  enum MqStartE data
)
{
  context->config.startAs = data;
}

void
MqConfigSetSetup (
  struct MqS * const context,
  MqCreateF fChildCreate,
  MqDeleteF fChildDelete,
  MqCreateF fParentCreate,
  MqDeleteF fParentDelete,
  MqExitF   fProcessExit,
  MqExitF   fThreadExit
)
{
//MqDLogV(context,__func__,0,"%p-%p-%p-%p-%p-%p\n", fChildCreate, fChildDelete, fParentCreate, fParentDelete, fProcessExit, fThreadExit);

  context->setup.Child.fCreate = fChildCreate;
  context->setup.Child.fDelete = fChildDelete;
  context->setup.Parent.fCreate = fParentCreate;
  context->setup.Parent.fDelete = fParentDelete;
  context->setup.fProcessExit = fProcessExit;
  context->setup.fThreadExit = fThreadExit;
}

void
MqConfigSetServerSetup (
  struct MqS * const context,
  MqTokenF fTok,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
)
{
//MqDLogV(context,__func__,0,"data<%p>\n", data);
  if (context->setup.ServerSetup.data && context->setup.ServerSetup.fFree) {
    (*context->setup.ServerSetup.fFree) (context, &context->setup.ServerSetup.data);
  }
  context->setup.isServer	   = MQ_YES;
  context->setup.ServerSetup.fFunc = fTok;
  context->setup.ServerSetup.data  = data;
  if (fFree != NULL) context->setup.ServerSetup.fFree = fFree;
  if (fCopy != NULL) context->setup.ServerSetup.fCopy = fCopy;
}

void
MqConfigSetEvent (
  struct MqS * const context,
  MqTokenF fTok,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
)
{
  if (context->setup.Event.data && context->setup.Event.fFree) {
    (*context->setup.Event.fFree) (context, &context->setup.Event.data);
  }
  context->setup.Event.fFunc = fTok;
  context->setup.Event.data  = data;
  if (fFree != NULL) context->setup.Event.fFree = fFree;
  if (fCopy != NULL) context->setup.Event.fCopy = fCopy;
}

void
MqConfigSetServerCleanup (
  struct MqS * const context,
  MqTokenF fTok,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
)
{
//MqDLogV(context,__func__,0,"data<%p>\n", data);
  if (context->setup.ServerCleanup.data && context->setup.ServerCleanup.fFree) {
    (*context->setup.ServerCleanup.fFree) (context, &context->setup.ServerCleanup.data);
  }
  context->setup.isServer	      = MQ_YES;
  context->setup.ServerCleanup.fFunc  = fTok;
  context->setup.ServerCleanup.data   = data;
  if (fFree != NULL) context->setup.ServerCleanup.fFree  = fFree;
  if (fCopy != NULL) context->setup.ServerCleanup.fCopy  = fCopy;
}

void
MqConfigSetBgError (
  struct MqS * const context,
  MqTokenF fTok,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
)
{
  if (context->setup.BgError.data && context->setup.BgError.fFree) {
    (*context->setup.BgError.fFree) (context, &context->setup.BgError.data);
  }
  context->setup.BgError.fFunc  = fTok;
  context->setup.BgError.data   = data;
  context->setup.BgError.fFree  = fFree;
  context->setup.BgError.fCopy  = fCopy;
}

enum MqErrorE
MqConfigSetIoUds (
  struct MqS * const context,
  MQ_CST fileName
)
{
  // unable to modify the communication setup !after! a link is connected
  if (unlikely(context->link.read != NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "already");
  }
#if defined(MQ_IS_POSIX)
  context->config.io.com = MQ_IO_UDS;
  if (fileName != NULL) context->config.io.uds.file = MqBufferCreateC(context, fileName);
  return MQ_OK;
#else
  return MqErrorC(context, __func__, 1, "UDS sockets are not supported in this environment");
#endif
}

enum MqErrorE
MqConfigSetIoTcp (
  struct MqS * const context,
  MQ_CST host,
  MQ_CST port,
  MQ_CST myhost,
  MQ_CST myport
)
{
  // unable to modify the communication setup !after! a link is connected
  if (unlikely(context->link.read != NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "already");
  }
  context->config.io.com = MQ_IO_TCP;
  if (host   != NULL && *host   != '\0') 
    context->config.io.tcp.host = 
      MqBufferSetC(context->config.io.tcp.host?context->config.io.tcp.host:MqBufferCreate(context,0),host);
  if (port   != NULL && *port   != '\0')
    context->config.io.tcp.port =
      MqBufferSetC(context->config.io.tcp.port?context->config.io.tcp.port:MqBufferCreate(context,0),port);
  if (myhost != NULL && *myhost != '\0')
    context->config.io.tcp.myhost =
      MqBufferSetC(context->config.io.tcp.myhost?context->config.io.tcp.myhost:MqBufferCreate(context,0),myhost);
  if (myport != NULL && *myport != '\0')
    context->config.io.tcp.myport = 
      MqBufferSetC(context->config.io.tcp.myport?context->config.io.tcp.myport:MqBufferCreate(context,0),myport);
  return MQ_OK;
}

enum MqErrorE
MqConfigSetIoPipe (
  struct MqS * const context,
  MQ_SOCK socket
)
{
  // unable to modify the communication setup !after! a link is connected
  if (unlikely(context->link.read != NULL)) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "already");
  }
  context->config.io.com = MQ_IO_PIPE;
  context->config.io.pipe.socket[1] = socket;
  return MQ_OK;
}

enum MqErrorE
MqConfigSetDaemon (
  struct MqS * const context,
  MQ_CST pidfile
)
{
  if (MQ_IS_SERVER(context)) {
    if (context->config.ignoreFork == MQ_YES) {
      return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "local", "--daemon");
    } else {
      MqErrorCheck (SysDaemonize(context, pidfile));
      MqConfigSetIsSilent (context, MQ_YES);
    }
  } else {
    return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "client", "--daemon");
  }
  return MQ_OK;
error:
  return MqErrorStack(context);
}

/*****************************************************************************/
/*                                                                           */
/*                                  get                                      */
/*                                                                           */
/*****************************************************************************/

int
MqConfigGetIsServer (
  struct MqS const * const context
)
{
  return MQ_IS_SERVER(context);
}

int
MqConfigGetIsString (
  struct MqS const * const context
)
{
  return (context->config.isString == MQ_YES);
}

int
MqConfigGetIsSilent (
  struct MqS const * const context
)
{
  return (context->config.isSilent == MQ_YES);
}

int
MqConfigGetIsDupAndThread (
  struct MqS const * const context
)
{
  return ((context->statusIs & MQ_STATUS_IS_DUP) && (context->statusIs & MQ_STATUS_IS_THREAD));
}

MQ_CST 
MqConfigGetName (
  struct MqS const * const context
)
{
  return context->config.name;
}

MQ_CST 
MqConfigGetIdent (
  struct MqS const * const context
)
{
  return context->setup.ident;
}

MQ_CST 
MqConfigGetSrvName (
  struct MqS const * const context
)
{
  return context->config.srvname;
}

MQ_INT
MqConfigGetBuffersize (
  struct MqS const * const context
)
{
  return context->config.io.buffersize;
}

MQ_INT
MqConfigGetDebug (
  struct MqS const * const context
)
{
  return context->config.debug;
}

MQ_TIME_T
MqConfigGetTimeout (
  struct MqS const * const context
)
{
  return context->config.io.timeout;
}

MQ_CST
MqConfigGetIoUdsFile (
  struct MqS * const context
)
{
  MQ_CST s=NULL;
  if (context->config.io.uds.file != NULL)
    MqBufferGetC(context->config.io.uds.file, &s);
  return s;
}

MQ_CST
MqConfigGetIoTcpHost (
  struct MqS * const context
)
{
  MQ_CST s=NULL;
  if (context->config.io.tcp.host != NULL)
    MqBufferGetC(context->config.io.tcp.host, &s);
  return s;
}

MQ_CST
MqConfigGetIoTcpPort (
  struct MqS * const context
)
{
  MQ_CST s=NULL;
  if (context->config.io.tcp.port != NULL)
    MqBufferGetC(context->config.io.tcp.port, &s);
  return s;
}

MQ_CST
MqConfigGetIoTcpMyHost (
  struct MqS * const context
)
{
  MQ_CST s=NULL;
  if (context->config.io.tcp.myhost != NULL)
    MqBufferGetC(context->config.io.tcp.myhost, &s);
  return s;
}

MQ_CST
MqConfigGetIoTcpMyPort (
  struct MqS * const context
)
{
  MQ_CST s=NULL;
  if (context->config.io.tcp.myport != NULL)
    MqBufferGetC(context->config.io.tcp.myport, &s);
  return s;
}

MQ_SOCK
MqConfigGetIoPipeSocket (
  struct MqS * const context
)
{
  return context->config.io.pipe.socket[1];
}

enum MqStartE
MqConfigGetStartAs (
  struct MqS * const context
)
{
  return context->config.startAs;
}

/*****************************************************************************/
/*                                                                           */
/*                             Managed Context                               */
/*                                                                           */
/*****************************************************************************/

void
MqConfigSetSelf (
  struct MqS * const context,
  void * self
) {
  context->self = self;
}

void*
MqConfigGetSelf (
  struct MqS const * const context
) {
  return context->self;
}

END_C_DECLS







