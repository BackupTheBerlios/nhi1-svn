/**
 *  \file       theLink/pymsgque/msgque_python.c
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

#define MSGQUE msgque

PyObject * NS(MqSException) = NULL;

extern PyTypeObject NS(MqS);
extern PyTypeObject NS(MqBufferS);

// ************************************************************

#define Init_DOC  "[list] initialize the arguments prefix"

static PyObject* NS(Init) (
  PyObject    *class,
  PyObject    *list
)
{
  Py_ssize_t len;
  struct MqBufferLS * initB = MqInitCreate();
  PyObject *item, *utf8;
  Py_ssize_t i;
  if (!PyList_Check(list)) goto error;
  len = PyList_GET_SIZE(list);
  for (i=0; i<len; i++) {
    if ((item = PyList_GetItem(list, i)) == NULL) goto error;
    if ((utf8 = PyUnicode_AsUTF8String(item)) == NULL) goto error;
    MqBufferLAppendC(initB, PyBytes_AsString(utf8));
    Py_DECREF(utf8);
    Py_DECREF(list);
  }

  SETUP_RETURN
}

// ************************************************************

#define FactoryAdd_DOC  "[ident,callback] add factory callback"

static PyObject* NS(FactoryAdd) (
  PyObject    *class,
  PyObject    *args
)
{
  enum MqErrorE ret;
  MQ_CST ident;
  PyObject *arg;
  if (PyTuple_GET_SIZE(args) == 1) {
    if (!PyArg_ParseTuple(args, "O!:FactoryAdd", &PyType_Type, &arg)) {
      return NULL;
    }
    ident = ((PyTypeObject*)arg)->tp_name;
  } else {
    if (!PyArg_ParseTuple(args, "sO!:FactoryAdd", &ident, &PyType_Type, &arg)) {
      return NULL;
    }
  }
  Py_INCREF (arg);
  ret = MqFactoryAdd(ident, NS(FactoryCreate), arg, NS(ProcFree), NS(FactoryDelete), NULL, NULL);
  if (MqErrorCheckI(ret)) {
    PyErr_SetString(PyExc_RuntimeError, "unable to add factory");
    return NULL;
  }
  Py_RETURN_NONE;
}

#define FactoryNew_DOC  "[ident,callback] add and call factory callback"

static PyObject* NS(FactoryNew) (
  PyObject    *class,
  PyObject    *args
)
{
  struct MqS *mqctx;
  MQ_CST ident;
  PyObject *arg;
  if (PyTuple_GET_SIZE(args) == 1) {
    if (!PyArg_ParseTuple(args, "O!:FactoryNew", &PyType_Type, &arg)) {
      return NULL;
    }
    ident = ((PyTypeObject*)arg)->tp_name;
  } else {
    if (!PyArg_ParseTuple(args, "sO!:FactoryNew", &ident, &PyType_Type, &arg)) {
      return NULL;
    }
  }
  Py_INCREF (arg);
  Py_INCREF (arg);
  mqctx = MqFactoryNew(ident, NS(FactoryCreate), arg, NS(ProcFree), NS(FactoryDelete), NULL, NULL);
  if (mqctx == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to add and call factory");
    return NULL;
  }
  arg = ((PyObject *)SELFX(mqctx));
  Py_INCREF(arg);
  return arg;
}

#define FactoryCall_DOC  "[ident] call factory callback"

static PyObject* NS(FactoryCall) (
  PyObject    *class,
  PyObject    *arg
)
{
  struct MqS * mqctx;
  MQ_STR ident = PyO2C_START (&arg);
  if (PyErr_Occurred() != NULL) return NULL;
  mqctx = MqFactoryCall(ident);
  if (mqctx == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to call factory");
    return NULL;
  }
  arg = ((PyObject *)SELFX(mqctx));
  Py_INCREF(arg);
  return arg;
}

#define ARG(N,M) { #N , (PyCFunction) NS(N), M, N ## _DOC}
static PyMethodDef NS(Methods)[] = {
    ARG(Init,	      METH_O),
    ARG(FactoryAdd,   METH_VARARGS),
    ARG(FactoryNew,   METH_VARARGS),
    ARG(FactoryCall,  METH_O),
    {NULL, NULL, 0, NULL}        /* Sentinel */
};
#undef ARG

static PyModuleDef NS(Module) = {
  PyModuleDef_HEAD_INIT,
  "pymsgque",
  "python language binding for: http://developer.berlios.de/projects/nhi1/",
  -1,
  NS(Methods), 
  NULL, NULL, NULL, NULL
};

__attribute__ ((visibility("default"))) PyMODINIT_FUNC
PyInit_pymsgque(void) 
{
  PyObject* m, *dict;

  // get 'MqS'
  NS(MqS).ob_base.ob_base.ob_type = &PyType_Type;
  PyErrorCheck (PyType_Ready(&NS(MqS)));

  // get 'MqBufferS'
  NS(MqBufferS).ob_base.ob_base.ob_type = &PyType_Type;
  PyErrorCheck (PyType_Ready(&NS(MqBufferS)));

  // get 'MqSException'
  dict = PyDict_New();
  PyDict_SetItemString (dict, "text", Py_None);
  PyDict_SetItemString (dict, "num", Py_None);
  PyDict_SetItemString (dict, "code", Py_None);
  
  PyErrorCheckN(NS(MqSException) = PyErr_NewException("pymsgque.MqSException", NULL, dict));

  PyErrorCheckN(m = PyModule_Create(&NS(Module)));

  Py_INCREF(&NS(MqS));
  Py_INCREF(&NS(MqBufferS));
  Py_INCREF(NS(MqSException));
  PyErrorCheck(PyModule_AddObject(m, "MqS",		(PyObject *)&NS(MqS)));
  PyErrorCheck(PyModule_AddObject(m, "MqBufferS",	(PyObject *)&NS(MqBufferS)));
  PyErrorCheck(PyModule_AddObject(m, "MqSException",	NS(MqSException)));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_TIMEOUT_DEFAULT",  -1));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_TIMEOUT_USER",     -2));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_TIMEOUT_MAX",      -3));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_WAIT_NO",	  0));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_WAIT_ONCE",	  1));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_WAIT_FOREVER",     2));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_START_DEFAULT",    0));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_START_FORK",	  1));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_START_THREAD",     2));
  PyErrorCheck(PyModule_AddIntConstant(m, "MqS_START_SPAWN",      3));

  // init libmsgque global data
  if (MqInitGet() == NULL && Py_GetProgramName() != NULL) {
    struct MqBufferLS * initB = MqInitCreate();
    char * buf1 = MqSysMalloc(MQ_ERROR_IGNORE, 250);
    PyObject *sys, *list, *item, *utf8;
    if (buf1 == NULL) return PyErr_NoMemory();
    if (wcstombs(buf1, Py_GetProgramName(), 250) != -1)
      MqBufferLAppendC(initB, buf1);
    if ((sys = PyImport_ImportModule("sys")) == NULL) return NULL;
    if ((list = PyObject_GetAttrString(sys, "argv")) == NULL) return NULL;
    if ((item = PyList_GetItem(list, 0)) == NULL) return NULL;
    if ((utf8 = PyUnicode_AsUTF8String(item)) == NULL) return NULL;
    MqBufferLAppendC(initB, PyBytes_AsString(utf8));
    Py_DECREF(utf8);
    Py_DECREF(list);
    Py_DECREF(sys);
  }

  // init factory
  MqFactoryDefault("pymsgque", NS(FactoryCreate), (MQ_PTR)&NS(MqS), NULL, NS(FactoryDelete), NULL, NULL);

  return m;

error:
  return NULL;
}

