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

static enum MqErrorE
NS(FactoryCreate) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR data,
  struct MqS  ** contextP
)
{
  if (create == MQ_FACTORY_NEW_FORK) PyOS_AfterFork();
  {
    struct MqS * const context = *contextP = 
	&((MqS_Obj*) PyObject_CallObject((PyObject*)data, NULL))->context;
    if (context == NULL) {
      NS(ErrorSet) ((PyObject*)tmpl->self);
      return MqErrorStack(tmpl);
    } else {
      MqConfigDup(context, tmpl);
      MqErrorCheck(MqSetupDup(context, tmpl));
      return MQ_OK;
error:
      return MqErrorStack(context);
    }
  }
}

static void
NS(FactoryDelete)(
  struct MqS * context,
  MQ_BOL doFactoryDelete,
  MQ_PTR data
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

PyObject* NS(ConfigSetIoUds) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  SETUP_context
  MQ_STR str = PyO2C_START (&arg);
  if (PyErr_Occurred() != NULL) return NULL;
  ErrorMqToPythonWithCheck(MqConfigSetIoUds (context, str));
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

PyObject* NS(ConfigSetIoPipe) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  SETUP_context
  if (!PyLong_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetIoPipe' must be an integer");
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqConfigSetIoPipe (context, (MQ_INT) PyLong_AsLong(arg)));
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

PyObject* NS(ConfigSetFilterFTR) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyCallable_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetFilterFTR' must be callable");
    return NULL;
  }
  Py_INCREF (arg);
  MqConfigSetFilterFTR (CONTEXT,NS(ProcCall),arg,NS(ProcFree),NS(ProcCopy));
  Py_RETURN_NONE;
}

PyObject* NS(ConfigSetFilterEOF) (
  MqS_Obj    *self,
  PyObject    *arg
)
{
  if (!PyCallable_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetFilterEOF' must be callable");
    return NULL;
  }
  Py_INCREF (arg);
  MqConfigSetFilterEOF (CONTEXT,NS(ProcCall),arg,NS(ProcFree),NS(ProcCopy));
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

PyObject* NS(ConfigSetFactory) (
  MqS_Obj *self,
  PyObject	*arg
)
{
  if (!PyCallable_Check(arg)) {
    PyErr_SetString(PyExc_TypeError, "parameter for 'ConfigSetFactory' must be callable");
    return NULL;
  }
  Py_INCREF (arg);
  MqConfigSetFactory(CONTEXT,
    NS(FactoryCreate),	arg,  NS(ProcFree), NS(ProcCopy),
    NS(FactoryDelete),	NULL, NULL,	    NULL
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

PyObject* NS(ConfigGetIsParent) (
  PyObject    *self
)
{
  if (ICONTEXT.config.parent == NULL) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

PyObject* NS(ConfigGetIsSlave) (
  PyObject    *self
)
{
  if (ICONTEXT.config.master == NULL) {
    Py_RETURN_FALSE;
  } else {
    Py_RETURN_TRUE;
  }
}

PyObject* NS(ConfigGetIsConnected) (
  MqS_Obj   *self
)
{
  if (ICONTEXT.link.onCreate == MQ_NO) {
    Py_RETURN_FALSE;
  } else {
    Py_RETURN_TRUE;
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

PyObject* NS(ConfigGetParent) (
  PyObject    *self,
  PyObject    *args
)
{
  if (ICONTEXT.config.parent == NULL) {
    Py_RETURN_NONE;
  } else {
    PyObject *parent = ((PyObject *)ICONTEXT.config.parent->self);
    Py_INCREF(parent);
    return parent;
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

PyObject* NS(ConfigGetCtxId) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyLong_FromLong(MqConfigGetCtxId(CONTEXT));
}

PyObject* NS(ConfigGetToken) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyC2O(MqConfigGetToken(CONTEXT));
}

PyObject* NS(ConfigGetMaster) (
  PyObject  *self 
)
{
  if (ICONTEXT.config.master == NULL) {
    Py_RETURN_NONE;
  } else {
    PyObject *masterO = ((PyObject *)ICONTEXT.config.master->self);
    Py_INCREF(masterO);
    return masterO;
  }
}


