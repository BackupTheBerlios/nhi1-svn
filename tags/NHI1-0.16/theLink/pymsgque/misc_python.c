/**
 *  \file       theLink/pymsgque/misc_python.c
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

#define MQ_CONTEXT_S context

extern PyObject * NS(MqSException);

//////////////////////////////////////////////////////////////////////////////////
///
///				    Helper
///
//////////////////////////////////////////////////////////////////////////////////

enum MqErrorE 
NS(ProcCall) (
  struct MqS  *	const context,
  MQ_PTR const dataP
)
{
  PyObject * result;
  PyObject * const self = (PyObject*) SELF;
  PyObject * selfarg = self;
  PyObject * const callable = (PyObject*) dataP;

  //MqDLogX (context,"11111",0,"thread<%p>, state<%p>\n", pthread_self(), PyThreadState_Get());
  PyErr_Clear();

  // for methods of the instance 'self' the argument 'self' is allready set
  if (PyMethod_Check(callable) && PyMethod_Self(callable) == self) selfarg = NULL;

  // call the function
  // MQ_CST data = PyBytes_AsString(PyUnicode_AsASCIIString(PyObject_Repr(callable))); printC(data)
  result = PyObject_CallFunctionObjArgs(callable, selfarg, NULL);

  //MqDLogX (context,"22222",0,"thread<%p>, state<%p>\n", pthread_self(), PyThreadState_Get());

  // no error return OK
  if (result == NULL) {
    NS(ErrorSet) (self);
  } else {
    Py_DECREF(result);
  }

  //MqDLogX (context,"33333",0,"thread<%p>, state<%p>\n", pthread_self(), PyThreadState_Get());

  return MqErrorGetCodeI(context);
}

void
MQ_DECL NS(ProcFree) (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  Py_DECREF(*dataP);
  *dataP = NULL;
}

void
MQ_DECL NS(ProcCopy) (
  struct MqS * const context,
  MQ_PTR *dataP
)
{
  // MQ_CST data = PyBytes_AsString(PyUnicode_AsASCIIString(PyObject_Repr(*dataP))); printC(data)
  PyObject* const function = PyMethod_Function(*dataP);
  if (function != NULL) {
    *dataP = (void*) PyMethod_New(function, (PyObject*)context->self);
    if (*dataP == NULL) MqPanicC(context,__func__,-1,"unable to 'PyMethod_New'");
    // MQ_CST data = PyBytes_AsString(PyUnicode_AsASCIIString(PyObject_Repr(*dataP))); printC(data)
  }
  Py_INCREF(*dataP);
}

void NS(pErrorFromMq) (
  struct MqS * const context
)
{
  PyErr_SetString(NS(MqSException), MqErrorGetText(context));
  PyObject_SetAttrString(NS(MqSException), "text", PyC2O(MqErrorGetText(context)));
  PyObject_SetAttrString(NS(MqSException), "num", PyLong_FromLong(MqErrorGetNumI(context)));
  PyObject_SetAttrString(NS(MqSException), "code", PyLong_FromLong(MqErrorGetCodeI(context)));
  MqErrorReset(context);
}

enum MqErrorE ListToMqBufferLS (
  struct MqS * const context,
  PyObject *argsO,
  struct MqBufferLS ** argsP
)
{
  *argsP = NULL;
  if (argsO == NULL) {
    return MQ_OK;
  } else {
    struct MqBufferLS * args = *argsP = MqBufferLCreate (PyList_GET_SIZE(argsO));
    PyObject *iterator = PyObject_GetIter(argsO);
    PyObject *item, *utf8;

    while ((item = PyIter_Next(iterator))) {
      PyErrorCheckN (utf8 = PyUnicode_AsUTF8String(item))
      MqBufferLAppendC(args, PyBytes_AsString(utf8));
      Py_DECREF(utf8);
      Py_DECREF(item);
    }
    Py_DECREF(iterator);
  }
  return MQ_OK;

error:
  MqBufferLDelete (argsP);
  NS(ErrorSet) ((PyObject*)context->self);
  return MQ_ERROR;
}


