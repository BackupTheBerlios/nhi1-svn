/**
 *  \file       pymsgque/msgque_python.h
 *  \brief      \$Id: msgque_python.h 512 2009-11-30 14:52:29Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 512 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MSGQUE_PYTHON_H
#define MSGQUE_PYTHON_H

#include <Python.h>

// public visible names with package prefix
#define NS(n)	pymsgque_ ## n

// HAVE_DECLSPEC_DLL is defined in the "python" header files
// DLL_EXPORTS is needed by "msgque.h"
#if defined(HAVE_DECLSPEC_DLL) && !defined(DLL_EXPORTS)
# define DLL_EXPORTS
#endif

#include "mqconfig.h"
#include "msgque.h"

#define MQ_IS_POSIX
#include "debug.h"

/*****************************************************************************/
/*                                                                           */
/*                              Object_API				     */
/*                                                                           */
/*****************************************************************************/

/// \brief python MqBufferS Object
typedef struct MqBufferS_Obj {
  PyObject_HEAD
  struct MqBufferS * buf;
} MqBufferS_Obj;

PyObject * MqBufferS_Obj_From_MQ_BUF ( MQ_BUF );

/// \brief python MqS Object
typedef struct MqS_Obj {
  PyObject_HEAD
  struct MqS context;  ///< link to the \libmsgque object
} MqS_Obj;

/*****************************************************************************/
/*                                                                           */
/*                             MISC functions				     */
/*                                                                           */
/*****************************************************************************/

#define PyErrorCheck(PROC) if ((PROC) < 0) goto error;
#define PyErrorCheckN(PROC) if ((PROC) == NULL) goto error;
#define PyErrorCheckNT(TAG,PROC) if ((PROC) == NULL) goto TAG;

#define ICONTEXT (((MqS_Obj*)self)->context)
#define CONTEXT &ICONTEXT

#define SETUP_context \
  struct MqS * const context = CONTEXT;

#define ErrorMqToPython() NS(pErrorFromMq)(context)
#define ErrorMqToPythonWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorMqToPython(); \
    goto error; \
  }

#define SETUP_CHECK_RETURN(PROC) \
  MqErrorCheck(PROC); \
  Py_RETURN_NONE; \
  error: \
    ErrorMqToPython(); \
    return NULL;

#define SELF ((MqS_Obj*)context->self)
#define SETUP_self MqS_Obj* self = SELF;

#undef printO
#define printO(o)   fprintf(stdout,"%s -> ",#o);PyObject_Print((PyObject*)o,stdout,0);fprintf(stdout,"\n");

#define SETUP_RETURN  Py_RETURN_NONE; \
error: \
  return NULL; 


// from misc_python.c

enum MqErrorE MQ_DECL NS(ProcCall) (struct MqS * const , MQ_PTR const);
void MQ_DECL NS(ProcFree) (struct MqS const * const , MQ_PTR*);
enum MqErrorE MQ_DECL NS(ProcCopy) (struct MqS * const , MQ_PTR*);
void NS(pErrorFromMq) (struct MqS * const);
enum MqErrorE ListToMqBufferLS (struct MqS * const, PyObject *, struct MqBufferLS **);

// from context_python.c
enum MqErrorE NS(ChildCreate) (struct MqS*, struct MqBufferLS*);
enum MqErrorE NS(ParentCreate) (struct MqS*, struct MqBufferLS*);
void NS(ChildDelete) (struct MqS**);
void NS(ParentDelete) (struct MqS**);
void NS(ParentExit) (int, int);
PyObject* NS(ErrorSet) (PyObject*);
void NS(ProcessExit) (int);

// misc

static mq_inline MQ_STR PyO2C_START(PyObject** argP) {
  MQ_STR str = NULL;
  if (*argP != Py_None) {
    if ((*argP = PyUnicode_AsUTF8String(*argP)) == NULL) return NULL;
    if ((str = PyBytes_AsString(*argP)) == NULL) return NULL;
  } else {
    *argP = NULL;
  }
  return str;
}

static mq_inline void PyO2C_STOP(PyObject**utf8P) {
  if (*utf8P != NULL) {
    Py_DECREF(*utf8P);
    *utf8P=NULL;
  }
}

static mq_inline PyObject* PyC2O(MQ_CST v) {
  return (v?PyUnicode_FromString(v):(Py_INCREF(Py_None), Py_None));
}

#endif /* MSGQUE_PYTHON_H */

