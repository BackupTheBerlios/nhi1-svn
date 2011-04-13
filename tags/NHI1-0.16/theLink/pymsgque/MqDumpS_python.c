/**
 *  \file       theLink/pymsgque/MqDumpS_python.c
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

#define MSGQUE msgque

//////////////////////////////////////////////////////////////////////////////////
///
///				 Method definition
///
//////////////////////////////////////////////////////////////////////////////////

static PyObject* NS(Size) (
  MqDumpS_Obj*  self
)
{
  return PyLong_FromLong(MqDumpSize(self->dump));
}

#define Size_DOC	    "return the size of the MqDumpS data"

// Fill the Struct-Array

#define ARG(N,M) { #N , (PyCFunction) NS(N), M, N ## _DOC}
static PyMethodDef NS(MqDumpS_Methods)[] = {

    ARG(Size, METH_NOARGS),

    {NULL, NULL, 0, NULL}        /* Sentinel */
};
#undef ARG

//////////////////////////////////////////////////////////////////////////////////
///
///				 Object definition
///
//////////////////////////////////////////////////////////////////////////////////

#define MqDumpS_DOC	"libmsgque MqDumpS object"

static void
NS(MqDumpS_dealloc)(MqDumpS_Obj* self)
{
  MqDumpDelete(&self->dump);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

PyTypeObject NS(MqDumpS) = {
  {PyObject_HEAD_INIT(NULL)},
  "pymsgque.MqDumpS",		  /* tp_name */
  sizeof(MqDumpS_Obj),	  /* tp_basicsize */
  0,				  /* tp_itemsize */
  (destructor)NS(MqDumpS_dealloc),	  /* tp_dealloc */
  0,				  /* tp_print */
  0,				  /* tp_getattr */
  0,				  /* tp_setattr */
  0,				  /* tp_reserved */
  0,				  /* tp_repr */
  0,				  /* tp_as_number */
  0,				  /* tp_as_sequence */
  0,				  /* tp_as_mapping */
  0,				  /* tp_hash  */
  0,				  /* tp_call */
  0,				  /* tp_str */
  0,				  /* tp_getattro */
  0,				  /* tp_setattro */
  0,				  /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT, 		  /* tp_flags */
  MqDumpS_DOC,			  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  NS(MqDumpS_Methods),		  /* tp_methods */
  0,				  /* tp_members */
  0,				  /* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  0,				  /* tp_init */
  0,				  /* tp_alloc */
  0,				  /* tp_new  OLD: NS(MqDumpS_new) */
};

//////////////////////////////////////////////////////////////////////////////////
///
///				 Public API
///
//////////////////////////////////////////////////////////////////////////////////

PyObject *
MqDumpS_New (
  struct MqDumpS *dump
)
{
  MqDumpS_Obj *dumpO = PyObject_New(MqDumpS_Obj, &NS(MqDumpS));
  dumpO->dump = dump;
  return (PyObject*)dumpO;
}
