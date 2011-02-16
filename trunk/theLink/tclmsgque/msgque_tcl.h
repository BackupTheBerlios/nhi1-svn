/**
 *  \file       theLink/tclmsgque/msgque_tcl.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MSGQUE_TCL_H
#define MSGQUE_TCL_H

#include "mqconfig.h"
#include "msgque.h"

#include "debug.h"

#include <tcl.h>

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

/*****************************************************************************/
/*                                                                           */
/*                              tclctx_basic                                */
/*                                                                           */
/*****************************************************************************/

#define NS(n)    Tclmsgque_ ## n
#define NS_ARGS	 Tcl_Interp * interp, struct TclContextS * const tclctx, \
		    int skip, int objc, struct Tcl_Obj *const *objv
#define TCL_ARGS Tcl_Interp * interp, int objc, struct Tcl_Obj *const *objv
#define HDB(name) int NS(name) ( Tcl_Interp * interp, struct MqBufferS * const buf )

#define SETUP_mqctx	struct MqS * const mqctx = (struct MqS * const) tclctx;
#define SETUP_tclctx	struct TclContextS * const tclctx = (struct TclContextS * const) mqctx;
#define SETUP_interp	Tcl_Interp *const interp = (Tcl_Interp*const) tclctx->mqctx.threadData;
#define SELF		((Tcl_Obj*)tclctx->mqctx.self)
#define SETUP_self	Tcl_Obj *self = SELF;
#define MQCTX		((struct MqS * const)tclctx)
#define TCLCTX		((struct TclContextS * const)mqctx)

#define ErrorMqToTcl() NS(pErrorFromMq)(interp, mqctx)
#define ErrorMqToTclWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorMqToTcl(); \
    goto error; \
  }
#define RETURN_TCL \
  return TCL_OK; \
error: \
  return TCL_ERROR;

#define TclErrorCheck(cmd) if (cmd != TCL_OK) return TCL_ERROR
#define TclErrorCheckG(cmd) if (cmd != TCL_OK) goto error
#define TclErrorCheckG1(cmd) if (cmd != TCL_OK) goto error1
#define TclErrorCheckG2(cmd) if (cmd != TCL_OK) goto error2
#define TclErrorToMq(ctx,cmd) if (cmd != TCL_OK) {\
  MqErrorC(mqctx,__func__,-1,Tcl_GetStringResult(interp)); \
  goto error; \
}
#define TclErrorToCtxWithReturn(ctx,cmd) if (cmd != TCL_OK) {\
  return MqErrorC(ctx,__func__,-1,Tcl_GetStringResult(interp)); \
}

/// \brief T)hread L)ocal S)torage identifer (set by "configure")
#if defined(TLS)
#  define MQ_TLS TLS
#else
#  define MQ_TLS
#endif

/*****************************************************************************/
/*                                                                           */
/*                        argument check functions                           */
/*                                                                           */
/*****************************************************************************/

#define CheckForAdditionalArg(txt) \
  objc--; objv++; \
  if (objc<1) { \
    Tcl_SetResult(interp, "no additional argument available for '" #txt "'", TCL_STATIC); \
    return TCL_ERROR; \
  }

#define CHECK_NOARGS \
  if (skip != objc) {\
    Tcl_WrongNumArgs(interp, skip, objv, "");\
    goto error;\
  }

#define CHECK_OBJ(val) \
  if (skip >= objc) {\
    Tcl_WrongNumArgs(interp, skip, objv, #val " ...");\
    goto error;\
  } else { \
    val = objv[skip]; \
    skip++; \
  }

#define CHECK_OBJ_OPT(val) \
  if (skip >= objc) {\
    val = NULL; \
  } else { \
    val = objv[skip]; \
    skip++; \
  }

#define CHECK_PROC(val, err) \
  if (skip >= objc) {\
    Tcl_WrongNumArgs(interp, skip, objv, #val " ...");\
    goto error;\
  } else { \
    TclErrorCheckG(NS(ProcCheck)(interp,objv[skip], err)); \
    val = objv[skip]; \
    skip++; \
  }

#define CHECK_PROC_OPT(val, err) \
  if (skip < objc) {\
    TclErrorCheckG(NS(ProcCheck)(interp,objv[skip], err)); \
    val = objv[skip]; \
    skip++; \
  }

#define CHECK_MQS(val) \
  val = NULL; \
  if (skip < objc) { \
    NS(GetClientData) (interp, objv[skip], MQ_MqS_SIGNATURE, (MQ_PTR*)&val); \
  } \
  if (val == NULL) { \
    Tcl_WrongNumArgs(interp, skip, objv, #val " ..."); \
    goto error; \
  } else { \
    skip++; \
  }

#define CHECK_MQS_OPT(val) \
  if (skip < objc) { \
    val = NULL; \
    if (objv[skip]->length == 0 && objv[skip]->typePtr == NULL) { \
      skip++; \
    } else { \
      NS(GetClientData) (interp, objv[skip], MQ_MqS_SIGNATURE, (MQ_PTR*)&val); \
      if (val == NULL) { \
	if (strncmp(Tcl_GetStringFromObj(objv[skip], NULL), "NULL", 4)) { \
	  Tcl_WrongNumArgs(interp, skip, objv, #val " ..."); \
	  goto error; \
	} else { \
	  Tcl_ResetResult(interp); \
	  skip++; \
	} \
      } else { \
	skip++; \
      } \
    } \
  }

#define CHECK_BUFFER(val) \
  val = NULL; \
  if (skip < objc) {\
    NS(GetClientData) (interp, objv[skip], MQ_MqBufferS_SIGNATURE, (MQ_PTR*)&val); \
  } \
  if (val == NULL) { \
    Tcl_WrongNumArgs(interp, skip, objv, #val " ..."); \
    goto error; \
  } else { \
    skip++; \
  }

#define CHECK_FACTORY(val) \
  val = NULL; \
  if (skip < objc) {\
    NS(GetClientData) (interp, objv[skip], MQ_MqFactoryS_SIGNATURE, (MQ_PTR*)&val); \
  } \
  if (val == NULL) { \
    Tcl_WrongNumArgs(interp, skip, objv, #val " ..."); \
    goto error; \
  } else { \
    skip++; \
  }

#define CHECK_DUMP(val) \
  val = NULL; \
  if (skip < objc) {\
    NS(GetClientData) (interp, objv[skip], MQ_MqDumpS_SIGNATURE, (MQ_PTR*)&val); \
  } \
  if (val == NULL) { \
    Tcl_WrongNumArgs(interp, skip, objv, #val " ..."); \
    goto error; \
  } else { \
    skip++; \
  }

#define VAL2CST(val) Tcl_GetString(val)
#define CHECK_C(val) \
  if (skip >= objc) {\
    Tcl_WrongNumArgs(interp, skip, objv, #val " ...");\
    goto error;\
  } else { \
    val = VAL2CST(objv[skip]); \
    skip++; \
  }

#define CHECK_C_OPT(val) \
  if (skip < objc) {\
    val = VAL2CST(objv[skip]); \
    skip++; \
  }

#define CHECK_B(val,len) \
  if (skip >= objc) {\
    Tcl_WrongNumArgs(interp, skip, objv, #val " ...");\
    goto error;\
  } else { \
    val = Tcl_GetByteArrayFromObj(objv[skip], &len); \
    skip++; \
  }

#define CHECK_TMPL(val,type,ttype,func) \
  if (skip >= objc) {\
    Tcl_WrongNumArgs(interp, skip, objv, #val " ...");\
    goto error;\
  } else { \
    ttype tmp; \
    TclErrorCheckG(func(interp, objv[skip], &tmp)); \
    val = (type) tmp; \
    skip++; \
  }

#define CHECK_TMPL_OPT(val,type,ttype,func) \
  if (skip < objc) {\
    ttype tmp; \
    TclErrorCheckG(func(interp, objv[skip], &tmp)); \
    val = (type) tmp; \
    skip++; \
  }

#define CHECK_ARGS(argv) \
  argv = (objc-skip > 0 ? MqBufferLCreate (objc-skip) : NULL); \
  { \
    int i; \
    for (i = skip; i < objc; i++) { \
      MqBufferLAppendC (argv, Tcl_GetString (objv[i])); \
    } \
  }

#define CHECK_Y(val) CHECK_TMPL(val,MQ_BYT,int,Tcl_GetIntFromObj)
#define CHECK_O(val) CHECK_TMPL(val,MQ_BOL,int,Tcl_GetBooleanFromObj)
#define CHECK_S(val) CHECK_TMPL(val,MQ_SRT,int,Tcl_GetIntFromObj)
#define CHECK_I(val) CHECK_TMPL(val,MQ_INT,int,Tcl_GetIntFromObj)
#define CHECK_F(val) CHECK_TMPL(val,MQ_FLT,double,Tcl_GetDoubleFromObj)
#define CHECK_W(val) CHECK_TMPL(val,MQ_WID,Tcl_WideInt,Tcl_GetWideIntFromObj)
#define CHECK_D(val) CHECK_TMPL(val,MQ_DBL,double,Tcl_GetDoubleFromObj)

#define CHECK_W_OPT(val) CHECK_TMPL_OPT(val,MQ_WID,Tcl_WideInt,Tcl_GetWideIntFromObj)
#define CHECK_I_OPT(val) CHECK_TMPL_OPT(val,MQ_INT,int,Tcl_GetIntFromObj)

/*****************************************************************************/
/*                                                                           */
/*                                  types                                    */
/*                                                                           */
/*****************************************************************************/

/** \brief object to link the \libmsgque object with application specific data
 *
 */
struct TclContextS {
  struct MqS mqctx;	///< link to the \libmsgque object
  Tcl_Command command;  ///< Tcl command to represent the 'Context'
  Tcl_Obj *dict;	///< dictionary linked to tclctx
};

/*****************************************************************************/
/*                                                                           */
/*                                 functions                                 */
/*                                                                           */
/*****************************************************************************/

// from msgque_tcl.h

MQ_STR NS(printName) ( Tcl_Obj *);
MQ_STR NS(printObj)  ( const char *, Tcl_Obj *);
int NS(MqS_Cmd) ( ClientData, Tcl_Interp *, int, Tcl_Obj * const [] );

// from context_tcl.c

int   NS(MqS_Init)	    ( Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]);
void  NS(MqS_Free)	    ( ClientData );

// from config_tcl.c

void  NS(FactoryDelete)	    ( struct MqS *, MQ_BOL, struct MqFactoryS* );
enum MqErrorE  NS(FactoryCreate)  ( struct MqS * const, enum MqFactoryE, struct MqFactoryS*const, struct MqS **);
enum MqErrorE NS(EventLink) ( struct MqS * const, MQ_PTR const );

// from MqBufferS_tcl.c

void   NS(MqBufferS_New) ( Tcl_Interp *, struct MqBufferS *);

// from MqDumpS_tcl.c

void   NS(MqDumpS_New) ( Tcl_Interp *, struct MqDumpS *);

// from misc_tcl.c

int   NS(pErrorFromMq)	    ( Tcl_Interp*, struct MqS * const);
int   NS(ErrorStringToTcl)  ( Tcl_Interp*, MQ_CST);
int   NS(GetClientData)	    ( Tcl_Interp*, Tcl_Obj*, MQ_INT, MQ_PTR*);
int   NS(ReadNext)	    ( struct MqReadS* const, Tcl_Obj **, Tcl_Obj **);
int   NS(ReadAll)	    ( Tcl_Interp *, struct MqReadS * const, int, Tcl_Obj **);
int   NS(PrepareList)	    ( struct MqS const * const, Tcl_Obj * const, int const);

int   NS(ProcCheck)	    ( Tcl_Interp *, struct Tcl_Obj *, char const * const);
void MQ_DECL NS(ProcFree)   ( struct MqS const * const, MQ_PTR *);
void MQ_DECL NS(ProcCopy)   ( struct MqS * const, MQ_PTR *);
enum MqErrorE MQ_DECL NS(ProcCall) ( struct MqS * const , MQ_PTR);
enum MqErrorE   NS(ProcError)	    ( struct TclContextS * const, MQ_CST);

// from error_tcl.c


#endif // MSGQUE_TCL_H







