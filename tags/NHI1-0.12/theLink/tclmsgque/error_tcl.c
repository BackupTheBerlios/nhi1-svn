/**
 *  \file       theLink/tclmsgque/error_tcl.c
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

int NS(ErrorGetText) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult (interp, Tcl_NewStringObj(MqErrorGetText(MQCTX), -1));
  RETURN_TCL
}

int NS(ErrorGetNum) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult (interp, Tcl_NewIntObj(MqErrorGetNumI(MQCTX)));
  RETURN_TCL
}

int NS(ErrorGetCode) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult (interp, Tcl_NewIntObj((int)MqErrorGetCodeI(MQCTX)));
  RETURN_TCL
}

int NS(ErrorC) (NS_ARGS)
{
  MQ_INT errnum;
  MQ_STR prefix, error_message;
  SETUP_mqctx
  CHECK_C(prefix)
  CHECK_I(errnum)
  CHECK_C(error_message)
  CHECK_NOARGS
  MqErrorC(mqctx, prefix, errnum, error_message);
  RETURN_TCL
}

int NS(ErrorRaise) (NS_ARGS)
{
  SETUP_mqctx
  ErrorMqToTclWithCheck(MqErrorGetCodeI(mqctx));
  RETURN_TCL
}

int NS(ErrorSet) (NS_ARGS)
{
  CHECK_NOARGS
  NS(ProcError) (tclctx, __func__);
  RETURN_TCL
}

int NS(ErrorSetCONTINUE) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  MqErrorSetCONTINUE(mqctx);
  RETURN_TCL
}

int NS(ErrorIsEXIT) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (MqErrorIsEXIT(MQCTX)));
  RETURN_TCL
}

int NS(ErrorReset) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_ResetResult (interp);
  MqErrorReset (MQCTX);
  RETURN_TCL
}

int NS(ErrorPrint) (NS_ARGS)
{
  CHECK_NOARGS
  MqErrorPrint (MQCTX);
  RETURN_TCL
}

