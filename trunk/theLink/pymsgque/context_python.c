/**
 *  \file       theLink/pymsgque/context_python.c
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
///				    Start
///
//////////////////////////////////////////////////////////////////////////////////

void
NS(ProcessExit) (
  int num
)
{
//MqDLogX(context,__func__,0,"self<%p>, refCount<%li>\n", context->self, ((PyObject*)context->self)->ob_refcnt);
  Py_Exit(num);
}

PyObject* NS(ProcessEvent) (
  PyObject    *self,
  PyObject    *args
)
{
  SETUP_context
  int timeout=MQ_TIMEOUT_DEFAULT;
  int wait=MQ_WAIT_NO;
  int val1, val2;
  const int size = (int) PyTuple_Size(args);

  if (!PyArg_ParseTuple(args, "|ii:ProcessEvent", &val1, &val2)) { 
    return NULL;
  }

  if (size == 2) {
    timeout = val1;
    wait = val2;
  } else if (size == 1) {
    wait = val1;
  }
  
  SETUP_CHECK_RETURN(MqProcessEvent(context, timeout, wait));
}

PyObject* NS(Delete) (
  PyObject    *self
)
{
  MqContextFree (CONTEXT);
  Py_DECREF(self);
  Py_RETURN_NONE;
}

PyObject* NS(ContextGetBuffer)(
  PyObject  *self
)
{
  return MqBufferS_New(MqContextGetBuffer(CONTEXT));
}

PyObject* NS(Exit)(
  PyObject  *self
)
{
  MqExit(CONTEXT);
  Py_RETURN_NONE;
}

PyObject* NS(LogC)(
  PyObject  *self,
  PyObject  *args
)
{
  MQ_CST prefix, text;
  MQ_INT level;
  if (!PyArg_ParseTuple(args, "sis:LogC", &prefix, &level, &text)) {
    return NULL;
  } else {
    MqLogC(CONTEXT,prefix,level,text);
  }
  Py_RETURN_NONE;
}
