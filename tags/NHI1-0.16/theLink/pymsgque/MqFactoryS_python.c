/**
 *  \file       theLink/pymsgque/MqFactoryS_python.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_python.h"

extern PyTypeObject NS(MqS);

enum MqErrorE
NS(FactoryCreate) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS *item,
  struct MqS ** contextP
)
{
  struct MqS * mqctx;
  PyObject *args, *result;
  PyTypeObject* type = (PyTypeObject*) item->Create.data;
  PyObject *tmplO = NULL;

  if (create == MQ_FACTORY_NEW_FORK) PyOS_AfterFork();

  // setup args
  if (create == MQ_FACTORY_NEW_INIT) {
    args = PyTuple_New(0);
  } else {
    args = PyTuple_New(1);
    tmplO = (PyObject*)SELFX(tmpl);
    PyTuple_SET_ITEM(args, 0, tmplO);
    Py_INCREF(tmplO);
  }

  // call constructor
  result = PyObject_Call((PyObject*)type, args, NULL);
  Py_DECREF(args);

  // call factory object 
  if (result == NULL || PyErr_Occurred() != NULL) goto error1;

  // right type
  if (PyObject_IsInstance(result, (PyObject*) &NS(MqS))) {
    mqctx = &((MqS_Obj*)result)->context;
  } else {
    goto error1;
  }

  // check for MQ error
  MqErrorCheck(MqErrorGetCode(mqctx));

  // copy and initialize "setup" data
  if (create != MQ_FACTORY_NEW_INIT) {
    MqSetupDup(mqctx, tmpl);
  }

  *contextP = mqctx;
  return MQ_OK;

error:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    MqErrorCopy (tmpl, mqctx);
    MqContextDelete (&mqctx);
    return MqErrorStack(tmpl);
  } else {
    return MQ_ERROR;
  }

error1:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    NS(ErrorSet) ((PyObject*)tmpl->self);
    return MqErrorGetCode(tmpl);
  } else {
    return MQ_ERROR;
  }

/*
error2:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    return MqErrorC(tmpl, __func__, -1, "Factory return no MqS type");
  } else {
    return MQ_ERROR;
  }
*/

}

void MQ_DECL
NS(FactoryDelete)(
  struct MqS * context,
  MQ_BOL doFactoryDelete,
  struct MqFactoryS * const data
)
{
  SETUP_self
  MqContextFree (context);
  Py_DECREF (self);
}

static void MQ_DECL 
NS(FactoryFree) (
  MQ_PTR *dataP
)
{
  Py_DECREF(*dataP);
  *dataP = NULL;
}

static void MQ_DECL 
NS(FactoryCopy) (
  MQ_PTR *dataP
)
{
  Py_INCREF(*dataP);
}

//////////////////////////////////////////////////////////////////////////////////
///
///				 static
///
//////////////////////////////////////////////////////////////////////////////////

PyObject* NS(FactoryAdd) (
  PyObject    *class,
  PyObject    *args
)
{
  SETUP_FACTORY_ARG(FactoryAdd)
  return MqFactoryS_New(
    MqFactoryAdd(MQ_ERROR_PRINT, ident, 
      NS(FactoryCreate), arg, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL)
  );
}

PyObject* NS(FactoryDefault) (
  PyObject    *class,
  PyObject    *args
)
{
  MQ_CST ident;
  PyObject *arg;
  if (PyTuple_GET_SIZE(args) == 1) {
    if (!PyArg_ParseTuple(args, "s:FactoryDefault", &ident)) {
      return NULL;
    }
    arg = (PyObject*) &NS(MqS);
  } else {
    if (!PyArg_ParseTuple(args, "sO!:FactoryDefault", &ident, &PyType_Type, &arg)) {
      return NULL;
    }
  }
  Py_INCREF (arg);
  return MqFactoryS_New (
    MqFactoryDefault(MQ_ERROR_PRINT, ident, 
      NS(FactoryCreate), arg, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL)
  );
}

PyObject* NS(FactoryDefaultIdent) (
  PyObject    *self
)
{
  return PyC2O(MqFactoryDefaultIdent());
}

PyObject* NS(FactoryGet) (
  PyObject    *class,
  PyObject    *args
)
{
  MQ_CST ident=NULL;
  if (!PyArg_ParseTuple(args, "|s:FactoryGet", &ident)) {
    return NULL;
  }
  return MqFactoryS_New (MqFactoryGet (ident));
}

PyObject* NS(FactoryGetCalled) (
  PyObject    *class,
  PyObject    *args
)
{
  MQ_CST ident = NULL;
  if (!PyArg_ParseTuple(args, "|s:FactoryGetCalled", &ident)) {
    return NULL;
  }
  return MqFactoryS_New (MqFactoryGetCalled (ident));
}

static PyObject* NS(New) (
  MqFactoryS_Obj    *self
)
{
  struct MqS *mqctx = MqFactoryNew(MQ_ERROR_PRINT, NULL, self->item);
  if (mqctx == NULL) {
    if (PyErr_Occurred() == NULL)
      PyErr_SetString(PyExc_RuntimeError, "MqFactoryS exception");
    return NULL;
  } else {
    PyObject *arg = ((PyObject *)SELFX(mqctx));
    Py_INCREF(arg);
    return arg;
  }
}

static PyObject* NS(Copy) (
  MqFactoryS_Obj  *self,
  PyObject	  *args
)
{
  MQ_CST ident;
  if (!PyArg_ParseTuple(args, "s:Copy", &ident)) {
    return NULL;
  }
  return MqFactoryS_New (MqFactoryCopy (self->item, ident));
}

#define New_DOC	    "create a new MqS instance from a MqFactoryS instance"
#define Copy_DOC    "create a copy of a MqFactoryS instance using a new factory-identifer"

// Fill the Struct-Array

#define ARG(N,M) { #N , (PyCFunction) NS(N), M, N ## _DOC}
static PyMethodDef NS(MqFactoryS_Methods)[] = {

    ARG(New,	  METH_NOARGS),
    ARG(Copy,	  METH_VARARGS),

    {NULL, NULL, 0, NULL}        /* Sentinel */
};
#undef ARG

//////////////////////////////////////////////////////////////////////////////////
///
///				 Object definition
///
//////////////////////////////////////////////////////////////////////////////////

#define MqFactoryS_DOC	"libmsgque MqFactoryS object"

static void
NS(MqFactoryS_dealloc)(MqFactoryS_Obj* self)
{
  Py_TYPE(self)->tp_free((PyObject*)self);
}

PyTypeObject NS(MqFactoryS) = {
  {PyObject_HEAD_INIT(NULL)},
  "pymsgque.MqFactoryS",		  /* tp_name */
  sizeof(MqFactoryS_Obj),	  /* tp_basicsize */
  0,				  /* tp_itemsize */
  (destructor)NS(MqFactoryS_dealloc),	  /* tp_dealloc */
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
  MqFactoryS_DOC,		  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  NS(MqFactoryS_Methods),	  /* tp_methods */
  0,				  /* tp_members */
  0,				  /* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  0,				  /* tp_init */
  0,				  /* tp_alloc */
  0,				  /* tp_new  OLD: NS(MqFactoryS_new) */
};

//////////////////////////////////////////////////////////////////////////////////
///
///				 Public API
///
//////////////////////////////////////////////////////////////////////////////////

PyObject *
MqFactoryS_New (
  struct MqFactoryS *item
)
{
  if (item == NULL) {
    if (PyErr_Occurred() == NULL)
      PyErr_SetString(PyExc_RuntimeError, "MqFactoryS exception");
    return NULL;
  } else {
    MqFactoryS_Obj *itemO = PyObject_New(MqFactoryS_Obj, &NS(MqFactoryS));
    itemO->item = item;
    return (PyObject*)itemO;
  }
}

//////////////////////////////////////////////////////////////////////////////////
///
///				 context
///
//////////////////////////////////////////////////////////////////////////////////

PyObject* NS(FactoryCtxSet) (
  MqS_Obj    *self,
  PyObject   *args
)
{
  SETUP_context
  MqFactoryS_Obj *obj;
  PyErrorCheck (PyArg_ParseTuple(args, "O!:FactoryCtxSet", &NS(MqFactoryS), &obj));
  ErrorMqToPythonWithCheck (MqFactoryCtxSet (context, obj->item));
  SETUP_RETURN
}

PyObject* NS(FactoryCtxGet) (
  PyObject    *self
)
{
  return MqFactoryS_New (MqFactoryCtxGet (CONTEXT));
}

PyObject* NS(FactoryCtxIdentSet) (
  MqS_Obj    *self,
  PyObject   *args
)
{
  SETUP_context
  const char *c;
  if (!PyArg_ParseTuple(args, "s:FactoryCtxIdentSet", &c)) {
    return NULL;
  }
  ErrorMqToPythonWithCheck (MqFactoryCtxIdentSet (context, c));
  SETUP_RETURN
}

PyObject* NS(FactoryCtxIdentGet) (
  PyObject    *self
)
{
  return PyC2O(MqFactoryCtxIdentGet(CONTEXT));
}


