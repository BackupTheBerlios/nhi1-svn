/**
 *  \file       theLink/pymsgque/factory_python.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_python.h"

extern PyTypeObject NS(MqS);

#define MQ_CONTEXT_S CONTEXT

enum MqErrorE
NS(FactoryCreate) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS *item,
  struct MqS ** contextP
)
{
  struct MqS * mqctx;
  PyObject *args, *result;
  PyTypeObject* type = (PyTypeObject*) item->Create.data;
  PyObject *tmplO = NULL;

  if (create == MQ_FACTORY_NEW_FORK) PyOS_AfterFork();

  // setup args
  if (create == MQ_FACTORY_NEW_INIT) {
    args = PyTuple_New(0);
  } else {
    args = PyTuple_New(1);
    tmplO = (PyObject*)SELFX(tmpl);
    PyTuple_SET_ITEM(args, 0, tmplO);
    Py_INCREF(tmplO);
  }

  // call constructor
  result = PyObject_Call((PyObject*)type, args, NULL);
  Py_DECREF(args);

  // call factory object 
  if (result == NULL || PyErr_Occurred() != NULL) goto error1;

  // right type
  if (PyObject_IsInstance(result, (PyObject*) &NS(MqS))) {
    mqctx = &((MqS_Obj*)result)->context;
  } else {
    goto error1;
  }

  // check for MQ error
  MqErrorCheck(MqErrorGetCode(mqctx));

  // copy and initialize "setup" data
  if (create != MQ_FACTORY_NEW_INIT) {
    MqSetupDup(mqctx, tmpl);
  }

  *contextP = mqctx;
  return MQ_OK;

error:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    MqErrorCopy (tmpl, mqctx);
    MqContextDelete (&mqctx);
    return MqErrorStack(tmpl);
  } else {
    return MQ_ERROR;
  }

error1:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    NS(ErrorSet) ((PyObject*)tmpl->self);
    return MqErrorGetCode(tmpl);
  } else {
    return MQ_ERROR;
  }

/*
error2:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    return MqErrorC(tmpl, __func__, -1, "Factory return no MqS type");
  } else {
    return MQ_ERROR;
  }
*/

}

void MQ_DECL
NS(FactoryDelete)(
  struct MqS * context,
  MQ_BOL doFactoryDelete,
  struct MqFactoryS * const data
)
{
  SETUP_self
  MqContextFree (context);
  Py_DECREF (self);
}

//////////////////////////////////////////////////////////////////////////////////
///
///				 context
///
//////////////////////////////////////////////////////////////////////////////////

PyObject* NS(FactoryCtxIdentSet) (
  MqS_Obj    *self,
  PyObject   *args
)
{
  SETUP_context
  const char *c;
  if (!PyArg_ParseTuple(args, "s:FactoryCtxIdentSet", &c)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqFactoryCtxIdentSet (context, c));
  SETUP_RETURN
}

PyObject* NS(FactoryCtxIdentGet) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqFactoryCtxIdentGet(&ICONTEXT));
}

PyObject* NS(FactoryCtxDefaultSet) (
  MqS_Obj   *self,
  PyObject  *args
)
{
  SETUP_context
  const char *c;
  if (!PyArg_ParseTuple(args, "s:FactoryCtxDefaultSet", &c)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqFactoryCtxDefaultSet (context, c));
  SETUP_RETURN
}

//////////////////////////////////////////////////////////////////////////////////
///
///				 static
///
//////////////////////////////////////////////////////////////////////////////////

PyObject* NS(FactoryAdd) (
  PyObject    *class,
  PyObject    *args
)
{
  enum MqFactoryReturnE ret;
  SETUP_FACTORY_ARG(FactoryAdd)
  ret = MqFactoryAdd(ident, NS(FactoryCreate), arg, NS(ProcFree), NS(FactoryDelete), NULL, NULL);
  MqFactoryErrorCheck(ret);
  Py_RETURN_NONE;
error:
  if (PyErr_Occurred() == NULL)
    PyErr_SetString(PyExc_RuntimeError, MqFactoryErrorMsg(ret));
  return NULL;
}

PyObject* NS(FactoryDefault) (
  PyObject    *class,
  PyObject    *args
)
{
  enum MqFactoryReturnE ret;
  MQ_CST ident;
  PyObject *arg;
  if (PyTuple_GET_SIZE(args) == 1) {
    if (!PyArg_ParseTuple(args, "s:FactoryDefault", &ident)) {
      return NULL;
    }
    arg = (PyObject*) &NS(MqS);
  } else {
    if (!PyArg_ParseTuple(args, "sO!:FactoryDefault", &ident, &PyType_Type, &arg)) {
      return NULL;
    }
  }
  Py_INCREF (arg);
  ret = MqFactoryDefault(ident, NS(FactoryCreate), arg, NS(ProcFree), NS(FactoryDelete), NULL, NULL);
  MqFactoryErrorCheck(ret);
  Py_RETURN_NONE;
error:
  if (PyErr_Occurred() == NULL)
    PyErr_SetString(PyExc_RuntimeError, MqFactoryErrorMsg(ret));
  return NULL;
}

PyObject* NS(FactoryDefaultIdent) (
  PyObject    *self
)
{
  return PyC2O(MqFactoryDefaultIdent());
}

PyObject* NS(FactoryNew) (
  PyObject    *class,
  PyObject    *args
)
{
  enum MqFactoryReturnE ret;
  struct MqS *mqctx;
  SETUP_FACTORY_ARG(FactoryNew)
  ret = MqFactoryNew(ident, NS(FactoryCreate), arg, NS(ProcFree), NS(FactoryDelete), NULL, NULL, NULL, &mqctx);
  MqFactoryErrorCheck(ret);
  arg = ((PyObject *)SELFX(mqctx));
  Py_INCREF(arg);
  return arg;
error:
  if (PyErr_Occurred() == NULL)
    PyErr_SetString(PyExc_RuntimeError, MqFactoryErrorMsg(ret));
  return NULL;
}

PyObject* NS(FactoryCall) (
  PyObject    *class,
  PyObject    *args
)
{
  enum MqFactoryReturnE ret;
  struct MqS *mqctx;
  const char *c;
  PyObject *arg;
  if (!PyArg_ParseTuple(args, "s:FactoryCall", &c)) {
    return NULL;
  }
  ret = MqFactoryCall(c, NULL, &mqctx);
  MqFactoryErrorCheck(ret);
  arg = ((PyObject *)SELFX(mqctx));
  Py_INCREF(arg);
  return arg;
error:
  if (PyErr_Occurred() == NULL)
    PyErr_SetString(PyExc_RuntimeError, MqFactoryErrorMsg(ret));
  return NULL;
}

