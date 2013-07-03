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
extern PyTypeObject NS(MqFactoryS);
extern PyTypeObject NS(MqDumpS);

// ************************************************************

#define Init_DOC  "[list] initialize the arguments prefix"

static PyObject* NS(Init) (
  PyObject    *class,
  PyObject    *list
)
{
  Py_ssize_t len;
  struct MqBufferLS * initB = MqInitArg0();
  PyObject *item, *utf8;
  Py_ssize_t i;
  if (!PyList_Check(list)) goto error;
  len = PyList_GET_SIZE(list);
  for (i=0; i<len; i++) {
    if ((item = PyList_GetItem(list, i)) == NULL) goto error;
    if ((utf8 = PyUnicode_AsUTF8String(item)) == NULL) goto error;
    MqBufferLAppendC(initB, PyBytes_AsString(utf8));
    Py_DECREF(utf8);
  }
  Py_DECREF(list);

  SETUP_RETURN
}

// ************************************************************

#define FactoryAdd_DOC  "[?ident?,class] add a factory class and return the MqFactoryS instance"

PyObject* NS(FactoryAdd) ( PyObject    *class, PyObject    *args);

#define FactoryDefault_DOC  "[?ident?,class] add a default factory class and return the MqFactoryS instance"

PyObject* NS(FactoryDefault) ( PyObject    *class, PyObject    *args);

#define FactoryDefaultIdent_DOC  "[] get the identifier of the default factory"

PyObject* NS(FactoryDefaultIdent) ( PyObject    *class, PyObject    *args);

#define FactoryGet_DOC  "[string] get the MqFactoryS instance from the factory-identifier"

PyObject* NS(FactoryGet) ( PyObject    *class, PyObject   *args);

#define FactoryGetCalled_DOC  "[string] get the MqFactoryS instance (in called mode) from the factory-identifier"

PyObject* NS(FactoryGetCalled) ( PyObject    *class, PyObject   *args);

// ************************************************************

#define ARG(N,M) { #N , (PyCFunction) NS(N), M, N ## _DOC}
static PyMethodDef NS(Methods)[] = {
    ARG(Init,		      METH_O),
    ARG(FactoryAdd,	      METH_VARARGS),
    ARG(FactoryDefault,	      METH_VARARGS),
    ARG(FactoryDefaultIdent,  METH_NOARGS),
    ARG(FactoryGet,	      METH_VARARGS),
    ARG(FactoryGetCalled,     METH_VARARGS),
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

  // get 'MqFactoryS'
  NS(MqFactoryS).ob_base.ob_base.ob_type = &PyType_Type;
  PyErrorCheck (PyType_Ready(&NS(MqFactoryS)));

  // get 'MqDumpS'
  NS(MqDumpS).ob_base.ob_base.ob_type = &PyType_Type;
  PyErrorCheck (PyType_Ready(&NS(MqDumpS)));

  // get 'MqSException'
  dict = PyDict_New();
  PyDict_SetItemString (dict, "text", Py_None);
  PyDict_SetItemString (dict, "num", Py_None);
  PyDict_SetItemString (dict, "code", Py_None);
  
  PyErrorCheckN(NS(MqSException) = PyErr_NewException("pymsgque.MqSException", NULL, dict));

  PyErrorCheckN(m = PyModule_Create(&NS(Module)));

  Py_INCREF(&NS(MqS));
  Py_INCREF(&NS(MqBufferS));
  Py_INCREF(&NS(MqFactoryS));
  Py_INCREF(&NS(MqDumpS));
  Py_INCREF(NS(MqSException));
  PyErrorCheck(PyModule_AddObject(m, "MqS",		(PyObject *)&NS(MqS)));
  PyErrorCheck(PyModule_AddObject(m, "MqBufferS",	(PyObject *)&NS(MqBufferS)));
  PyErrorCheck(PyModule_AddObject(m, "MqFactoryS",	(PyObject *)&NS(MqFactoryS)));
  PyErrorCheck(PyModule_AddObject(m, "MqDumpS",		(PyObject *)&NS(MqDumpS)));
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
  if (MqInitGetArg0() == NULL && Py_GetProgramName() != NULL) {
    struct MqBufferLS * initB = MqInitArg0();
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
  if (!strcmp(MqFactoryDefaultIdent(), "libmsgque"))
    if (MqFactoryDefault(MQ_ERROR_PRINT, "pymsgque", 
	  NS(FactoryCreate), (MQ_PTR)&NS(MqS), NULL, NULL, NS(FactoryDelete), NULL, NULL, NULL) == NULL) {
      PyErr_SetString(PyExc_RuntimeError, "MqFactoryS exception");
      return NULL;
    }
  return m;
error:
  return NULL;
}



