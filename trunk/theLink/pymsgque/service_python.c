/**
 *  \file       theLink/pymsgque/service_python.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_python.h"

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


