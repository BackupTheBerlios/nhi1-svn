/**
 *  \file       theLink/pymsgque/MqBufferS_python.c
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

//////////////////////////////////////////////////////////////////////////////////
///
///				 Method definition
///
//////////////////////////////////////////////////////////////////////////////////

#define ErrorBufToPython() NS(pErrorFromMq)(self->buf->context)
#define ErrorBufToPythonWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorBufToPython(); \
    goto error; \
  }

#define GET(S,L,F,T) \
static PyObject* NS(Get ## S) ( \
  MqBufferS_Obj*  self  \
) \
{ \
  MQ_ ## L val; \
  ErrorBufToPythonWithCheck(MqBufferGet ## S (self->buf, &val)); \
  return F ( ( T ) val); \
error: \
  return NULL; \
}

GET(Y,BYT,PyLong_FromLong,long)
GET(S,SRT,PyLong_FromLong,long)
GET(I,INT,PyLong_FromLong,long)
GET(F,FLT,PyFloat_FromDouble,double)
GET(W,WID,PyLong_FromLongLong,PY_LONG_LONG)
GET(D,DBL,PyFloat_FromDouble,double)
GET(C,CST,PyC2O,const char*)

static PyObject* NS(GetO) (
  MqBufferS_Obj*  self 
)
{
  MQ_BOL i;
  ErrorBufToPythonWithCheck(MqBufferGetO(self->buf, &i));
  if (i == MQ_YES) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
error:
  return NULL;
}

static PyObject* NS(GetB) (
  MqBufferS_Obj*  self
)
{
  MQ_BIN b;
  MQ_SIZE len;
  ErrorBufToPythonWithCheck(MqBufferGetB(self->buf, &b, &len));
  return PyByteArray_FromStringAndSize((const char*)b, len);
error:
  return NULL;
}

static PyObject* NS(GetType) (
  MqBufferS_Obj*  self
)
{
  char str[2] = {MqBufferGetType(self->buf), '\0'};
  return PyC2O(str);
}

#define GetType_DOC	    "return the type of object saved in the buffer"
#define GetY_DOC	    "return a MQ_BYT object from a buffer"
#define GetO_DOC	    "return a MQ_BOL object from a buffer"
#define GetS_DOC	    "return a MQ_SRT object from a buffer"
#define GetI_DOC	    "return a MQ_INT object from a buffer"
#define GetF_DOC	    "return a MQ_FLT object from a buffer"
#define GetW_DOC	    "return a MQ_WID object from a buffer"
#define GetD_DOC	    "return a MQ_DBL object from a buffer"
#define GetB_DOC	    "return a MQ_BIN object from a buffer"
#define GetC_DOC	    "return a MQ_STR object from a buffer"

// Fill the Struct-Array

#define ARG(N,M) { #N , (PyCFunction) NS(N), M, N ## _DOC}
static PyMethodDef NS(MqBufferS_Methods)[] = {

    ARG(GetType,	METH_NOARGS),
    ARG(GetY,		METH_NOARGS),
    ARG(GetO,		METH_NOARGS),
    ARG(GetS,		METH_NOARGS),
    ARG(GetI,		METH_NOARGS),
    ARG(GetF,		METH_NOARGS),
    ARG(GetW,		METH_NOARGS),
    ARG(GetD,		METH_NOARGS),
    ARG(GetB,		METH_NOARGS),
    ARG(GetC,		METH_NOARGS),

    {NULL, NULL, 0, NULL}        /* Sentinel */
};
#undef ARG

//////////////////////////////////////////////////////////////////////////////////
///
///				 Object definition
///
//////////////////////////////////////////////////////////////////////////////////

#define MqBufferS_DOC	"libmsgque MqBufferS object"

static void
NS(MqBufferS_dealloc)(MqBufferS_Obj* self)
{
  Py_TYPE(self)->tp_free((PyObject*)self);
}

PyTypeObject NS(MqBufferS) = {
  {PyObject_HEAD_INIT(NULL)},
  "pymsgque.MqBufferS",		  /* tp_name */
  sizeof(MqBufferS_Obj),	  /* tp_basicsize */
  0,				  /* tp_itemsize */
  (destructor)NS(MqBufferS_dealloc),	  /* tp_dealloc */
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
  MqBufferS_DOC,		  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  NS(MqBufferS_Methods),	  /* tp_methods */
  0,				  /* tp_members */
  0,				  /* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  0,				  /* tp_init */
  0,				  /* tp_alloc */
  0,				  /* tp_new  OLD: NS(MqBufferS_new) */
};

//////////////////////////////////////////////////////////////////////////////////
///
///				 Public API
///
//////////////////////////////////////////////////////////////////////////////////

PyObject *
MqBufferS_Obj_From_PTR (
  MQ_BUF bufP
)
{
  MqBufferS_Obj *bufO = PyObject_New(MqBufferS_Obj, &NS(MqBufferS));
  bufO->buf = bufP;
  return (PyObject*)bufO;
}



