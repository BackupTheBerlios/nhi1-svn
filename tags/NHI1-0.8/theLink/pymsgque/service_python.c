/**
 *  \file       theLink/pymsgque/service_python.c
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

PyObject* NS(ServiceGetToken) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqServiceGetToken(CONTEXT));
}

PyObject* NS(ServiceGetFilter) (
  PyObject  *self ,
  PyObject  *args
)
{
  SETUP_context
  struct MqS *filter;
  int id = 0;
  PyObject *filterO = NULL;
  if (!PyArg_ParseTuple(args, "|i:ServiceGetFilter", &id)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqServiceGetFilter (context, id, &filter));
  filterO = ((PyObject *)filter->self);
  Py_INCREF(filterO);
error:
  return filterO;
}

PyObject* NS(ServiceIsTransaction) (
  MqS_Obj   *self
)
{
  if (MqServiceIsTransaction(CONTEXT)) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

PyObject* NS(ServiceCreate) (
  PyObject  *self, 
  PyObject  *args
)
{
  const char *token;
  PyObject *callable;
  SETUP_context;
  if (!PyArg_ParseTuple(args, "sO:ServiceCreate", &token, &callable)) {
    return NULL;
  }

  // verify the callable object
  if (!PyCallable_Check(callable)) {
    PyErr_SetString(PyExc_TypeError, "second parameter must be callable");
    return NULL;
  }
  Py_INCREF(callable);

  // create the service
  ErrorMqToPythonWithCheck(MqServiceCreate(context, token, NS(ProcCall), (void*) callable, NS(ProcFree)));
  Py_RETURN_NONE;
error:
  return NULL;
}

PyObject* NS(ServiceProxy) (
  PyObject  *self, 
  PyObject  *args
)
{
  const char *token;
  int id=0;
  SETUP_context;
  if (!PyArg_ParseTuple(args, "s|i:ServiceProxy", &token, &id)) {
    return NULL;
  }

  ErrorMqToPythonWithCheck(MqServiceProxy(context, token, id));
  Py_RETURN_NONE;
error:
  return NULL;
}

PyObject* NS(ServiceDelete) (
  PyObject  *self, 
  PyObject  *args
)
{
  const char *token;
  SETUP_context;
  if (!PyArg_ParseTuple(args, "s:ServiceDelete", &token)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck(MqServiceDelete(context, token));
  Py_RETURN_NONE;
error:
  return NULL;
}


