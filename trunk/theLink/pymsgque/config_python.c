/**
 *  \file       theLink/pymsgque/config_python.c
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

#define MQ_CONTEXT_S CONTEXT

enum MqErrorE
NS(FactoryCreate) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryItemS *item,
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
  struct MqFactoryItemS * const data
)
{
  SETUP_self
  MqContextFree (context);
  Py_DECREF (self);
}

//////////////////////////////////////////////////////////////////////////////////
///
///				 Method definition
///
//////////////////////////////////////////////////////////////////////////////////

PyObject* NS(ConfigSetBuffersize) (
  MqS_Obj *self,
  PyObject	*arg
)
{
  if (!PyLong_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetBuffersize' must be an integer");
    return NULL;
  }
  MqConfigSetBuffersize (&self->context, (MQ_INT) PyLong_AsLong(arg));
  Py_RETURN_NONE;

}

PyObject* NS(ConfigSetDebug) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyLong_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetDebug' must be an integer");
    return NULL;
  }
  MqConfigSetDebug (&self->context, (MQ_INT) PyLong_AsLong(arg));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetTimeout) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyLong_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetTimeout' must be an integer");
    return NULL;
  }
  MqConfigSetTimeout (&self->context, (MQ_WID) PyLong_AsLong(arg));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetName) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  MQ_STR str = PyO2C_START (&arg);
  if (PyErr_Occurred() != NULL) return NULL;
  MqConfigSetName (CONTEXT, str);
  PyO2C_STOP (&arg);
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetSrvName) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  MQ_STR str = PyO2C_START (&arg);
  if (PyErr_Occurred() != NULL) return NULL;
  MqConfigSetSrvName (CONTEXT, str);
  PyO2C_STOP (&arg);
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetIdent) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  MQ_STR str = PyO2C_START (&arg);
  if (PyErr_Occurred() != NULL) return NULL;
  MqConfigSetIdent (CONTEXT, str);
  PyO2C_STOP (&arg);
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetIoUdsFile) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  SETUP_context
  MQ_STR str = PyO2C_START (&arg);
  if (PyErr_Occurred() != NULL) return NULL;
  ErrorMqToPythonWithCheck(MqConfigSetIoUdsFile (context, str));
  PyO2C_STOP (&arg);
  SETUP_RETURN
}

PyObject* NS(ConfigSetIoTcp) (
  MqS_Obj    *self,
  PyObject    *args
)
{
  SETUP_context
  MQ_STR host, port, myhost, myport;
  if (!PyArg_ParseTuple(args, "zzzz:ConfigSetIoTcp", &host, &port, &myhost, &myport)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqConfigSetIoTcp (context, host, port, myhost, myport));
  SETUP_RETURN
}

PyObject* NS(ConfigSetIoPipeSocket) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  SETUP_context
  if (!PyLong_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetIoPipeSocket' must be an integer");
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqConfigSetIoPipeSocket (context, (MQ_INT) PyLong_AsLong(arg)));
  SETUP_RETURN
}

PyObject* NS(ConfigSetStartAs) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyLong_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetStartAs' must be an integer");
    return NULL;
  }
  MqConfigSetStartAs (&self->context, (MQ_INT) PyLong_AsLong(arg));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetIsSilent) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyBool_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetIsSilent' must be boolean");
    return NULL;
  }
  MqConfigSetIsSilent (&self->context, (arg == Py_True));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetIsServer) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyBool_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetIsServer' must be boolean");
    return NULL;
  }
  MqConfigSetIsServer (&self->context, (arg == Py_True));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetIsString) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyBool_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetIsString' must be boolean");
    return NULL;
  }
  MqConfigSetIsString (&self->context, (arg == Py_True));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetIgnoreExit) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyBool_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetIgnoreExit' must be boolean");
    return NULL;
  }
  MqConfigSetIgnoreExit (&self->context, (arg == Py_True));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetServerSetup) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyCallable_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetServerSetup' must be callable");
    return NULL;
  }
  Py_INCREF (arg);
  MqConfigSetServerSetup (CONTEXT,NS(ProcCall),arg,NS(ProcFree),NS(ProcCopy));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetServerCleanup) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyCallable_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetServerCleanup' must be callable");
    return NULL;
  }
  Py_INCREF (arg);
  MqConfigSetServerCleanup (CONTEXT,NS(ProcCall),arg,NS(ProcFree),NS(ProcCopy));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetBgError) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyCallable_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetBgError' must be callable");
    return NULL;
  }
  Py_INCREF (arg);
  MqConfigSetBgError (CONTEXT,NS(ProcCall),arg,NS(ProcFree),NS(ProcCopy));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetEvent) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyCallable_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetEvent' must be callable");
    return NULL;
  }
  Py_INCREF (arg);
  MqConfigSetEvent (CONTEXT,NS(ProcCall),arg,NS(ProcFree),NS(ProcCopy));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetFactory) (
  MqS_Obj   *self,
  PyObject  *args
)
{
  SETUP_FACTORY_ARG(ConfigSetFactory)
  MqConfigSetFactory(CONTEXT, ident,
    NS(FactoryCreate),	arg,  NS(ProcFree), NS(FactoryDelete),	NULL, NULL
  );
  Py_RETURN_NONE;
}

PyObject* NS(ConfigGetDebug) (
  PyObject    *self
)
{
  return PyLong_FromLong(ICONTEXT.config.debug);
}

PyObject* NS(ConfigGetTimeout) (
  PyObject    *self
)
{
  return PyLong_FromLong((long)MqConfigGetTimeout(&ICONTEXT));
}

PyObject* NS(ConfigGetBuffersize) (
  PyObject    *self
)
{
  return PyLong_FromLong(MqConfigGetBuffersize(&ICONTEXT));
}

PyObject* NS(ConfigGetIsString) (
  PyObject    *self
)
{
  if (ICONTEXT.config.isString == MQ_YES) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

PyObject* NS(ConfigGetIsServer) (
  PyObject    *self
)
{
  if (ICONTEXT.setup.isServer == MQ_YES) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

PyObject* NS(ConfigGetIsSilent) (
  PyObject    *self
)
{
  if (ICONTEXT.config.isSilent == MQ_YES) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

PyObject* NS(ConfigGetName) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetName(&ICONTEXT));
}

PyObject* NS(ConfigGetSrvName) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetSrvName(&ICONTEXT));
}

PyObject* NS(ConfigGetIdent) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetIdent(&ICONTEXT));
}

PyObject* NS(ConfigGetIoUdsFile) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetIoUdsFile(&ICONTEXT));
}

PyObject* NS(ConfigGetIoTcpHost) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetIoTcpHost(&ICONTEXT));
}

PyObject* NS(ConfigGetIoTcpPort) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetIoTcpPort(&ICONTEXT));
}

PyObject* NS(ConfigGetIoTcpMyHost) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetIoTcpMyHost(&ICONTEXT));
}

PyObject* NS(ConfigGetIoTcpMyPort) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetIoTcpMyPort(&ICONTEXT));
}

PyObject* NS(ConfigGetIoPipeSocket) (
  PyObject    *self
)
{
  return PyLong_FromLong(MqConfigGetIoPipeSocket(&ICONTEXT));
}

PyObject* NS(ConfigGetStartAs) (
  PyObject    *self
)
{
  return PyLong_FromLong(MqConfigGetStartAs(&ICONTEXT));
}

PyObject* NS(ConfigGetStatusIs) (
  PyObject    *self
)
{
  return PyLong_FromLong(MqConfigGetStatusIs(&ICONTEXT));
}

PyObject* NS(ConfigSetDaemon) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  SETUP_context
  MQ_STR str = PyO2C_START (&arg);
  if (PyErr_Occurred() != NULL) return NULL;
  ErrorMqToPythonWithCheck (MqConfigSetDaemon (CONTEXT, str));
  PyO2C_STOP (&arg);
  Py_RETURN_NONE;
error:
  return NULL;
}



