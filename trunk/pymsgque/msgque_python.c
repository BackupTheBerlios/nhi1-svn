/**
 *  \file       pymsgque/msgque_python.c
 *  \brief      \$Id: msgque_python.c 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_python.h"

#define MSGQUE msgque

PyObject * NS(MqSException) = NULL;

extern PyTypeObject NS(MqS);
extern PyTypeObject NS(MqBufferS);

#define Init_DOC		    "[list] initialize the arguments prefix"

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

#define ARG(N,M) { #N , (PyCFunction) NS(N), M, N ## _DOC}
static PyMethodDef NS(Methods)[] = {
    ARG(Init,	METH_O),
    {NULL, NULL, 0, NULL}        /* Sentinel */
};
#undef ARG

static PyModuleDef NS(Module) = {
  PyModuleDef_HEAD_INIT,
  "pymsgque",
  "python language binding for: http://libmsgque.sourceforge.net/",
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

  // init libmsgque global data
  if (MqInitBuf == NULL && Py_GetProgramName() != NULL) {
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

  return m;

error:
  return NULL;
}

