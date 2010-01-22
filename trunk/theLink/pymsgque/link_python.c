/**
 *  \file       theLink/pymsgque/link_python.c
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

PyObject* NS(LinkIsParent) (
  PyObject    *self
)
{
  if (ICONTEXT.config.parent == NULL) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

PyObject* NS(LinkIsConnected) (
  MqS_Obj   *self
)
{
  if (MqLinkIsConnected(CONTEXT)) {
    Py_RETURN_FALSE;
  } else {
    Py_RETURN_TRUE;
  }
}

PyObject* NS(LinkGetParent) (
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

PyObject* NS(LinkGetCtxId) (
  PyObject    *self,
  PyObject    *args
)
{
  return PyLong_FromLong(MqLinkGetCtxId(CONTEXT));
}

PyObject* NS(LinkCreate)(
  MqS_Obj   *self, 
  PyObject  *args
)
{
  PyObject *argsO = NULL;

  if (!PyArg_ParseTuple(args, "|O!:LinkCreate", &PyList_Type, &argsO)) {
    return NULL;
  } else {
    SETUP_context 
    struct MqBufferLS * largv = NULL;
    MqErrorCheck (ListToMqBufferLS (context, argsO, &largv));
    SETUP_CHECK_RETURN (MqLinkCreate(context, &largv));
  }
}

PyObject* NS(LinkCreateChild)(
  MqS_Obj   *self, 
  PyObject  *args
)
{
  MqS_Obj *parentO = NULL;
  PyObject *argsO = NULL;

  if (!PyArg_ParseTuple(args, "O!|O!:LinkCreateChild", 
	&NS(MqS),		&parentO,
	&PyList_Type,		&argsO
    )) {
    return NULL;
  } else {
    SETUP_context
    struct MqS * const parent = &parentO->context;
    struct MqBufferLS * largv = NULL;

    MqErrorCheck (MqSetupDup(context, parent));
    MqErrorCheck (ListToMqBufferLS (context, argsO, &largv));
    SETUP_CHECK_RETURN (MqLinkCreateChild(context, parent, &largv));
  }
}

PyObject* NS(LinkDelete)(
  PyObject  *self 
)
{
  MqLinkDelete (CONTEXT);
  Py_RETURN_NONE;
}

PyObject* NS(LinkConnect)(
  PyObject  *self 
)
{
  SETUP_context
  SETUP_CHECK_RETURN (MqLinkConnect (CONTEXT));
}
