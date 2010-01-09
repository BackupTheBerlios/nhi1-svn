/**
 *  \file       theLink/pymsgque/error_python.c
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

extern PyObject * NS(MqSException);

PyObject* NS(ErrorC) (
  PyObject  *self,
  PyObject  *args      
)
{
  const char *func, *message;
  int num;
  SETUP_context;
  if (!PyArg_ParseTuple(args, "sis:ErrorC", &func, &num, &message)) {
    return NULL;
  }
  MqErrorC(context, func, num, message);
  Py_RETURN_NONE;
}

PyObject* NS(ErrorRaise) (
  PyObject  *self
)
{
  SETUP_context;
  ErrorMqToPythonWithCheck(MqErrorGetCodeI(context));
  SETUP_RETURN
}

PyObject* NS(ErrorReset) (
  PyObject  *self
)
{
  MqErrorReset(CONTEXT);
  Py_RETURN_NONE;
}

PyObject* NS(ErrorPrint) (
  PyObject  *self
)
{
  MqErrorPrint(CONTEXT);
  Py_RETURN_NONE;
}

PyObject* NS(ErrorGetNum) (
  PyObject  *self
)
{
  return PyLong_FromLong(MqErrorGetNumI(CONTEXT));
}

PyObject* NS(ErrorGetText) (
  PyObject  *self
)
{
  return PyC2O(MqErrorGetText(CONTEXT));
}

PyObject* NS(ErrorSetCONTINUE)(
  PyObject  *self
)
{
  MqErrorSetCONTINUE(CONTEXT);
  Py_RETURN_NONE;
}

PyObject* NS(ErrorSet)(
  PyObject  *self
)
{
  SETUP_context
  PyObject *typeO = NULL, *valueO=NULL, *tbO = NULL, *retO = Py_None;

  PyThreadState *tstate = PyThreadState_Get();

  if (tstate->curexc_type == NULL || tstate->curexc_type == Py_None) {
    typeO = tstate->exc_type;
    valueO = tstate->exc_value;
    tbO = tstate->exc_traceback;
  } else {
    typeO = tstate->curexc_type;
    valueO = tstate->curexc_value;
    tbO = tstate->curexc_traceback;
  }
  Py_XINCREF(typeO);
  Py_XINCREF(valueO);
  Py_XINCREF(tbO);
  PyErr_Clear();

  {
    PyObject *valO = NULL ;

    if (typeO == Py_None || typeO == NULL) {
      MqErrorC (context, __func__, -1, "No active exception to reraise");

    } else if (PyErr_GivenExceptionMatches(typeO,NS(MqSException))) {
      MqErrorSet (context, 
       (MQ_INT) PyLong_AsLong(PyObject_GetAttrString(typeO,"num")),
       (enum MqErrorE) PyLong_AsLong(PyObject_GetAttrString(typeO,"code")),
       (MQ_CST const) PyBytes_AsString(PyUnicode_AsUTF8String(PyObject_GetAttrString(typeO,"text")))
      );

    } else if (valueO) {
      PyObject *tmod = NULL, *strO = NULL, *lstO = NULL;
      PyErrorCheckNT(end1,  tmod = PyImport_ImportModule("traceback"));
      if (tbO) {
	PyErr_NormalizeException(&typeO, &valueO, &tbO);
	PyErrorCheckNT(end1,  lstO = PyObject_CallMethod(tmod, "format_exception", "OOO", typeO, valueO, tbO));
      } else {
	PyErrorCheckNT(end1,  lstO = PyObject_CallMethod(tmod, "format_exception_only", "OO", typeO, valueO));
      }
      PyErrorCheckNT(end1,  strO = PyC2O(""));
      PyErrorCheckNT(end1,  valO = PyObject_CallMethod(strO, "join", "O", lstO));
end1:
      Py_XDECREF(tmod);
      Py_XDECREF(lstO);
      Py_XDECREF(strO);
      if (valO != NULL) {
	PyObject *strO=NULL, *utf8=NULL;

	PyErrorCheckNT(end2, strO = PyObject_Str(valO));
	PyErrorCheckNT(end2, utf8 = PyUnicode_AsUTF8String(strO));
	MqErrorC(context, __func__, -1, PyBytes_AsString(utf8));
end2:
	Py_XDECREF(utf8);
	Py_XDECREF(strO);
      }
      if (MqErrorGetCodeI(context) != MQ_ERROR)
	MqErrorV(context, __func__, -1, "%s: python error", PyExceptionClass_Name(typeO));
    }
  }

  Py_XDECREF(typeO);
  Py_XDECREF(valueO);
  Py_XDECREF(tbO);
  Py_XINCREF(retO);

  return retO;
}

