/**
 *  \file       theLink/pymsgque/storage_python.c
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_python.h"

//extern PyTypeObject NS(MqS);

PyObject* NS(StorageOpen) (
  PyObject	*self,
  PyObject	*args
)
{
  SETUP_context
  const char *c;
  if (!PyArg_ParseTuple(args, "s:StorageOpen", &c)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqStorageOpen(context,c));
  SETUP_RETURN
}

PyObject* NS(StorageClose) (
  PyObject	*self
)
{
  SETUP_context
  ErrorMqToPythonWithCheck (MqStorageClose(context));
  SETUP_RETURN
}

PyObject* NS(StorageInsert) (
  PyObject	*self
)
{
  MQ_TRA transLId = 0L;
  SETUP_context
  ErrorMqToPythonWithCheck (MqStorageInsert(context,&transLId));
  return PyLong_FromLongLong((PY_LONG_LONG)transLId);
error:
  return NULL; 
}

PyObject* NS(StorageSelect) (
  PyObject	*self,
  PyObject	*args
)
{
  MQ_TRA transLId = 0L;
  SETUP_context
  if (!PyArg_ParseTuple(args, "|L:StorageSelect", &transLId)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqStorageSelect(context,&transLId));
  return PyLong_FromLongLong((PY_LONG_LONG)transLId);
error:
  return NULL; 
}

PyObject* NS(StorageDelete) (
  PyObject	*self,
  PyObject	*args
)
{
  MQ_TRA transLId = 0L;
  SETUP_context
  if (!PyArg_ParseTuple(args, "L:StorageDelete", &transLId)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqStorageDelete(context,transLId));
  return PyLong_FromLongLong((PY_LONG_LONG)transLId);
error:
  return NULL; 
}

PyObject* NS(StorageCount) (
  PyObject	*self
)
{
  MQ_TRA transLId = 0L;
  SETUP_context
  ErrorMqToPythonWithCheck (MqStorageCount(context,&transLId));
  return PyLong_FromLongLong((PY_LONG_LONG)transLId);
error:
  return NULL; 
}
