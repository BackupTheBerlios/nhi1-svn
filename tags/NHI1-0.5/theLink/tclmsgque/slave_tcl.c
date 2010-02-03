/**
 *  \file       theLink/tclmsgque/slave_tcl.c
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

#define MQ_CONTEXT_S mqctx

int NS(SlaveWorker) (NS_ARGS)
{
  SETUP_mqctx
  struct MqBufferLS * argv;
  int id;

  CHECK_I(id)
  CHECK_ARGS(argv)

  ErrorMqToTclWithCheck (MqSlaveWorker (mqctx, id, &argv));
  RETURN_TCL
}

int NS(SlaveCreate) (NS_ARGS)
{
  SETUP_mqctx
  MQ_SIZE id;
  struct MqS *MqS_object;
  CHECK_I(id)
  CHECK_MQS(MqS_object)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqSlaveCreate (mqctx, id, MqS_object));
  RETURN_TCL
}

int NS(SlaveDelete) (NS_ARGS)
{
  SETUP_mqctx
  MQ_SIZE id;
  CHECK_I(id)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqSlaveDelete (mqctx, id));
  RETURN_TCL
}

int NS(SlaveGet) (NS_ARGS)
{
  SETUP_mqctx
  struct MqS * slave;
  MQ_SIZE id;
  CHECK_I(id)
  slave = MqSlaveGet(mqctx, id);
  if (slave == NULL) {
    Tcl_SetResult(interp, "", TCL_STATIC);
  } else {
    Tcl_SetObjResult(interp, (Tcl_Obj*)slave->self);
  }
  RETURN_TCL
}

int NS(SlaveGetMaster) (NS_ARGS)
{
  struct MqS * const master = MqSlaveGetMasterI(MQCTX);
  CHECK_NOARGS
  if (master != NULL)
    Tcl_SetObjResult(interp, (Tcl_Obj*)master->self);
  else
    Tcl_SetResult(interp, "", TCL_STATIC);
  RETURN_TCL
}

int NS(SlaveIs) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj (MqSlaveIs(MQCTX)));
  RETURN_TCL
}
