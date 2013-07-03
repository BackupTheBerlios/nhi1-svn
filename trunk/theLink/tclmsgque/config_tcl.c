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

enum MqErrorE NS(FactoryCreate) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS * const item,
  struct MqS ** contextP
) {
  struct MqS * mqctx = NULL;

  Tcl_Interp * interp;

  if (create == MQ_FACTORY_NEW_THREAD) {
    interp = Tcl_CreateInterp();
    TclErrorToCtxWithReturn (tmpl, Tcl_Init(interp))
    Tcl_SetVar (interp, "MQ_STARTUP_IS_THREAD", "yes", TCL_GLOBAL_ONLY);
    TclErrorToCtxWithReturn (tmpl, Tcl_EvalFile(interp, MqInitGetArg0()->data[1]->cur.C))
  } else if (create == MQ_FACTORY_NEW_INIT) {
    interp = (Tcl_Interp *const) tmpl;
  } else {
    interp = (Tcl_Interp *const) tmpl->threadData;
  }

  {
    // Call Factory
    Tcl_Obj *lobjv[3];
    int ret,num=0,i;

    // 1. add service handler
    if (item->Create.data) {
      lobjv[num++] = (Tcl_Obj*) item->Create.data;
    } else {
      lobjv[num++] = Tcl_NewStringObj("tclmsgque",-1);
      lobjv[num++] = Tcl_NewStringObj("MqS",-1);
    }
    if (create == MQ_FACTORY_NEW_INIT || create == MQ_FACTORY_NEW_THREAD) {
      lobjv[num++] = Tcl_NewObj();
    } else {
      lobjv[num++] = (Tcl_Obj*) tmpl->self;
    }
    
    // 2. add refCount
    for (i=0;i<num;i++) Tcl_IncrRefCount(lobjv[i]);

    // 3. evaluate the script
    ret = Tcl_EvalObjv (interp, num, lobjv, TCL_EVAL_GLOBAL);

    // 4. cleanup
    for (i=0;i<num;i++) Tcl_DecrRefCount(lobjv[i]);
    TclErrorCheckG1(ret);

    // 5. get context from the Factory return value
    TclErrorCheckG2(NS(GetClientData) (interp, Tcl_GetObjResult(interp), MQ_MqS_SIGNATURE, (MQ_PTR*) &mqctx));
  }

  // check for MQ error
  MqErrorCheck(MqErrorGetCode(mqctx));

  // copy and initialize "setup" data
  if (create != MQ_FACTORY_NEW_INIT) {
    MqSetupDup(mqctx, tmpl);
  }

  // tcl-special: default event handler has !no! data and calling just the
  // tcl-event-update function. CHILD does not need to call it too
  if (create == MQ_FACTORY_NEW_CHILD && mqctx->setup.Event.data == NULL) {
    mqctx->setup.Event.fCall = NULL;
  }
  
  *contextP = mqctx;
  return MQ_OK;

error:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    MqErrorCopy (tmpl, mqctx);
    MqContextDelete (&mqctx);
    return MqErrorStack(tmpl);
  } else {
    return MQ_ERROR;
  }

error1:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    NS(ProcError) ((struct TclContextS * const)tmpl, "FactoryCreate");
    return MqErrorGetCode(tmpl);
  } else {
    return MQ_ERROR;
  }

error2:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    return MqErrorC(tmpl, __func__, -1, "Factory return no MqS type");
  } else {
    return MQ_ERROR;
  }
}

void NS(FactoryDelete) (
  struct MqS * ctx,
  MQ_BOL doFactoryCleanup,
  struct MqFactoryS *item
) {
  struct TclContextS * tclctx = (struct TclContextS *) ctx;
  enum MqStatusIsE statusIs = tclctx->mqctx.statusIs;
  MQ_INT refCount = tclctx->mqctx.refCount;
  SETUP_interp;
  Tcl_DeleteCommandFromToken (interp, tclctx->command);
  // the "thread" have to delete the interpreter
  if (refCount == 0 && statusIs & MQ_STATUS_IS_THREAD) {
    Tcl_DeleteInterp(interp);
  }
  // cleanup data
  if (item->Create.data) {
    Tcl_DecrRefCount((Tcl_Obj*)item->Create.data);
    item->Create.data = NULL;
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

int NS(ConfigSetStorage) (NS_ARGS)
{
  MQ_CST storage;
  CHECK_C(storage)
  CHECK_NOARGS
  MqConfigSetStorage (MQCTX, storage);
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

int NS(ConfigSetIoUdsFile) (NS_ARGS)
{
  SETUP_mqctx
  MQ_CST file;
  CHECK_C(file)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqConfigSetIoUdsFile (mqctx, file));
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

int NS(ConfigSetIoPipeSocket) (NS_ARGS)
{
  SETUP_mqctx
  MQ_SOCK socket;
  CHECK_I(socket)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqConfigSetIoPipeSocket (mqctx, socket));
  RETURN_TCL
}

int NS(ConfigSetStartAs) (NS_ARGS)
{
  int startAs;
  static const char *optA[] = { "DEFAULT", "FORK", "THREAD", "SPAWN", NULL };
  enum optAE { DEFAULT, FORK, THREAD, SPAWN };
  if (objc-skip<1) {
    Tcl_SetResult(interp, "no additional argument available for 'startAs'", TCL_STATIC);
    return TCL_ERROR;
  }
  if (Tcl_GetIntFromObj (interp, objv[skip], &startAs) != TCL_OK) {
    Tcl_ResetResult(interp);
    TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[skip], optA, "startAs", 0, &startAs));
  }
  objv++;objc--;
  CHECK_NOARGS;
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

int NS(ConfigGetName) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR) tclctx->mqctx.config.name, TCL_STATIC);
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

int NS(ConfigGetStorage) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR) MqConfigGetStorage(&tclctx->mqctx), TCL_STATIC);
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

int NS(ConfigGetStatusIs) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewIntObj (MqConfigGetStatusIs(&tclctx->mqctx)));
  RETURN_TCL
}



