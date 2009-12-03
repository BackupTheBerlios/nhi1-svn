/**
 *  \file       theLink/tclmsgque/service_tcl.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"

int NS(ServiceCreate) (NS_ARGS)
{
  SETUP_mqctx
  MQ_STR token;
  Tcl_Obj * service;
  CHECK_C(token)
  CHECK_PROC(service, "CONTEXT ServiceCreate token tclproc")
  CHECK_NOARGS
  Tcl_IncrRefCount (service);
  ErrorMqToTclWithCheck (MqServiceCreate (mqctx, token, NS(ProcCall), service, NS(ProcFree)));
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
