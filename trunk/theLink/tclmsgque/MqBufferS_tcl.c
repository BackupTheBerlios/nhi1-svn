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

typedef int (
  *LookupKeywordF
) (
  Tcl_Interp  *		      interp,
  struct MqBufferS * const    buf
);

struct LookupKeyword {
  const char	  *key;
  LookupKeywordF  keyF;
};

static HDB(GetType)
{
  char str[2] = {MqBufferGetType(buf), '\0'};
  Tcl_SetResult (interp, str, TCL_VOLATILE);
  return TCL_OK;
}

static HDB(GetY)
{
  MQ_BYT val;
  ErrorBufToTclWithCheck (MqBufferGetY (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

static HDB(GetO)
{
  MQ_BOL val;
  ErrorBufToTclWithCheck (MqBufferGetO (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewBooleanObj(val == MQ_YES));
  RETURN_TCL
}

static HDB(GetS)
{
  MQ_SRT val;
  ErrorBufToTclWithCheck (MqBufferGetS (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

static HDB(GetI)
{
  MQ_INT val;
  ErrorBufToTclWithCheck (MqBufferGetI (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
  RETURN_TCL
}

static HDB(GetF)
{
  MQ_FLT val;
  ErrorBufToTclWithCheck (MqBufferGetF (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
  RETURN_TCL
}

static HDB(GetW)
{
  MQ_WID val;
  ErrorBufToTclWithCheck (MqBufferGetW (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewWideIntObj(val));
  RETURN_TCL
}

static HDB(GetD)
{
  MQ_DBL val;
  ErrorBufToTclWithCheck (MqBufferGetD (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
  RETURN_TCL
}

static HDB(GetB)
{
  Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(buf->data,buf->cursize));
  return TCL_OK;
}

static HDB(GetC)
{
  MQ_CST val;
  ErrorBufToTclWithCheck (MqBufferGetC (buf, &val));
  Tcl_SetObjResult(interp, Tcl_NewStringObj(val,-1));
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

  static struct LookupKeyword keys[] = {
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
    { NULL,	    NULL	  }
  };

  if (objc != 2) {
    Tcl_WrongNumArgs (interp, 1, objv, "Get...");
    return TCL_ERROR;
  }

  TclErrorCheck (Tcl_GetIndexFromObjStruct (interp, objv[1], &keys, 
      sizeof(struct LookupKeyword), "subcommand", 0, &index));

  return (*keys[index].keyF) (interp, buf);
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
}

int NS(MqBufferS_Pointer) (
  Tcl_Interp * interp,
  struct MqBufferS * buf
)
{
  char buffer[100];
  sprintf(buffer, "<MqBufferS-%p>", buf);
  Tcl_CreateObjCommand (interp, buffer, NS(MqBufferS_Cmd), buf, NS(MqBufferS_Free));

  Tcl_SetResult (interp, buffer, TCL_VOLATILE);
  Tcl_CreateExitHandler (NS(MqBufferS_Free), buf);

  return MQ_OK;
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

  if (objc != 3 || NS(GetClientData) (interp, objv[2], (MQ_PTR*) &tmpCtx) == TCL_ERROR) {
    Tcl_WrongNumArgs (interp, 0, NULL, "tclmsgque MqBufferS buffer");
    return TCL_ERROR;
  }
  bufCtx->buf = MqBufferDup(((struct MqBufferS *)&tmpCtx)->buf);
  bufCtx->persistent = MQ_YES;

  return NS(MqBufferS_Pointer) (interp, bufCtx);
}
 **/



