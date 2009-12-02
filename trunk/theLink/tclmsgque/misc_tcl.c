/**
 *  \file       tclmsgque/misc_tcl.c
 *  \brief      \$Id: misc_tcl.c 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"
#include <math.h>

#define MQ_CONTEXT_S mqctx

int NS(GetClientData) (
  Tcl_Interp * interp,
  Tcl_Obj * obj,
  MQ_PTR *out
)
{
  Tcl_CmdInfo infoPtr;
  Tcl_Command command;

  *out = NULL;

  command = Tcl_GetCommandFromObj (interp, obj);
  if (command == NULL)
    return TCL_ERROR;
  Tcl_GetCommandInfoFromToken (command, &infoPtr);
  *out = infoPtr.objClientData;
  return TCL_OK;
}

int NS(pErrorFromMq) (
  Tcl_Interp * interp,
  struct MqS * const mqctx
)
{
  Tcl_Obj *objv[4];
  objv[0] = Tcl_NewStringObj ("TCLMSGQUE", -1);
  objv[1] = Tcl_NewIntObj (MqErrorGetNum(mqctx));
  objv[2] = Tcl_NewIntObj (MqErrorGetCode(mqctx));
  objv[3] = Tcl_NewStringObj (MqErrorGetText(mqctx), -1);
  Tcl_SetObjErrorCode (interp, Tcl_NewListObj (4, objv));
  Tcl_SetResult(interp, (MQ_STR) MqErrorGetText(mqctx), TCL_VOLATILE);
  MqErrorReset(mqctx);
  return TCL_ERROR;
}

int NS(ErrorStringToTcl)  (
  Tcl_Interp  *interp, 
  MQ_CST      message
)
{
  Tcl_SetResult (interp, (char*)message, TCL_VOLATILE);
  return TCL_ERROR;
}

enum MqErrorE NS(ProcError) (
  struct TclContextS * const tclctx,
  MQ_CST proc
)
{
  SETUP_interp
  enum MqErrorE ret = MQ_OK;
  Tcl_Obj *item;
  Tcl_Obj *errorCode = Tcl_GetVar2Ex (interp, "errorCode", NULL, TCL_GLOBAL_ONLY);
  if (
    Tcl_ListObjIndex (NULL, errorCode, 0, &item) == TCL_ERROR  ||   // index "0" is not in the list "code"
    strncmp (Tcl_GetString (item), "TCLMSGQUE", 9)		    // error is not from "TCLMSGQUE"
  ) {
    // tcl error
    ret = MqErrorC (MQCTX,proc,-1,Tcl_GetVar (interp, "errorInfo", TCL_GLOBAL_ONLY));
  } else {
    // tclmsgque error
    int errnum = -1;
    int errcode = -1;
    Tcl_ListObjIndex (NULL, errorCode, 1, &item);
    Tcl_GetIntFromObj(NULL, item, &errnum); 
    Tcl_ListObjIndex (NULL, errorCode, 2, &item);
    Tcl_GetIntFromObj(NULL, item, &errcode); 
    Tcl_ListObjIndex (NULL, errorCode, 3, &item);
    ret = MqErrorSet (MQCTX, errnum, (enum MqErrorE) errcode, Tcl_GetString(item));
  }
  Tcl_ResetResult(interp);
  return ret;
}

enum MqErrorE NS(ProcCall) (
  struct MqS * const mqctx,
  MQ_PTR const dataP
)
{
  SETUP_tclctx
  SETUP_interp
  Tcl_Obj *lobjv[2];
  int ret;
  Tcl_Obj *proc = (Tcl_Obj*) dataP;

  // 0. clean all old errors
  //Tcl_ResetResult(interp);

  // the "BqError" have to survive the following line -> skip it
  //MqErrorReset(mqctx);

  // 1. add service handler
  lobjv[0] = proc;

  // 2. setup Command (e.g. the ContextS)
  lobjv[1] = ((Tcl_Obj*)tclctx->mqctx.self);

  // 3. evaluate the script
  Tcl_IncrRefCount(lobjv[0]);
  Tcl_IncrRefCount(lobjv[1]);
  ret = Tcl_EvalObjv (interp, 2, lobjv, TCL_EVAL_GLOBAL);
  Tcl_DecrRefCount(lobjv[1]);
  Tcl_DecrRefCount(lobjv[0]);

  return ret == TCL_OK ? Tcl_ResetResult(interp),MQ_OK : NS(ProcError) (tclctx, "ErrorSet");
}

void NS(ProcFree) (
  struct MqS const * const msgque,
  MQ_PTR *dataP
)
{
  Tcl_DecrRefCount ( (Tcl_Obj*) *dataP);
  *dataP = NULL;
}

enum MqErrorE NS(ProcCopy) (
  struct MqS * const msgque,
  MQ_PTR *dataP
)
{
  Tcl_IncrRefCount ( (Tcl_Obj*) *dataP);
  return MQ_OK;
}

int NS(ProcCheck) (
  Tcl_Interp * interp,
  struct Tcl_Obj * cmdObj,
  char const * const wrongNrStr
)
{
  int ret,len;
  Tcl_DString cmd;
  if (!Tcl_GetCommandFromObj (interp, cmdObj)) {
    Tcl_WrongNumArgs (interp, 0, NULL, wrongNrStr);
    return TCL_ERROR;
  }
  Tcl_DStringInit(&cmd);
  Tcl_DStringAppendElement(&cmd,"info");
  Tcl_DStringAppendElement(&cmd,"args");
  Tcl_DStringAppendElement(&cmd,Tcl_GetString(cmdObj));
  ret = Tcl_EvalEx(interp, Tcl_DStringValue(&cmd), Tcl_DStringLength(&cmd), TCL_EVAL_GLOBAL);
  Tcl_DStringFree(&cmd);
  TclErrorCheck(ret);
  TclErrorCheck(Tcl_ListObjLength(interp, Tcl_GetObjResult(interp), &len));
  if (len != 1) {
    Tcl_DString msg;
    Tcl_DStringInit(&msg);
    Tcl_DStringAppend(&msg,"wrong # args: ", -1);
    if (len > 1) Tcl_DStringAppend(&msg,"only ", -1);
    Tcl_DStringAppend(&msg,"one argument for procedure \"", -1);
    Tcl_DStringAppend(&msg,Tcl_GetString(cmdObj), -1);
    Tcl_DStringAppend(&msg,"\" is required", -1);
    Tcl_DStringResult(interp, &msg);
    Tcl_DStringFree(&msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}

