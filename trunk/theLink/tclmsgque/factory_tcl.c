/**
 *  \file       theLink/tclmsgque/factory_tcl.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"

/*****************************************************************************/
/*                                                                           */
/*                             factory-static                                */
/*                                                                           */
/*****************************************************************************/

int NS(FactoryAdd) (TCL_ARGS)
{
  enum MqFactoryReturnE ret;
  int skip = 2;
  MQ_CST ident;
  Tcl_Obj *factory;
  CHECK_C(ident)
  CHECK_PROC(factory, "FactoryAdd ident factory-proc")
  CHECK_NOARGS
  Tcl_IncrRefCount(factory);
  MqFactoryErrorCheck(ret = MqFactoryAdd(ident, NS(FactoryCreate), factory, NULL, NS(FactoryDelete), NULL, NULL));
  return TCL_OK;
error:
  Tcl_SetResult(interp, (MQ_STR) MqFactoryErrorMsg(ret), TCL_STATIC);
  return TCL_ERROR;
}

int NS(FactoryCall) (TCL_ARGS)
{
  struct MqS * mqctx;
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_CALL_ERR;
  int skip = 2;
  MQ_CST ident;
  CHECK_C(ident)
  CHECK_NOARGS
  MqFactoryErrorCheck(ret = MqFactoryCall (ident, (MQ_PTR) interp, &mqctx));
  Tcl_SetObjResult(interp, (Tcl_Obj*) mqctx->self);
  return TCL_OK;
error:
  Tcl_SetResult(interp, (MQ_STR) MqFactoryErrorMsg(ret), TCL_STATIC);
  return TCL_ERROR;
}

int NS(FactoryNew) (TCL_ARGS)
{
  struct MqS * mqctx;
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_NEW_ERR;
  int skip = 2;
  MQ_CST ident;
  Tcl_Obj *factory;
  CHECK_C(ident)
  CHECK_PROC(factory, "FactoryNew ident factory-proc")
  CHECK_NOARGS
  MqFactoryErrorCheck(ret = MqFactoryNew(ident, 
    NS(FactoryCreate), factory, NULL, NS(FactoryDelete), NULL, NULL, (MQ_PTR) interp, &mqctx)
  );
  Tcl_SetObjResult(interp, (Tcl_Obj*) mqctx->self);
  return TCL_OK;
error:
  Tcl_SetResult(interp, (MQ_STR) MqFactoryErrorMsg(ret), TCL_STATIC);
  return TCL_ERROR;
}

/*****************************************************************************/
/*                                                                           */
/*                             factory-context                               */
/*                                                                           */
/*****************************************************************************/

int NS(FactoryCtxIdentSet) (NS_ARGS)
{
  SETUP_mqctx;
  MQ_CST ident;
  CHECK_C(ident)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqFactoryCtxIdentSet (mqctx, ident));
  RETURN_TCL
}

int NS(FactoryCtxIdentGet) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR) MqFactoryCtxIdentGet(&tclctx->mqctx), TCL_STATIC);
  RETURN_TCL
}

int NS(FactoryCtxDefaultSet) (NS_ARGS)
{
  SETUP_mqctx;
  MQ_CST ident;
  CHECK_C(ident)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqFactoryCtxDefaultSet(mqctx, ident));
  RETURN_TCL
}
