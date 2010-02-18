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
  PyObject    *args,
  PyObject    *keywds
)
{
  int timeout=MQ_TIMEOUT_USER;
  const char *wait="NO";
  static char *kwlist[] = {"timeout", "wait", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, keywds, "|is:ProcessEvent", kwlist,
	&timeout, &wait)) { 
    return NULL;
  } else {
    SETUP_context
    enum MqWaitOnEventE flags = MQ_WAIT_NO;
    if (!strncmp(wait,"NO",2)) {
      flags = MQ_WAIT_NO;
    } else if (!strncmp(wait,"ONCE",4)) {
      flags = MQ_WAIT_ONCE;
    } else if (!strncmp(wait,"FOREVER",7)) {
      flags = MQ_WAIT_FOREVER;
    } else {
      PyErr_SetString(PyExc_TypeError, "wait=(NO|ONCE|FOREVER)");
      return NULL;
    }
    SETUP_CHECK_RETURN(MqProcessEvent(context, timeout, flags));
  }
}

PyObject* NS(Delete) (
  PyObject    *self
)
{
  MqContextFree (CONTEXT);
  Py_DECREF(self);
  Py_RETURN_NONE;
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
