/**
 *  \file       theLink/tclmsgque/config_tcl.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"

#define MQ_CONTEXT_S MQCTX

enum MqErrorE NS(EventLink) (
  struct MqS * const mqctx,
  MQ_PTR const data
)
{
  Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT);
  return data == NULL ? MQ_OK : NS(ProcCall) (mqctx, data);
}

static enum MqErrorE NS(FactoryCreate) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR data,
  struct MqS ** contextP
) {
  struct MqBufferS * const buf = tmpl->temp;
  struct MqS * mqctx = *contextP = MqContextCreate (sizeof(struct TclContextS), tmpl);
  SETUP_tclctx
  Tcl_Interp * interp;

  if (create == MQ_FACTORY_NEW_THREAD) {
    interp = Tcl_CreateInterp();
    TclErrorToMq (Tcl_Init(interp))
    Tcl_SetVar (interp, "MQ_STARTUP_IS_THREAD", "yes", TCL_GLOBAL_ONLY);
    TclErrorToMq (Tcl_EvalFile(interp, MqInitBuf->data[1]->cur.C))
  } else {
    interp = (Tcl_Interp *const) tmpl->threadData;
  }
  tclctx->mqctx.threadData = (MQ_PTR)interp;

  // initialize
  MqBufferSetV(buf, "<MqS-%p>", tclctx);
  tclctx->command = Tcl_CreateObjCommand (interp, buf->cur.C, NS(MqS_Cmd), tclctx, NS(MqS_Free));
  tclctx->mqctx.self = (void*)Tcl_NewStringObj(buf->cur.C,-1);
    Tcl_IncrRefCount((Tcl_Obj*)tclctx->mqctx.self);
  tclctx->dict = NULL;

  // copy setup data and initialize data entries
  MqErrorCheck (MqSetupDup(mqctx, tmpl));

  // child does not need an event-handler if not user supplied
  if (create == MQ_FACTORY_NEW_CHILD && mqctx->setup.Event.data == NULL) {
    mqctx->setup.Event.fFunc = NULL;
  }
  
  return MQ_OK;

error:
  MqErrorCopy (tmpl, mqctx);
  MqContextDelete (&mqctx);
  return MqErrorStack(tmpl);
}

void NS(FactoryDelete) (
  struct MqS * ctx,
  MQ_BOL doFactoryCleanup,
  MQ_PTR data
) {
  struct TclContextS * tclctx = (struct TclContextS *) ctx;
  enum MqStatusIsE statusIs = tclctx->mqctx.statusIs;
  SETUP_interp;
  Tcl_DeleteCommandFromToken (interp, tclctx->command);
  // the "thread" have to delete the interpreter
  if (statusIs & MQ_STATUS_IS_THREAD) {
    Tcl_DeleteInterp(interp);
  }
}

int NS(ConfigReset) (NS_ARGS)
{
  CHECK_NOARGS
  MqConfigReset (MQCTX);
  RETURN_TCL
}

int NS(ConfigSetBuffersize) (NS_ARGS)
{
  MQ_INT buffersize;
  CHECK_I(buffersize)
  CHECK_NOARGS
  MqConfigSetBuffersize (MQCTX, buffersize);
  RETURN_TCL
}

int NS(ConfigSetDebug) (NS_ARGS)
{
  MQ_INT debug;
  CHECK_I(debug)
  CHECK_NOARGS
  MqConfigSetDebug (MQCTX, debug);
  RETURN_TCL
}

int NS(ConfigSetTimeout) (NS_ARGS)
{
  MQ_WID timeout;
  CHECK_W(timeout)
  CHECK_NOARGS
  MqConfigSetTimeout (MQCTX, timeout);
  RETURN_TCL
}

int NS(ConfigSetName) (NS_ARGS)
{
  MQ_CST name;
  CHECK_C(name)
  CHECK_NOARGS
  MqConfigSetName (MQCTX, name);
  RETURN_TCL
}

int NS(ConfigSetSrvName) (NS_ARGS)
{
  MQ_CST srvname;
  CHECK_C(srvname)
  CHECK_NOARGS
  MqConfigSetSrvName (MQCTX, srvname);
  RETURN_TCL
}

int NS(ConfigSetIdent) (NS_ARGS)
{
  MQ_CST ident;
  CHECK_C(ident)
  CHECK_NOARGS
  MqConfigSetIdent (MQCTX, ident);
  RETURN_TCL
}

int NS(ConfigCheckIdent) (NS_ARGS)
{
  MQ_CST ident;
  CHECK_C(ident)
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj(MqConfigCheckIdent (MQCTX, ident)));
  RETURN_TCL
}

int NS(ConfigSetIsSilent) (NS_ARGS)
{
  MQ_BOL isSilent;
  CHECK_O(isSilent)
  CHECK_NOARGS
  MqConfigSetIsSilent (MQCTX, isSilent);
  RETURN_TCL
}

int NS(ConfigSetIsServer) (NS_ARGS)
{
  MQ_BOL isSilent;
  CHECK_O(isSilent)
  CHECK_NOARGS
  MqConfigSetIsServer (MQCTX, isSilent);
  RETURN_TCL
}

int NS(ConfigSetIsString) (NS_ARGS)
{
  MQ_BOL isString;
  CHECK_O(isString)
  CHECK_NOARGS
  MqConfigSetIsString (MQCTX, isString);
  RETURN_TCL
}

int NS(ConfigSetIgnoreExit) (NS_ARGS)
{
  MQ_BOL ignoreExit;
  CHECK_O(ignoreExit)
  CHECK_NOARGS
  MqConfigSetIgnoreExit (MQCTX, ignoreExit);
  RETURN_TCL
}

int NS(ConfigSetServerSetup) (NS_ARGS)
{
  Tcl_Obj *srvSetup;
  CHECK_PROC(srvSetup, "ConfigSetServerSetup setup-proc")
  CHECK_NOARGS
  Tcl_IncrRefCount(srvSetup);
  MqConfigSetServerSetup (MQCTX, NS(ProcCall), srvSetup, NS(ProcFree), NS(ProcCopy));
  RETURN_TCL
}

int NS(ConfigSetEvent) (NS_ARGS)
{
  Tcl_Obj *event;
  CHECK_PROC(event, "ConfigSetEvent event-proc")
  CHECK_NOARGS
  Tcl_IncrRefCount(event);
  MqConfigSetEvent (MQCTX, NS(EventLink), event, NS(ProcFree), NS(ProcCopy));
  RETURN_TCL
}

int NS(ConfigSetServerCleanup) (NS_ARGS)
{
  Tcl_Obj *srvCleanup;
  CHECK_PROC(srvCleanup, "ConfigSetServerCleanup cleanup-proc")
  CHECK_NOARGS
  Tcl_IncrRefCount(srvCleanup);
  MqConfigSetServerCleanup (MQCTX, NS(ProcCall), srvCleanup, NS(ProcFree), NS(ProcCopy));
  RETURN_TCL
}

int NS(ConfigSetBgError) (NS_ARGS)
{
  Tcl_Obj *bgerror;
  CHECK_PROC(bgerror, "ConfigSetBgError bgerror-proc")
  CHECK_NOARGS
  Tcl_IncrRefCount(bgerror);
  MqConfigSetBgError (MQCTX, NS(ProcCall), bgerror, NS(ProcFree), NS(ProcCopy));
  RETURN_TCL
}

int NS(ConfigSetFactory) (NS_ARGS)
{
  CHECK_NOARGS
  MqConfigSetFactory(MQCTX, 
    NS(FactoryCreate), NULL, NULL, NULL,
    NS(FactoryDelete), NULL, NULL, NULL
  );
  RETURN_TCL
}

int NS(ConfigSetIoUds) (NS_ARGS)
{
  SETUP_mqctx
  MQ_CST file;
  CHECK_C(file)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqConfigSetIoUds (mqctx, file));
  RETURN_TCL
}

int NS(ConfigSetIoTcp) (NS_ARGS)
{
  SETUP_mqctx
  MQ_CST host, port, myhost, myport;
  CHECK_C(host)
  CHECK_C(port)
  CHECK_C(myhost)
  CHECK_C(myport)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqConfigSetIoTcp (mqctx, host, port, myhost, myport));
  RETURN_TCL
}

int NS(ConfigSetIoPipe) (NS_ARGS)
{
  SETUP_mqctx
  MQ_SOCK socket;
  CHECK_I(socket)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqConfigSetIoPipe (mqctx, socket));
  RETURN_TCL
}

int NS(ConfigSetStartAs) (NS_ARGS)
{
  int startAs;
  CHECK_I(startAs)
  CHECK_NOARGS
  MqConfigSetStartAs (MQCTX, startAs);
  RETURN_TCL
}

int NS(ConfigSetDaemon) (NS_ARGS)
{
  SETUP_mqctx
  MQ_CST pidfile;
  CHECK_C(pidfile)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqConfigSetDaemon (mqctx, pidfile));
  RETURN_TCL
}

int NS(ConfigGetDebug) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewIntObj (tclctx->mqctx.config.debug));
  RETURN_TCL
}

int NS(ConfigGetBuffersize) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewIntObj (tclctx->mqctx.config.io.buffersize));
  RETURN_TCL
}

int NS(ConfigGetTimeout) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewIntObj ((int)tclctx->mqctx.config.io.timeout));
  RETURN_TCL
}

int NS(ConfigGetIsString) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (tclctx->mqctx.config.isString));
  RETURN_TCL
}

int NS(ConfigGetIsSilent) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (tclctx->mqctx.config.isSilent));
  RETURN_TCL
}

int NS(ConfigGetIsServer) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (tclctx->mqctx.setup.isServer));
  RETURN_TCL
}

int NS(ConfigGetIsSlave) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (tclctx->mqctx.config.master != NULL));
  RETURN_TCL
}

int NS(ConfigGetName) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, tclctx->mqctx.config.name, TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetSrvName) (NS_ARGS)
{
  MQ_CST str;
  CHECK_NOARGS
  str = MqConfigGetSrvName(&tclctx->mqctx);
  Tcl_SetResult(interp, (MQ_STR) (str ? str : ""), TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetIdent) (NS_ARGS)
{
  MQ_CST str;
  CHECK_NOARGS
  str = MqConfigGetIdent(&tclctx->mqctx);
  Tcl_SetResult(interp, (MQ_STR) (str ? str : ""), TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetMaster) (NS_ARGS)
{
  CHECK_NOARGS
  if (tclctx->mqctx.config.master != NULL)
    Tcl_SetObjResult(interp, (Tcl_Obj*)tclctx->mqctx.config.master->self);
  else
    Tcl_SetResult(interp, "", TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetIoUdsFile) (NS_ARGS)
{
  MQ_STR file = (MQ_STR)MqConfigGetIoUdsFile(&tclctx->mqctx);
  CHECK_NOARGS
  Tcl_SetResult(interp, file?file:"", TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetIoTcpHost) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR)MqConfigGetIoTcpHost(&tclctx->mqctx), TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetIoTcpPort) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR)MqConfigGetIoTcpPort(&tclctx->mqctx), TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetIoTcpMyHost) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR)MqConfigGetIoTcpMyHost(&tclctx->mqctx), TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetIoTcpMyPort) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR)MqConfigGetIoTcpMyPort(&tclctx->mqctx), TCL_STATIC);
  RETURN_TCL
}

int NS(ConfigGetIoPipeSocket) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewIntObj (MqConfigGetIoPipeSocket(&tclctx->mqctx)));
  RETURN_TCL
}

int NS(ConfigGetStartAs) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewIntObj (MqConfigGetStartAs(&tclctx->mqctx)));
  RETURN_TCL
}

