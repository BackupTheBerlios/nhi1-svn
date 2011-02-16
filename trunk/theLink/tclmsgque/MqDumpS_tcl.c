/**
 *  \file       theLink/tclmsgque/MqDumpS_tcl.c
 *  \brief      \$Id: MqDumpS_tcl.c 478 2011-02-13 17:10:28Z aotto1968 $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 478 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"

typedef int (
  *LookupKeywordF
) (
  Tcl_Interp  *		      interp,
  struct MqDumpS * const    buf
);

struct LookupKeyword {
  const char	  *key;
  LookupKeywordF  keyF;
};

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
static int NS(MqDumpS_Cmd) (
  ClientData clientData,
  Tcl_Interp * interp,
  int objc,
  Tcl_Obj * const objv[]
)
{
  int index;
  struct MqDumpS * buf = (struct MqDumpS *) clientData;

  static struct LookupKeyword keys[] = {
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

/** \brief delete a \e MqDumpS object (called by "rename $buf {}")
 *
 *  \param[in] clientData command specific data, a \e MqDumpS object in this case
 **/
static void NS(MqDumpS_Free) (
  ClientData clientData
)
{
  struct MqDumpS *buf = (struct MqDumpS *) clientData;
  Tcl_DeleteExitHandler (NS(MqDumpS_Free), buf);
}

void NS(MqDumpS_New) (
  Tcl_Interp * interp,
  struct MqDumpS * buf
)
{
  char buffer[30];
  sprintf(buffer, "<MqDumpS-%p>", buf);
  Tcl_CreateObjCommand (interp, buffer, NS(MqDumpS_Cmd), buf, NS(MqDumpS_Free));

  Tcl_SetResult (interp, buffer, TCL_VOLATILE);
  Tcl_CreateExitHandler (NS(MqDumpS_Free), buf);
}
