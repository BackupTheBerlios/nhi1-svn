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

#define SET(S,L,F,T) \
static PyObject* NS(Set ## S) ( \
  MqBufferS_Obj*  self, \
  PyObject	  *args \
) \
{ \
  T val; \
  if (!PyArg_ParseTuple(args, MQ_CPPSTR(F) ":Set" MQ_CPPSTR(S), &val)) { \
    return NULL; \
  } \
  MqBufferSet ## S(self->buf, (MQ_ ## L) val); \
  return (PyObject*) self; \
}

SET(Y,BYT,h,short int)
SET(S,SRT,h,short int)
SET(I,INT,i,int)
SET(F,FLT,f,float)
SET(W,WID,L,PY_LONG_LONG)
SET(D,DBL,d,double)
SET(C,CST,s,const char *)

PyObject* NS(SetO) (
  MqBufferS_Obj	*self,
  PyObject	*args
)
{
  PyObject *O;
  if (!PyArg_ParseTuple(args, "O!:SetO", &PyBool_Type, &O)) {
    return NULL;
  }
  MqBufferSetO(self->buf, (MQ_BOL) (O == Py_True ? MQ_YES : MQ_NO));
  return (PyObject*) self;
}

PyObject* NS(SetB) (
  MqBufferS_Obj	*self,
  PyObject	*args
)
{
  PyObject *o;
  if (!PyArg_ParseTuple(args, "O!:SetB", &PyByteArray_Type, &o)) {
    return NULL;
  }
  MqBufferSetB(self->buf,(MQ_BIN)PyByteArray_AS_STRING(o),(MQ_SIZE)PyByteArray_GET_SIZE(o));
  return (PyObject*) self;
}

static PyObject* NS(AppendC) (
  MqBufferS_Obj*  self,
  PyObject	  *args
)
{
  MQ_CST val;
  if (!PyArg_ParseTuple(args, "s:AppendC", &val)) {
    return NULL;
  }
  MqBufferAppendC(self->buf, (MQ_CST) val);
  return (PyObject*) self;
}

static PyObject* NS(GetType) (
  MqBufferS_Obj*  self
)
{
  char str[2] = {MqBufferGetType(self->buf), '\0'};
  return PyC2O(str);
}

static PyObject* NS(Dup) (
  MqBufferS_Obj*  self
)
{
  return MqBufferS_New(MqBufferDup(self->buf));
}

static PyObject* NS(Delete) (
  MqBufferS_Obj*  self
)
{
  MqBufferDelete(&self->buf);
  Py_RETURN_NONE;
}

#define Dup_DOC		    "return the duplicate of the MqBufferS object"
#define Delete_DOC	    "Delete the MqBufferS object created with 'Dup'"
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

#define SetY_DOC	    "set a MQ_BYT object into the buffer and return the buffer"
#define SetO_DOC	    "set a MQ_BOL object into the buffer and return the buffer"
#define SetS_DOC	    "set a MQ_SRT object into the buffer and return the buffer"
#define SetI_DOC	    "set a MQ_INT object into the buffer and return the buffer"
#define SetF_DOC	    "set a MQ_FLT object into the buffer and return the buffer"
#define SetW_DOC	    "set a MQ_WID object into the buffer and return the buffer"
#define SetD_DOC	    "set a MQ_DBL object into the buffer and return the buffer"
#define SetB_DOC	    "set a MQ_BIN object into the buffer and return the buffer"
#define SetC_DOC	    "set a MQ_STR object into the buffer and return the buffer"
#define AppendC_DOC	    "append a MQ_STR object to the buffer and return the buffer"

// Fill the Struct-Array

#define ARG(N,M) { #N , (PyCFunction) NS(N), M, N ## _DOC}
static PyMethodDef NS(MqBufferS_Methods)[] = {

    ARG(Dup,		METH_NOARGS),
    ARG(Delete,		METH_NOARGS),
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

    ARG(SetY,		METH_VARARGS),
    ARG(SetO,		METH_VARARGS),
    ARG(SetS,		METH_VARARGS),
    ARG(SetI,		METH_VARARGS),
    ARG(SetF,		METH_VARARGS),
    ARG(SetW,		METH_VARARGS),
    ARG(SetD,		METH_VARARGS),
    ARG(SetB,		METH_VARARGS),
    ARG(SetC,		METH_VARARGS),

    ARG(AppendC,	METH_VARARGS),

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
printP(self)
  Py_TYPE(self)->tp_free((PyObject*)self);
}

PyTypeObject NS(MqBufferS) = {
  {PyObject_HEAD_INIT(NULL)},
  "pymsgque.MqBufferS",		  /* tp_name */

  // mystic +1 to avoid an memory error in "buffer-[45]-*"
  // Problem: the return pointer in PyObject_New called by MqBufferS_New for two
  // calls !! overlap !! -> difference smaller then "tp_basicsize" -> mystic.
  sizeof(MqBufferS_Obj)+1,	  /* tp_basicsize */

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
MqBufferS_New (
  MQ_BUF bufP
)
{
  MqBufferS_Obj *bufO = PyObject_New(MqBufferS_Obj, &NS(MqBufferS));
  bufO->buf = bufP;
  return (PyObject*)bufO;
}

