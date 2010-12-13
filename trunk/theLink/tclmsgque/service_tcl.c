/**
 *  \file       theLink/tclmsgque/service_tcl.c
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

int NS(ServiceGetFilter) (NS_ARGS)
{
  SETUP_mqctx
  struct MqS * ftr;
  MQ_SIZE id=0;
  CHECK_DI(id);
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqServiceGetFilter(mqctx, id, &ftr));
  if (ftr != NULL) {
    Tcl_SetObjResult(interp,(Tcl_Obj*)ftr->self);
  } else {
    Tcl_SetResult(interp,"", TCL_STATIC);
  }
  RETURN_TCL
}

int NS(ServiceGetToken) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (char *)MqServiceGetToken(MQCTX), TCL_VOLATILE);
  RETURN_TCL
}

int NS(ServiceIsTransaction) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (MqServiceIsTransaction(MQCTX)));
  RETURN_TCL
}

int NS(ServiceCreate) (NS_ARGS)
{
  SETUP_mqctx
  MQ_STR token;
  Tcl_Obj * service = NULL;
  CHECK_C(token)
  CHECK_OPTIONAL_PROC(service, "CONTEXT ServiceCreate token tclproc")
  CHECK_NOARGS
  Tcl_IncrRefCount (service);
  if (service == NULL) {
    ErrorMqToTclWithCheck (MqServiceCreate (mqctx, token, NULL, NULL, NULL));
  } else {
    ErrorMqToTclWithCheck (MqServiceCreate (mqctx, token, NS(ProcCall), service, NS(ProcFree)));
  }
  RETURN_TCL
}

int NS(ServiceDelete) (NS_ARGS)
{
  SETUP_mqctx
  MQ_STR token;
  CHECK_C(token)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqServiceDelete (mqctx, token));
  RETURN_TCL
}

int NS(ServiceProxy) (NS_ARGS)
{
  SETUP_mqctx
  MQ_STR token;
  MQ_SIZE id=0;
  CHECK_C(token)
  CHECK_DI(id)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqServiceProxy (mqctx, token, id));
  RETURN_TCL
}

int NS(ProcessEvent) (NS_ARGS)
{
  SETUP_mqctx
  int timeout = MQ_TIMEOUT_DEFAULT;
  int wait = MQ_WAIT_NO;
  int iA, iC;
  static const char *optA[] = { "-timeout", "-wait", NULL };
  static const char *optB[] = { "NO", "ONCE", "FOREVER", NULL };
  static const char *optC[] = { "DEFAULT", "USER", "MAX", NULL };
  enum optAE { TIMEOUT, WAIT };
  enum optCE { DEFAULT, USER, MAX };

  // look for options
  objc -= skip;
  objv += skip;
  while (objc) {
    TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[0], optA, "option", 0, &iA));
    switch ((enum optAE) iA) {
      case WAIT:
	CheckForAdditionalArg (-wait);
	TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[0], optB, "-wait", 0, &wait));
	objv++;objc--;
	break;
      case TIMEOUT:
	CheckForAdditionalArg (-timeout);
	if (Tcl_GetIntFromObj (interp, objv[0], &timeout) != TCL_OK) {
	  Tcl_ResetResult(interp);
	  TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[0], optC, "option", 0, &iC));
	  switch ((enum optCE) iC) {
	    case DEFAULT:
	      timeout = -1;
	      break;
	    case USER:
	      timeout = -2;
	      break;
	    case MAX:
	      timeout = -3;
	      break;
	  }
	}
	objv++;objc--;
        break;
    }
  }
  ErrorMqToTclWithCheck (MqProcessEvent (mqctx, timeout, wait));
  RETURN_TCL
}
