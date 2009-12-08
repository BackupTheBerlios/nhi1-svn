/**
 *  \file       theLink/pymsgque/slave_python.c
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

PyObject* NS(SlaveWorker)(
  MqS_Obj   *self,
  PyObject  *args
)
{
  PyObject *argsO = NULL;
  MQ_SIZE id;

  if (!PyArg_ParseTuple(args, "i|O!:SlaveWorker", &id, &PyList_Type, &argsO)) {
    return NULL;
  } else {
    SETUP_context 
    struct MqBufferLS * largv;
    MqErrorCheck (ListToMqBufferLS (context, argsO, &largv));
    SETUP_CHECK_RETURN (MqSlaveWorker(context, id, &largv));
  }
}

PyObject* NS(SlaveCreate) (
  PyObject  *self, 
  PyObject  *args
)
{
  int id;
  PyObject *slaveO;

  if (!PyArg_ParseTuple(args, "iO!:SlaveCreate", &id, &NS(MqS), &slaveO)) {
    return NULL;
  } else {
    SETUP_context;
    SETUP_CHECK_RETURN (MqSlaveCreate (context, id, &((MqS_Obj*)slaveO)->context));
  }
}

PyObject* NS(SlaveDelete) (
  PyObject  *self, 
  PyObject  *args
)
{
  int id;

  if (!PyArg_ParseTuple(args, "i:SlaveDelete", &id)) {
    return NULL;
  } else {
    SETUP_context;
    SETUP_CHECK_RETURN(MqSlaveDelete (context, id));
  }
}

PyObject* NS(SlaveGet) (
  PyObject  *self, 
  PyObject  *args
)
{
  PyObject * retO = Py_None;
  struct MqS * slave;
  int id;
  SETUP_context;

  if (!PyArg_ParseTuple(args, "i:SlaveGet", &id)) {
    return NULL;
  }

  slave = MqSlaveGet(context, id);

  if (slave != NULL) {
    PyObject *slaveO = ((PyObject *)slave->self);
    retO = slaveO;
  }

  Py_XINCREF(retO);
  return retO;
}

