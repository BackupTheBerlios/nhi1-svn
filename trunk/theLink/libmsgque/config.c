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
#include "factory.h"

BEGIN_C_DECLS

extern struct MqFactoryItemS *defaultFactoryItem;

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
	if (*start != NULL) (*start)->bits.MqContextDelete_LOCK = MQ_YES;
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
  if (tmpl == NULL) {
    MqFactoryCtxItem(context,defaultFactoryItem);
  } else {
    MqConfigDup(context, tmpl);
  }
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
    MqDLogC(context,3,"FREE\n");
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

    pErrorCleanup(context);
    MqBufferDelete(&context->temp);

    MqSysFree(context->config.name);
    MqSysFree(context->config.srvname);

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
  } else if (context->setup.factory != NULL) {
    struct MqFactoryItemS *item = context->setup.factory;
    if (item->Delete.fCall != NULL) {
      MQ_BOL doFactoryCleanup = context->link.bits.doFactoryCleanup;
      context->link.bits.doFactoryCleanup = MQ_NO;
      context->setup.factory = NULL;
      (*item->Delete.fCall) (context, doFactoryCleanup, item);
    } else {
      // without default factory-cleanup continue with "normal" cleanup
      goto CONT;
    }
  } else {
CONT:
    // set this because "Factory.Delete.fCall" is !not! required
    MqDLogC(context,3,"DELETE\n");
    context->link.bits.doFactoryCleanup = MQ_NO;
    context->bits.MqContextDelete_LOCK = MQ_YES;
    MqContextFree(context);
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
  context->contextsize = size;
  context->signature = MQ_MqS_SIGNATURE;
  MqDLogC(context,3,"CREATE\n");
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
  to->config		= from->config;
  to->config.name	= MqSysStrDup(MQ_ERROR_PANIC, from->config.name);
  to->config.srvname	= NULL;
  to->config.parent	= NULL;
  to->config.master	= NULL;
  to->config.master_id	= 0;
  to->statusIs		= MQ_STATUS_IS_DUP;
  pConfigInit (to);
}

#define CheckCopyStr(P) \
  if (context->setup.P == NULL && from->setup.P != NULL) \
    context->setup.P = MqSysStrDup(MQ_ERROR_PANIC, from->setup.P)

#define CheckCopyPtr(P) \
  if (context->setup.P == NULL && from->setup.P != NULL) \
    context->setup.P = from->setup.P

#define CheckCopyData(P,C) \
  if (context->setup.P == NULL && from->setup.P != NULL && from->setup.C != NULL) { \
    context->setup.P = from->setup.P; \
    (*from->setup.C) (context, &context->setup.P); \
  }

#define CheckCopyCallback(P) \
  CheckCopyPtr(P.fCall); \
  CheckCopyPtr(P.fFree); \
  CheckCopyPtr(P.fCopy); \
  CheckCopyData(P.data, P.fCopy)

void
MqSetupDup (
  register struct MqS * const context,
  register struct MqS const * const from
)
{
  // !!attention!! copy only data from "from" to "context" not
  // available in "context" -> this is called a "merge"

  // protect the code
  if (from == NULL) return;

  // 2. factory
  CheckCopyPtr(factory);

  // 3.	Child
  CheckCopyPtr(Child.fCreate);
  CheckCopyPtr(Child.fDelete);

  // 4.	Parent
  CheckCopyPtr(Parent.fCreate);
  CheckCopyPtr(Parent.fDelete);

  // 5. Callback's
  CheckCopyCallback(BgError);
  CheckCopyCallback(ServerSetup);
  CheckCopyCallback(ServerCleanup);
  CheckCopyCallback(Event);

  // 6. Init-Proc's
  CheckCopyPtr(fThreadInit);
  CheckCopyPtr(fForkInit);
  CheckCopyPtr(fSpawnInit);
  CheckCopyPtr(fProcessExit);
  CheckCopyPtr(fThreadExit);
  CheckCopyPtr(fHelp);

  // 7. misc
  context->setup.isServer = from->setup.isServer;
  context->setup.ignoreExit = from->setup.ignoreExit;
}

static enum MqErrorE
sDefaultFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryItemS* item,
  struct MqS  ** contextP
)
{
  *contextP = MqContextCreate (0, tmpl);
  // I don't know anything about the target
  // -> use the template as source of the setup
  MqSetupDup (*contextP, tmpl);
  return MQ_OK;
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
  if (context->config.name == NULL) {
    context->config.name = MqSysStrDup(MQ_ERROR_PANIC, data);
  } else if (strcmp(context->config.name, data)) {
    MqSysFree(context->config.name);
    context->config.name = MqSysStrDup(MQ_ERROR_PANIC, data);
  }
  if (MQ_IS_SERVER(context)) {
    MqSysFree(context->config.srvname);
    context->config.srvname = MqSysStrDup(MQ_ERROR_PANIC, "LOCK");
  }
}

void 
MqConfigUpdateName (
  struct MqS * const context,
  MQ_CST  data
) {
  if (context->config.name == NULL || !strcmp(context->config.name,defaultFactoryItem->ident)) {
    MqConfigSetName(context, data);
  }
}

void 
MqConfigSetSrvName (
  struct MqS * const context,
  MQ_CST  data
) {
  MqSysFree(context->config.srvname);
  context->config.srvname = MqSysStrDup(MQ_ERROR_PANIC, data);
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
  if (context->setup.ServerSetup.data && context->setup.ServerSetup.fFree) {
    (*context->setup.ServerSetup.fFree) (context, &context->setup.ServerSetup.data);
  }
  context->setup.isServer	   = MQ_YES;
  context->setup.ServerSetup.fCall = fTok;
  context->setup.ServerSetup.data  = data;
  context->setup.ServerSetup.fFree = fFree;
  context->setup.ServerSetup.fCopy = fCopy;
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
  context->setup.Event.fCall = fTok;
  context->setup.Event.data  = data;
  context->setup.Event.fFree = fFree;
  context->setup.Event.fCopy = fCopy;
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
  context->setup.ServerCleanup.fCall  = fTok;
  context->setup.ServerCleanup.data   = data;
  context->setup.ServerCleanup.fFree  = fFree;
  context->setup.ServerCleanup.fCopy  = fCopy;
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
  context->setup.BgError.fCall  = fTok;
  context->setup.BgError.data   = data;
  context->setup.BgError.fFree  = fFree;
  context->setup.BgError.fCopy  = fCopy;
}

enum MqErrorE
MqConfigSetIoUdsFile (
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
MqConfigSetIoPipeSocket (
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
      return MqErrorDbV (MQ_ERROR_OPTION_FORBIDDEN, "current", "--daemon");
    } else {
      MqErrorCheck (MqSysDaemonize(context, pidfile));
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

/*
int
MqConfigGetIsDupAndThread (
  struct MqS const * const context
)
{
  return ((context->statusIs & MQ_STATUS_IS_DUP) && (context->statusIs & MQ_STATUS_IS_THREAD));
}
*/

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
  return context->setup.factory ? context->setup.factory->ident : "";
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
  if (context->config.io.uds.file != NULL) {
    MqBufferGetC(context->config.io.uds.file, &s);
  }
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

enum MqStatusIsE
MqConfigGetStatusIs (
  struct MqS * const context
)
{
  return context->statusIs;
}

/*****************************************************************************/
/*                                                                           */
/*                                Factory                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE 
MqFactoryCtxIdent (
  struct MqS * const context,
  MQ_CST ident
) {
  enum MqFactoryReturnE ret;
  MqFactoryCheck (ret = MqFactoryItemGet(ident, &context->setup.factory));
  MqConfigUpdateName(context, ident);
  return MQ_OK;
error:
  return MqErrorC(context, __func__, -1, MqFactoryMsg(ret));
}

enum MqErrorE 
MqFactoryCtxNew (
  struct MqS * const context,
  MQ_CST	    ident,
  MqFactoryCreateF  fCreate,
  MQ_PTR	    CreateData,
  MqTokenDataFreeF  fCreateFree,
  MqFactoryDeleteF  fDelete,
  MQ_PTR	    DeleteData,
  MqTokenDataFreeF  fDeleteFree
) {
  enum MqFactoryReturnE ret;
  MqFactoryCheck(ret = MqFactoryAdd(ident, fCreate, CreateData, fCreateFree, fDelete, DeleteData, fDeleteFree));
  MqFactoryCtxIdent(context, ident);
  return MQ_OK;
error:
  return MqErrorC(context, __func__, -1, MqFactoryMsg(ret));
}

void 
MqFactoryCtxItem (
  struct MqS * const context,
  struct MqFactoryItemS * const item
) {
  context->setup.factory = item;
  MqConfigUpdateName(context, item->ident);
}

enum MqErrorE 
MqFactoryCtxDefault (
  struct MqS * const context,
  MQ_CST const ident
) {
  enum MqFactoryReturnE ret;
  MqFactoryCheck(ret = MqFactoryDefault(ident, sDefaultFactory, NULL, NULL, NULL, NULL, NULL));
  MqFactoryCtxIdent(context, ident);
error:
  return MqErrorC(context, __func__, -1, MqFactoryMsg(ret));
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

#define sSetupMark(cb) if (cb.data != NULL) (*markF)(cb.data);

void pSetupMark (
  struct MqS * const context,
  MqMarkF const markF
)
{
  sSetupMark (context->setup.Event);
  sSetupMark (context->setup.ServerSetup);
  sSetupMark (context->setup.ServerCleanup);
  sSetupMark (context->setup.BgError);
}

/*****************************************************************************/
/*                                                                           */
/*                                 Setup                                     */
/*                                                                           */
/*****************************************************************************/

void
ConfigCreate (void)
{
  MqFactoryDefault("libmsgque", sDefaultFactory, NULL, NULL, NULL, NULL, NULL);
}

END_C_DECLS

