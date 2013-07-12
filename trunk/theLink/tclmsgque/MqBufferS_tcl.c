/**
 *  \file       theLink/tclmsgque/MqBufferS_tcl.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"

#define ErrorBufToTcl() NS(pErrorFromMq)(interp, buf->context)
#define ErrorBufToTclWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorBufToTcl(); \
    goto error; \
  }

#define SET(T,L) \
static int NS(Set ## T) (MqBufferS_ARGS) \
{ \
  L val; \
  CHECK_ ## T(val) \
  CHECK_NOARGS \
  MqBufferSet ## T (buf, val); \
  RETURN_TCL \
}

typedef int (
  *LookupKeywordF
) (
  Tcl_Interp  *		      interp,
  struct MqBufferS * const    buf,
  int                         skip,
  int                         objc,
  struct Tcl_Obj *const *     objv
);

struct LookupKeyword {
  const char	  *key;
  LookupKeywordF  keyF;
};

static int NS(GetType) (MqBufferS_ARGS)
{
  CHECK_NOARGS
  char str[2] = {MqBufferGetType(buf), '\0'};
  Tcl_SetResult (interp, str, TCL_VOLATILE);
  RETURN_TCL
}

SET(Y,MQ_BYT)
SET(O,MQ_BOL)
SET(S,MQ_SRT)
SET(I,MQ_INT)
SET(W,MQ_WID)
SET(F,MQ_FLT)
SET(D,MQ_DBL)
SET(C,MQ_CST)

static int NS(SetB) (MqBufferS_ARGS)
{
  MQ_BIN val;
  MQ_SIZE len;
  CHECK_B(val,len)
  CHECK_NOARGS
  MqBufferSetB (buf, val, len);
  RETURN_TCL
}

static int NS(GetY) (MqBufferS_ARGS)
{
  MQ_BYT val;
  CHECK_NOARGS
  ErrorBufToTclWithCheck (MqBufferGetY (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

static int NS(GetO) (MqBufferS_ARGS)
{
  MQ_BOL val;
  CHECK_NOARGS
  ErrorBufToTclWithCheck (MqBufferGetO (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj(val == MQ_YES));
  RETURN_TCL
}

static int NS(GetS) (MqBufferS_ARGS)
{
  MQ_SRT val;
  CHECK_NOARGS
  ErrorBufToTclWithCheck (MqBufferGetS (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

static int NS(GetI) (MqBufferS_ARGS)
{
  MQ_INT val;
  CHECK_NOARGS
  ErrorBufToTclWithCheck (MqBufferGetI (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

static int NS(GetF) (MqBufferS_ARGS)
{
  MQ_FLT val;
  CHECK_NOARGS
  ErrorBufToTclWithCheck (MqBufferGetF (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
  RETURN_TCL
}

static int NS(GetW) (MqBufferS_ARGS)
{
  MQ_WID val;
  CHECK_NOARGS
  ErrorBufToTclWithCheck (MqBufferGetW (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewWideIntObj(val));
  RETURN_TCL
}

static int NS(GetD) (MqBufferS_ARGS)
{
  MQ_DBL val;
  CHECK_NOARGS
  ErrorBufToTclWithCheck (MqBufferGetD (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
  RETURN_TCL
}

static int NS(GetB) (MqBufferS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(buf->data,buf->cursize));
  RETURN_TCL
}

static int NS(GetC) (MqBufferS_ARGS)
{
  MQ_CST val;
  CHECK_NOARGS
  ErrorBufToTclWithCheck (MqBufferGetC (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewStringObj(val,-1));
  RETURN_TCL
}

static int NS(Dup) (MqBufferS_ARGS)
{
  CHECK_NOARGS
  NS(MqBufferS_New) (interp, MqBufferDup(buf));
  RETURN_TCL
}

static int NS(Delete) (MqBufferS_ARGS)
{
  char buffer[30];
  CHECK_NOARGS
  sprintf(buffer, "<MqBufferS-%p>", buf);
  MqBufferDelete((MQ_BUF*)&buf);
  Tcl_DeleteCommand (interp, buffer);
  RETURN_TCL
}

/** \brief create the \b $buffer tcl command
 *
 *  \tclmsgque
 *
 * \param[in] clientData command specific data, \e ContextS in this case
 * \param[in] interp the current Tcl interpreter
 * \param[in] objc number of items in the \e objv array
 * \param[in] objv array with arguments of the \c $CTX 'Tcl' command
 * \return Tcl error-code
 **/
static int NS(MqBufferS_Cmd) (
  ClientData clientData,
  Tcl_Interp * interp,
  int objc,
  Tcl_Obj * const objv[]
)
{
  int index;
  struct MqBufferS * buf = (struct MqBufferS *) clientData;

  const static struct LookupKeyword keys[] = {
    { "GetType",    NS(GetType)	  },

    { "GetY",	    NS(GetY)	  },
    { "GetO",	    NS(GetO)	  },
    { "GetS",	    NS(GetS)	  },
    { "GetI",	    NS(GetI)	  },
    { "GetF",	    NS(GetF)	  },
    { "GetW",	    NS(GetW)	  },
    { "GetD",	    NS(GetD)	  },
    { "GetB",	    NS(GetB)	  },
    { "GetC",	    NS(GetC)	  },

    { "SetY",	    NS(SetY)	  },
    { "SetO",	    NS(SetO)	  },
    { "SetS",	    NS(SetS)	  },
    { "SetI",	    NS(SetI)	  },
    { "SetF",	    NS(SetF)	  },
    { "SetW",	    NS(SetW)	  },
    { "SetD",	    NS(SetD)	  },
    { "SetB",	    NS(SetB)	  },
    { "SetC",	    NS(SetC)	  },

    { "Dup",	    NS(Dup)	  },
    { "Delete",	    NS(Delete)	  },
    { NULL,	    NULL	  }
  };

  if (objc < 2) {
    Tcl_WrongNumArgs (interp, 1, objv, "subcommand ...");
    return TCL_ERROR;
  }

  TclErrorCheck (Tcl_GetIndexFromObjStruct (interp, objv[1], &keys, 
      sizeof(struct LookupKeyword), "subcommand", 0, &index));

  return (*keys[index].keyF) (interp, buf, 2, objc, objv);
}

/** \brief delete a \e MqBufferS object (called by "rename $buf {}")
 *
 *  \param[in] clientData command specific data, a \e MqBufferS object in this case
 **/
static void NS(MqBufferS_Free) (
  ClientData clientData
)
{
  struct MqBufferS *buf = (struct MqBufferS *) clientData;
  Tcl_DeleteExitHandler (NS(MqBufferS_Free), buf);
  MqBufferDelete(&buf);
}

void NS(MqBufferS_New) (
  Tcl_Interp * interp,
  struct MqBufferS * buf
)
{
  char buffer[30];
  sprintf(buffer, "<MqBufferS-%p>", buf);
  Tcl_CreateObjCommand (interp, buffer, NS(MqBufferS_Cmd), buf, NS(MqBufferS_Free));

  Tcl_SetResult (interp, buffer, TCL_VOLATILE);
  Tcl_CreateExitHandler (NS(MqBufferS_Free), buf);
}

/* \brief create a buffer object 
 *
 *  \param[in] interp current Tcl interpreter
 *  \param[in] objc number of objects in \e objv
 *  \param[in] objv array of \e Tcl_Obj objects
 *  \return Tcl error-code
int NS(MqBufferS_Init) (
  Tcl_Interp * interp,
  int objc,
  Tcl_Obj * const objv[]
)
{
  struct MqBufferS * bufCtx = MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof(*bufCtx));
  struct MqBufferS * tmpCtx;

  if (objc != 3 || NS(GetClientData) (interp, objv[2], MQ_MqBufferS_SIGNATURE, (MQ_PTR*) &tmpCtx) == TCL_ERROR) {
    Tcl_WrongNumArgs (interp, 0, NULL, "tclmsgque MqBufferS buffer");
    return TCL_ERROR;
  }
  bufCtx->buf = MqBufferDup(((struct MqBufferS *)&tmpCtx)->buf);
  bufCtx->persistent = MQ_YES;

  return NS(MqBufferS_New) (interp, bufCtx);
}
 **/

