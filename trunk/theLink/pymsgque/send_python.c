/**
 *  \file       theLink/pymsgque/send_python.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_python.h"

extern PyTypeObject NS(MqBufferS);

///
///   SEND
///

PyObject* NS(SendSTART) (
  PyObject	*self 
)
{
  SETUP_context
  ErrorMqToPythonWithCheck (MqSendSTART(context));
  SETUP_RETURN
}

PyObject* NS(SendEND) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  const char *token;
  if (!PyArg_ParseTuple(args, "s:SendEND", &token)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendEND(context,token));
  SETUP_RETURN
}

PyObject* NS(SendEND_AND_WAIT) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  const char *token;
  int timeout = -1;

  if (!PyArg_ParseTuple(args, "s|i:SendEND_AND_WAIT", &token, &timeout)) {
    return NULL;
  }

  ErrorMqToPythonWithCheck (MqSendEND_AND_WAIT(context,token,timeout));

  SETUP_RETURN
}

PyObject* NS(SendEND_AND_CALLBACK) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  MQ_CST token;
  PyObject *callback;
  if (!PyArg_ParseTuple(args, "sO:SendEND_AND_CALLBACK", &token, &callback)) {
    return NULL;
  }
  Py_INCREF(callback);
  ErrorMqToPythonWithCheck (MqSendEND_AND_CALLBACK(context, token, NS(ProcCall), callback, NS(ProcFree)));
  SETUP_RETURN
}

PyObject* NS(SendRETURN) (
  PyObject	*self
)
{
  SETUP_context
  ErrorMqToPythonWithCheck (MqSendRETURN(context))
  SETUP_RETURN
}

PyObject* NS(SendERROR) (
  PyObject	*self
)
{
  SETUP_context
  ErrorMqToPythonWithCheck (MqSendERROR(context))
  SETUP_RETURN
}

PyObject* NS(SendL_START) (
  PyObject	*self
)
{
  SETUP_context
  ErrorMqToPythonWithCheck (MqSendL_START(context));
  SETUP_RETURN
}

PyObject* NS(SendL_END) (
  PyObject	*self
)
{
  SETUP_context
  ErrorMqToPythonWithCheck (MqSendL_END(context));
  SETUP_RETURN
}

PyObject* NS(SendT_START) (
  PyObject	*self ,
  PyObject	*args
)
{
  SETUP_context
  const char *c;
  if (!PyArg_ParseTuple(args, "s:SendT_START", &c)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendT_START(context,c));
  SETUP_RETURN
}

PyObject* NS(SendT_END) (
  PyObject	*self
)
{
  SETUP_context
  ErrorMqToPythonWithCheck (MqSendT_END(context));
  SETUP_RETURN
}

PyObject* NS(SendY) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  short int y;
  if (!PyArg_ParseTuple(args, "h:SendY", &y)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendY(context, (MQ_BYT) y));
  SETUP_RETURN
}

PyObject* NS(SendO) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  PyObject *O;
  if (!PyArg_ParseTuple(args, "O!:SendO", &PyBool_Type, &O)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendO(context, (O == Py_True ? MQ_YES : MQ_NO)));
  SETUP_RETURN
}

PyObject* NS(SendS) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  short int s;
  if (!PyArg_ParseTuple(args, "h:SendS", &s)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendS(context,s));
  SETUP_RETURN
}

PyObject* NS(SendI) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  int i;
  if (!PyArg_ParseTuple(args, "i:SendI", &i)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendI(context,i));
  SETUP_RETURN
}

PyObject* NS(SendF) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  float f;
  if (!PyArg_ParseTuple(args, "f:SendF", &f)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendF(context,f));
  SETUP_RETURN
}

PyObject* NS(SendW) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  PY_LONG_LONG w;
  if (!PyArg_ParseTuple(args, "L:SendW", &w)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendW(context,w));
  SETUP_RETURN
}

PyObject* NS(SendD) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  double d;
  if (!PyArg_ParseTuple(args, "d:SendD", &d)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendD(context,d));
  SETUP_RETURN
}

PyObject* NS(SendC) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  const char *c;
  if (!PyArg_ParseTuple(args, "s:SendC", &c)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendC(context,c));
  SETUP_RETURN
}

PyObject* NS(SendB) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  PyObject *o;
  if (!PyArg_ParseTuple(args, "O!:SendB", &PyByteArray_Type, &o)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendB(context,(MQ_BIN)PyByteArray_AS_STRING(o),PyByteArray_GET_SIZE(o)));
  SETUP_RETURN
}

PyObject* NS(SendN) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  PyObject *o;
  if (!PyArg_ParseTuple(args, "O!:SendN", &PyByteArray_Type, &o)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendN(context,(MQ_BIN)PyByteArray_AS_STRING(o),PyByteArray_GET_SIZE(o)));
  SETUP_RETURN
}

PyObject* NS(SendBDY) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  PyObject *o;
  if (!PyArg_ParseTuple(args, "O!:SendBDY", &PyByteArray_Type, &o)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendBDY(context,(MQ_BIN)PyByteArray_AS_STRING(o),PyByteArray_GET_SIZE(o)));
  SETUP_RETURN
}

PyObject* NS(SendU) (
  PyObject  *self,
  PyObject  *args
)
{
  SETUP_context
  MqBufferS_Obj *bufO = NULL;
  if (!PyArg_ParseTuple(args, "O!:SendO", &NS(MqBufferS), &bufO)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqSendU(context,bufO->buf));
  SETUP_RETURN
}
