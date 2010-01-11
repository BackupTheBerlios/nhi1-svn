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
  if (ICONTEXT.link.onCreate == MQ_NO) {
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

