/**
 *  \file       tclmsgque/send_tcl.c
 *  \brief      \$Id: send_tcl.c 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"

int NS(SendSTART) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendSTART(mqctx));
  RETURN_TCL
}

int NS(SendEND) (NS_ARGS)
{
  SETUP_mqctx
  MQ_STR token;
  CHECK_C(token)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendEND(mqctx,token));
  RETURN_TCL
}

int NS(SendEND_AND_WAIT) (NS_ARGS)
{
  SETUP_mqctx
  MQ_STR token;
  MQ_INT timeout=MQ_TIMEOUT;
  CHECK_C(token)
  CHECK_DI(timeout)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendEND_AND_WAIT(mqctx,token,timeout));
  RETURN_TCL
}

int NS(SendEND_AND_CALLBACK) (NS_ARGS)
{
  SETUP_mqctx
  MQ_STR token;
  Tcl_Obj *callback;
  CHECK_C(token)
  CHECK_OBJ(callback)
  CHECK_NOARGS
  Tcl_IncrRefCount(callback);
  ErrorMqToTclWithCheck(MqSendEND_AND_CALLBACK(mqctx, token, NS(ProcCall), callback, NS(ProcFree)));
  RETURN_TCL
}

int NS(SendRETURN) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendRETURN(mqctx));
  RETURN_TCL
}

int NS(SendFTR) (NS_ARGS)
{
  SETUP_mqctx
  MQ_INT timeout=MQ_TIMEOUT;
  CHECK_DI(timeout)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendFTR(mqctx, timeout));
  RETURN_TCL
}

int NS(SendEOF) (NS_ARGS)
{
  SETUP_mqctx
  MQ_INT timeout=MQ_TIMEOUT;
  CHECK_DI(timeout)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendEOF(mqctx, timeout));
  RETURN_TCL
}

int NS(SendERROR) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendERROR(mqctx));
  RETURN_TCL
}

int NS(SendY) (NS_ARGS)
{
  SETUP_mqctx
  MQ_BYT val;
  CHECK_Y(val)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendY(mqctx, val));
  RETURN_TCL
}

int NS(SendO) (NS_ARGS)
{
  SETUP_mqctx
  MQ_BOL val;
  CHECK_O(val)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendO(mqctx, val));
  RETURN_TCL
}

int NS(SendS) (NS_ARGS)
{
  SETUP_mqctx
  MQ_SRT val;
  CHECK_S(val)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendS(mqctx, val));
  RETURN_TCL
}

int NS(SendI) (NS_ARGS)
{
  SETUP_mqctx
  MQ_INT val;
  CHECK_I(val)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendI(mqctx, val));
  RETURN_TCL
}

int NS(SendF) (NS_ARGS)
{
  SETUP_mqctx
  MQ_FLT val;
  CHECK_F(val)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendF(mqctx, val));
  RETURN_TCL
}

int NS(SendW) (NS_ARGS)
{
  SETUP_mqctx
  MQ_WID val;
  CHECK_W(val)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendW(mqctx, val));
  RETURN_TCL
}

int NS(SendD) (NS_ARGS)
{
  SETUP_mqctx
  MQ_DBL val;
  CHECK_D(val)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendD(mqctx, val));
  RETURN_TCL
}

int NS(SendC) (NS_ARGS)
{
  SETUP_mqctx
  MQ_STR val;
  CHECK_C(val)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendC(mqctx, val));
  RETURN_TCL
}

int NS(SendB) (NS_ARGS)
{
  SETUP_mqctx
  MQ_BIN val;
  MQ_SIZE len;
  CHECK_B(val,len)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendB(mqctx, val, len));
  RETURN_TCL
}

int NS(SendU) (NS_ARGS)
{
  SETUP_mqctx
  struct MqBufferS * MqBufferS_obj;
  CHECK_BUFFER(MqBufferS_obj)
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendU(mqctx, MqBufferS_obj));
  RETURN_TCL
}

int NS(SendL_START) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendL_START(mqctx));
  RETURN_TCL
}

int NS(SendL_END) (NS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck(MqSendL_END(mqctx));
  RETURN_TCL
}

int NS(SendAll) (NS_ARGS)
{
  SETUP_mqctx
  for (;skip<objc;skip++) {
    ErrorMqToTclWithCheck(MqSendC(mqctx, Tcl_GetString(objv[skip])));
  }
  RETURN_TCL
}
