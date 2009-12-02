/**
 *  \file       pymsgque/context_python.c
 *  \brief      \$Id: context_python.c 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_python.h"

#define MQ_CONTEXT_S context

extern PyTypeObject NS(MqS);
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
  int timeout=-1;
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

PyObject* NS(Exit)(
  PyObject  *self
)
{
  MqExit(CONTEXT);
  Py_RETURN_NONE;
}
