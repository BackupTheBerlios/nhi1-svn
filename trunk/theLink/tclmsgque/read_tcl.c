/**
 *  \file       theLink/tclmsgque/read_tcl.c
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

#define MSGQUE msgque

int NS(ReadL_START) (NS_ARGS)
{
  SETUP_mqctx
  MQ_BUF MqBufferS_obj = NULL;
  if (skip != objc) {
    CHECK_BUFFER (MqBufferS_obj)
  }
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadL_START(mqctx, MqBufferS_obj));
  RETURN_TCL
}

int NS(ReadL_END) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadL_END(mqctx));
  RETURN_TCL
}

int NS(ReadT_START) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadT_START(mqctx));
  RETURN_TCL
}

int NS(ReadT_END) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadT_END(mqctx));
  RETURN_TCL
}

int NS(ReadY) (NS_ARGS)
{
  SETUP_mqctx
  MQ_BYT val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadY(mqctx, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

int NS(ReadO) (NS_ARGS)
{
  SETUP_mqctx
  MQ_BOL val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadO(mqctx, &val));
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj(val));
  RETURN_TCL
}

int NS(ReadS) (NS_ARGS)
{
  SETUP_mqctx
  MQ_SRT val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadS(mqctx, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

int NS(ReadI) (NS_ARGS)
{
  SETUP_mqctx
  MQ_INT val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadI(mqctx, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

int NS(ReadF) (NS_ARGS)
{
  SETUP_mqctx
  MQ_FLT val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadF(mqctx, &val));
  Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
  RETURN_TCL
}

int NS(ReadW) (NS_ARGS)
{
  SETUP_mqctx
  MQ_WID val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadW(mqctx, &val));
  Tcl_SetObjResult(interp, Tcl_NewWideIntObj(val));
  RETURN_TCL
}

int NS(ReadD) (NS_ARGS)
{
  SETUP_mqctx
  MQ_DBL val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadD(mqctx, &val));
  Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
  RETURN_TCL
}

int NS(ReadC) (NS_ARGS)
{
  SETUP_mqctx
  MQ_CST val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadC(mqctx, &val));
  Tcl_SetObjResult(interp, Tcl_NewStringObj(val,-1));
  RETURN_TCL
}

int NS(ReadB) (NS_ARGS)
{
  SETUP_mqctx
  MQ_BIN val;
  MQ_SIZE len;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadB(mqctx, &val, &len));
  Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(val,len));
  RETURN_TCL
}

int NS(ReadN) (NS_ARGS)
{
  SETUP_mqctx
  MQ_CBI val;
  MQ_SIZE len;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadN(mqctx, &val, &len));
  Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(val,len));
  RETURN_TCL
}

int NS(ReadDUMP) (NS_ARGS)
{
  SETUP_mqctx
  struct MqDumpS *val;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadDUMP(mqctx, &val));
  NS(MqDumpS_New)(interp, val);
  RETURN_TCL
}

int NS(ReadLOAD) (NS_ARGS)
{
  struct MqDumpS *dump;
  SETUP_mqctx
  CHECK_DUMP(dump);
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadLOAD(mqctx, dump));
  RETURN_TCL
}

int NS(ReadU) (NS_ARGS)
{
  SETUP_mqctx
  struct MqBufferS * buf = NULL;
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadU(mqctx, &buf));
  NS(MqBufferS_New) (interp, buf);
  return TCL_OK;
error:
  return TCL_ERROR;
}

int NS(ReadGetNumItems) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewIntObj(MqReadGetNumItems(mqctx)));
  RETURN_TCL
}

int NS(ReadItemExists) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj(MqReadItemExists(mqctx)));
  RETURN_TCL
}

int NS(ReadUndo) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadUndo(mqctx));
  RETURN_TCL
}

int NS(ReadProxy) (NS_ARGS)
{
  SETUP_mqctx
  struct MqS * MqS_object;
  CHECK_MQS (MqS_object)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadProxy(mqctx, MqS_object));
  RETURN_TCL
}

int NS(ReadForward) (NS_ARGS)
{
  SETUP_mqctx
  struct MqS * target;
  CHECK_MQS (target)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqReadForward(mqctx, target));
  RETURN_TCL
}

int NS(ReadALL) (NS_ARGS)
{
  SETUP_mqctx
  Tcl_Obj *RET = Tcl_NewListObj(0,NULL);
  Tcl_Obj *OBJ;
  MQ_BUF buf;
  CHECK_NOARGS
  while (MqReadItemExists(mqctx)) {
    OBJ = NULL;
    MqReadU(mqctx, &buf);
    switch (buf->type) {
      case MQ_BYTT: {
	OBJ = Tcl_NewIntObj(buf->cur.A->Y);
	break;
      }
      case MQ_BOLT: {
	OBJ = Tcl_NewBooleanObj(buf->cur.A->O);
	break;
      }
      case MQ_SRTT: {
	OBJ = Tcl_NewIntObj(buf->cur.A->S);
	break;
      }
      case MQ_INTT: {
	OBJ = Tcl_NewIntObj(buf->cur.A->I);
	break;
      }
      case MQ_FLTT: {
	OBJ = Tcl_NewDoubleObj(buf->cur.A->F);
	break;
      }
      case MQ_WIDT: {
	OBJ = Tcl_NewWideIntObj(buf->cur.A->W);
	break;
      }
      case MQ_DBLT: {
	OBJ = Tcl_NewDoubleObj(buf->cur.A->D);
	break;
      }
      case MQ_BINT: {
	OBJ = Tcl_NewByteArrayObj(buf->cur.B,buf->cursize);
	break;
      }
      case MQ_STRT: {
	OBJ = Tcl_NewStringObj(buf->cur.C,-1);
	break;
      }
      case MQ_LSTT: {
	MqReadL_START(mqctx, buf);
	NS(ReadALL)(interp, tclctx, skip, objc, objv);
	MqReadL_END(mqctx);
	OBJ = Tcl_GetObjResult(interp);
	break;
      }
      case MQ_TRAT: {
	MqPanicSYS(mqctx);
      }
    }
    if (OBJ != NULL) Tcl_ListObjAppendElement(interp, RET, OBJ);
  }
  Tcl_SetObjResult(interp, RET);
  RETURN_TCL
}

