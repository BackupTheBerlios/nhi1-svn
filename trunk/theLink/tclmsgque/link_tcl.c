/**
 *  \file       theLink/tclmsgque/link_tcl.c
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

int NS(LinkGetCtxId) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewIntObj (MqLinkGetCtxIdI(MQCTX)));
  RETURN_TCL
}

int NS(LinkIsParent) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (MqLinkIsParentI(MQCTX)));
  RETURN_TCL
}

int NS(LinkIsConnected) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (MqLinkIsConnected(MQCTX)));
  RETURN_TCL
}

int NS(LinkGetParent) (NS_ARGS)
{
  struct MqS * const parent = MqLinkGetParentI(MQCTX);
  CHECK_NOARGS
  if (parent != NULL) {
    Tcl_SetObjResult(interp,(Tcl_Obj*)parent->self);
  } else {
    Tcl_SetResult(interp,"", TCL_STATIC);
  }
  RETURN_TCL
}

int NS(LinkGetTargetIdent) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewStringObj(MqLinkGetTargetIdent (MQCTX), -1));
  RETURN_TCL
}

int NS(LinkDelete) (NS_ARGS)
{
  CHECK_NOARGS
  MqLinkDelete (MQCTX);
  RETURN_TCL
}

int NS(LinkConnect) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqLinkConnect (mqctx));
  RETURN_TCL
}

int NS(LinkCreate) (NS_ARGS)
{
  SETUP_mqctx
  struct MqBufferLS * args = NULL;

  // command-line arguments to MqBufferLS
  if (objc-skip > 0) {
    int i;
    args = MqBufferLCreate (objc-skip+1);
    if (Tcl_GetString(objv[skip])[0] == '-')
      MqBufferLAppendC (args, (const MQ_STR) Tcl_GetNameOfExecutable());
    for (i = skip; i < objc; i++) {
      MqBufferLAppendC (args, Tcl_GetString (objv[i]));
    }
  }

  // create Context
  ErrorMqToTclWithCheck (MqLinkCreate(mqctx, &args));
  RETURN_TCL
}

int NS(LinkCreateChild) (NS_ARGS)
{
  // check for connected
  struct MqS * parent;

  // get the parent tclctx
  if (objc < 3 || NS(GetClientData) (interp, objv[skip], (MQ_PTR*) &parent) == TCL_ERROR) {
    Tcl_WrongNumArgs (interp, 2, objv, "parent ...");
    return TCL_ERROR;
  } else {
    SETUP_mqctx
    struct MqBufferLS *args = NULL;
    int i;

    skip++;

    // copy data entries
    ErrorMqToTclWithCheck (MqSetupDup (mqctx, parent));

    // command-line arguments to MqBufferLS
    if (objc-skip > 0) {
      args = MqBufferLCreate (objc-skip+1);
      if (Tcl_GetString(objv[skip])[0] == '-')
	MqBufferLAppendC (args, (const MQ_STR) Tcl_GetNameOfExecutable());
      for (i = skip; i < objc; i++) {
	MqBufferLAppendC (args, Tcl_GetString (objv[i]));
      }
    }

    // create Context
    ErrorMqToTclWithCheck (MqLinkCreateChild(mqctx, parent, &args));
    RETURN_TCL
  }
}
