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
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

static void
pConfigInit (
  struct MqS * const context
)
{
  context->config.io.buffersize = 4096;
  context->config.io.timeout = MQ_TIMEOUT10;
  context->config.io.com = MQ_IO_PIPE;
  context->config.io.uds.file = NULL;
  context->config.io.tcp.host = NULL;
  context->config.io.tcp.port = NULL;
  context->config.io.tcp.myhost = NULL;
  context->config.io.tcp.myport = NULL;
  context->config.io.pipe.socks[0] = -1;
  context->config.io.pipe.socks[1] = -1;
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
  pErrorSetup(context);
}

void
MqContextFree (
  struct MqS * const context
)
{
  if (context == NULL || context->MqContextFree_LOCK == MQ_YES) {
    return;
  } else {
    context->MqContextFree_LOCK = MQ_YES;
    MqLinkDelete(context);

    // cleanup setup data entries
    if (context->setup.FilterFTR.data && context->setup.FilterFTR.fFree) {
      (*context->setup.FilterFTR.fFree) (context, &context->setup.FilterFTR.data);
    }
    if (context->setup.FilterEOF.data && context->setup.FilterEOF.fFree) {
      (*context->setup.FilterEOF.fFree) (context, &context->setup.FilterEOF.data);
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
  }
}

void
MqContextDelete (
  struct MqS ** contextP
) {
  struct MqS * context = *contextP;
  *contextP = NULL;
  if (context == NULL || context->MqContextDelete_LOCK == MQ_YES) {
    return;
  } else if (context->setup.Factory.Delete.fCall) {
    MqFactoryDeleteF fCall = context->setup.Factory.Delete.fCall;
    MQ_BOL doFactoryCleanup = context->link.doFactoryCleanup;
    context->link.doFactoryCleanup = MQ_NO;
    context->setup.Factory.Delete.fCall = NULL;
    (*fCall) (context, doFactoryCleanup, context->setup.Factory.Delete.data);
  } else {
    // set this because "setup.Factory.Delete.fCall" is !not! required
    context->link.doFactoryCleanup = MQ_NO;
    context->MqContextDelete_LOCK = MQ_YES;
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
  context->statusIs = MQ_STATUS_IS_INITIAL;
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
  MQ_PTR	      *dataToSet,
  MqTokenDataCopyF    copyData,
  MQ_PTR	      oldData
)
{
  if (oldData) {
    *dataToSet = oldData;
  } else if (*dataToSet != NULL && copyData != NULL) {
    return (*copyData) (context, dataToSet);
  }
  return MQ_OK;
}

enum MqErrorE
MqSetupDup (
  struct MqS * const context,
  struct MqS const * const from
)
{
  // save all "data" entries because these are allready set by the 
  // Class-Constructor proper
  MQ_PTR ServerSetup = context->setup.ServerSetup.data;
  MQ_PTR ServerCleanup = context->setup.ServerCleanup.data;
  MQ_PTR BgError = context->setup.BgError.data;
  MQ_PTR FilterFTR = context->setup.FilterFTR.data;
  MQ_PTR FilterEOF = context->setup.FilterEOF.data;
  MQ_PTR FactoryC = context->setup.Factory.Create.data;
  MQ_PTR FactoryD = context->setup.Factory.Delete.data;

  // copy "setup" 
  context->setup = from->setup;
  context->setup.ident = mq_strdup_save(from->setup.ident);

  // reinitialize "data" entries which were !not! set by the class constructor
  MqErrorCheck (sSetupDupHelper (context, &context->setup.ServerSetup.data, context->setup.ServerSetup.fCopy, ServerSetup));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.ServerCleanup.data, context->setup.ServerCleanup.fCopy, ServerCleanup));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.BgError.data, context->setup.BgError.fCopy, BgError));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.FilterFTR.data, context->setup.FilterFTR.fCopy, FilterFTR));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.FilterEOF.data, context->setup.FilterEOF.fCopy, FilterEOF));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.Factory.Create.data, context->setup.Factory.Create.fCopy, FactoryC));
  MqErrorCheck (sSetupDupHelper (context, &context->setup.Factory.Delete.data, context->setup.Factory.Delete.fCopy, FactoryD));

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
  return MqErrorGetCode (tmpl);
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
  (*contextP)->link.doFactoryCleanup = MQ_YES;

  return MQ_OK;

error:
  if (*contextP != NULL) {
    MqErrorCopy(tmpl, *contextP);
    MqContextDelete(contextP);
  }
  return MqErrorGetCode (tmpl);
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
MqConfigSetIsString (
  struct MqS * const context,
  MQ_BOL data
) {
  context->config.isString = data;
}

void 
MqConfigSetParent (
  struct MqS * const context,
  struct MqS * const parent
) {
  context->config.parent = parent;
}

void 
MqConfigSetMaster (
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
//MqDLogX(context,__func__,0,"data<%p>\n", data);
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
//MqDLogX(context,__func__,0,"%p-%p-%p-%p-%p-%p\n", fChildCreate, fChildDelete, fParentCreate, fParentDelete, fProcessExit, fThreadExit);

  context->setup.Child.fCreate = fChildCreate;
  context->setup.Child.fDelete = fChildDelete;
  context->setup.Parent.fCreate = fParentCreate;
  context->setup.Parent.fDelete = fParentDelete;
  context->setup.fProcessExit = fProcessExit;
  context->setup.fThreadExit = fThreadExit;
}

void
MqConfigSetFilterFTR (
  struct MqS * const context,
  MqTokenF fFunc,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
)
{
//MqDLogX(context,__func__,0,"data<%p>\n", data);
  if (context->setup.FilterFTR.data && context->setup.FilterFTR.fFree) {
    (*context->setup.FilterFTR.fFree) (context, &context->setup.FilterFTR.data);
  }
  context->setup.isServer = MQ_YES;
  context->setup.FilterFTR.fFunc = fFunc;
  context->setup.FilterFTR.data  = data;
  context->setup.FilterFTR.fFree = fFree;
  context->setup.FilterFTR.fCopy = fCopy;
}

void
MqConfigSetFilterEOF (
  struct MqS * const context,
  MqTokenF fFunc,
  MQ_PTR data,
  MqTokenDataFreeF fFree,
  MqTokenDataCopyF fCopy
)
{
//MqDLogX(context,__func__,0,"data<%p>\n", data);
  if (context->setup.FilterEOF.data && context->setup.FilterEOF.fFree) {
    (*context->setup.FilterEOF.fFree) (context, &context->setup.FilterEOF.data);
  }
  context->setup.isServer        = MQ_YES;
  context->setup.FilterEOF.fFunc = fFunc;
  context->setup.FilterEOF.data  = data;
  context->setup.FilterEOF.fFree = fFree;
  context->setup.FilterEOF.fCopy = fCopy;
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
//MqDLogX(context,__func__,0,"data<%p>\n", data);
  if (context->setup.ServerSetup.data && context->setup.ServerSetup.fFree) {
    (*context->setup.ServerSetup.fFree) (context, &context->setup.ServerSetup.data);
  }
  context->setup.isServer	   = MQ_YES;
  context->setup.ServerSetup.fFunc = fTok;
  context->setup.ServerSetup.data  = data;
  context->setup.ServerSetup.fFree = fFree;
  context->setup.ServerSetup.fCopy = fCopy;
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
//MqDLogX(context,__func__,0,"data<%p>\n", data);
  if (context->setup.ServerCleanup.data && context->setup.ServerCleanup.fFree) {
    (*context->setup.ServerCleanup.fFree) (context, &context->setup.ServerCleanup.data);
  }
  context->setup.isServer	      = MQ_YES;
  context->setup.ServerCleanup.fFunc  = fTok;
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
  context->config.io.pipe.socks[1] = socket;
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
MqConfigGetIsParent (
  struct MqS const * const context
)
{
  return MQ_IS_PARENT(context);
}

int
MqConfigGetIsSlave (
  struct MqS const * const context
)
{
  return (context->config.master != NULL);
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
MqConfigGetIsConnected (
  struct MqS const * const context
)
{
  return (context->link.onCreate == MQ_YES);
}

/*
int
MqConfigGetDoFactoryCleanup (
  struct MqS const * const context
)
{
  return (context->link.doFactoryCleanup == MQ_YES);
}
*/

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

MQ_BOL 
MqConfigCheckIdent (
  struct MqS * const context,
  MQ_CST ident
)
{
  MQ_CST ftrIdent;
  if (context == NULL) return MQ_NO;
  MqErrorCheck (MqSendSTART (context));
  MqErrorCheck (MqSendEND_AND_WAIT (context, "_IDN", MQ_TIMEOUT_USER));
  MqErrorCheck (MqReadC (context, &ftrIdent));
  if (strcmp (ident, ftrIdent)) return MQ_NO;
  return MQ_YES;
error:
  MqErrorReset(context);
  return MQ_NO;
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

struct MqS *
MqConfigGetParent (
  struct MqS const * const context
)
{
  return context->config.parent;
}

struct MqS *
MqConfigGetMaster (
  struct MqS const * const context
)
{
  return context->config.master;
}

MQ_SIZE
MqConfigGetCtxId (
  struct MqS const * const context
)
{
  return context->link.ctxId;
}

MQ_CST 
MqConfigGetToken (
  struct MqS const * const context
)
{
  return (context->link.srvT != NULL? pTokenGetCurrent(context->link.srvT) : NULL);
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
  return context->config.io.pipe.socks[1];
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


