/**
 *  \file       theLink/pymsgque/read_python.c
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

extern PyTypeObject NS(MqS);
extern PyTypeObject NS(MqBufferS);

///
///  READ
///

PyObject* NS(ReadL_START) (
  PyObject*	self,
  PyObject*	args
)
{
  SETUP_context
  MqBufferS_Obj *bufO = NULL;
  PyErrorCheck (PyArg_ParseTuple(args, "|O!:ReadL_START", &NS(MqBufferS), &bufO));
  ErrorMqToPythonWithCheck(MqReadL_START(context, (bufO == NULL ? NULL : bufO->buf)));
  Py_RETURN_NONE;
error:
  return NULL;
}

PyObject* NS(ReadL_END) (
  PyObject*	self 
)
{
  SETUP_context
  ErrorMqToPythonWithCheck(MqReadL_END(context));
  Py_RETURN_NONE;
error:
  return NULL;
}

PyObject* NS(ReadT_START) (
  PyObject*	self,
  PyObject*	args
)
{
  SETUP_context
  MqBufferS_Obj *bufO = NULL;
  PyErrorCheck (PyArg_ParseTuple(args, "|O!:ReadT_START", &NS(MqBufferS), &bufO));
  ErrorMqToPythonWithCheck(MqReadT_START(context, (bufO == NULL ? NULL : bufO->buf)));
  Py_RETURN_NONE;
error:
  return NULL;
}

PyObject* NS(ReadT_END) (
  PyObject*	self 
)
{
  SETUP_context
  ErrorMqToPythonWithCheck(MqReadT_END(context));
  Py_RETURN_NONE;
error:
  return NULL;
}

PyObject* NS(ReadY) (
  PyObject*	self 
)
{
  SETUP_context
  MQ_BYT i;
  ErrorMqToPythonWithCheck(MqReadY(context, &i));
  return PyLong_FromLong(i);
error:
  return NULL;
}

PyObject* NS(ReadO) (
  PyObject*	self 
)
{
  SETUP_context
  MQ_BOL i;
  ErrorMqToPythonWithCheck(MqReadO(context, &i));
  if (i == MQ_YES) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
error:
  return NULL;
}

PyObject* NS(ReadS) (
  PyObject*	self 
)
{
  SETUP_context
  MQ_SRT i;
  ErrorMqToPythonWithCheck(MqReadS(context, &i));
  return PyLong_FromLong(i);
error:
  return NULL;
}

PyObject* NS(ReadI) (
  PyObject*	self 
)
{
  SETUP_context
  MQ_INT i;
  ErrorMqToPythonWithCheck(MqReadI(context, &i));
  return PyLong_FromLong(i);
error:
  return NULL;
}

PyObject* NS(ReadF) (
  PyObject*	self 
)
{
  SETUP_context
  MQ_FLT f;
  ErrorMqToPythonWithCheck(MqReadF(context, &f));
  return PyFloat_FromDouble((double) f);
error:
  return NULL;
}

PyObject* NS(ReadW) (
  PyObject*	self 
)
{
  SETUP_context
  MQ_WID w;
  ErrorMqToPythonWithCheck(MqReadW(context, &w));
  return PyLong_FromLongLong((PY_LONG_LONG)w);
error:
  return NULL;
}

PyObject* NS(ReadD) (
  PyObject*	self 
)
{
  SETUP_context
  MQ_DBL d;
  ErrorMqToPythonWithCheck(MqReadD(context, &d));
  return PyFloat_FromDouble((double) d);
error:
  return NULL;
}

PyObject* NS(ReadC) (
  PyObject*	self 
)
{
  SETUP_context
  MQ_CST s;
  ErrorMqToPythonWithCheck(MqReadC(context, &s));
  return PyC2O(s);
error:
  return NULL;
}

PyObject* NS(ReadB) (
  PyObject*	self
)
{
  SETUP_context
  MQ_BIN b;
  MQ_SIZE len;
  ErrorMqToPythonWithCheck(MqReadB(context, &b, &len));
  return PyByteArray_FromStringAndSize((const char*)b, len);
error:
  return NULL;
}

PyObject* NS(ReadN) (
  PyObject*	self
)
{
  SETUP_context
  MQ_BIN b;
  MQ_SIZE len;
  ErrorMqToPythonWithCheck(MqReadN(context, &b, &len));
  return PyByteArray_FromStringAndSize((const char*)b, len);
error:
  return NULL;
}

PyObject* NS(ReadBDY) (
  PyObject*	self
)
{
  SETUP_context
  MQ_BIN b;
  MQ_SIZE len;
  ErrorMqToPythonWithCheck(MqReadBDY(context, &b, &len));
  return PyByteArray_FromStringAndSize((const char*)b, len);
error:
  return NULL;
}

PyObject* NS(ReadU) (
  PyObject*	self
)
{
  SETUP_context
  MQ_BUF buffer;
  ErrorMqToPythonWithCheck(MqReadU(context, &buffer));
  return MqBufferS_Obj_From_MQ_BUF(buffer);
error:
  return NULL;
}

PyObject* NS(ReadGetNumItems) (
  PyObject*	self
)
{
  return PyLong_FromLong(MqReadGetNumItems(CONTEXT));
}

PyObject* NS(ReadItemExists) (
  PyObject*	self
)
{
  if (MqReadItemExists(CONTEXT)) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

PyObject* NS(ReadUndo) (
  PyObject*	self
)
{
  SETUP_context
  ErrorMqToPythonWithCheck(MqReadUndo(context));
  Py_RETURN_NONE;
error:
  return NULL;
}

PyObject* NS(ReadProxy) (
  PyObject  *self,
  PyObject  *args
)
{
  SETUP_context
  MqS_Obj *sendCtx;
  if (!PyArg_ParseTuple(args, "O!:ReadProxy", &NS(MqS), &sendCtx))
    return NULL;
  ErrorMqToPythonWithCheck(MqReadProxy(context, &sendCtx->context));
  Py_RETURN_NONE;
error:
  return NULL;
}



