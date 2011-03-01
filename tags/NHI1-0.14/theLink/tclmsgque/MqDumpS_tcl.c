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
  Tcl_Interp  *		    interp,
  struct MqDumpS * const    dump
);

struct LookupKeyword {
  const char	  *key;
  LookupKeywordF  keyF;
};

static HDD(Size)
{
  Tcl_SetObjResult(interp, Tcl_NewIntObj (MqDumpSize (dump)));
  return TCL_OK;
}

/** \brief create the \b $dump tcl command
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
  struct MqDumpS * dump = (struct MqDumpS *) clientData;

  const static struct LookupKeyword keys[] = {
    { "Size",	    NS(Size)	  },
    { NULL,	    NULL	  }
  };

  if (objc != 2) {
    Tcl_WrongNumArgs (interp, 1, objv, "Get...");
    return TCL_ERROR;
  }

  TclErrorCheck (Tcl_GetIndexFromObjStruct (interp, objv[1], &keys, 
      sizeof(struct LookupKeyword), "subcommand", 0, &index));

  return (*keys[index].keyF) (interp, dump);
}

/** \brief delete a \e MqDumpS object (called by "rename $dump {}")
 *
 *  \param[in] clientData command specific data, a \e MqDumpS object in this case
 **/
static void NS(MqDumpS_Free) (
  ClientData clientData
)
{
  struct MqDumpS *dump = (struct MqDumpS *) clientData;
  Tcl_DeleteExitHandler (NS(MqDumpS_Free), dump);
  MqSysFree(dump);
}

void NS(MqDumpS_New) (
  Tcl_Interp * interp,
  struct MqDumpS * dump
)
{
  char buffer[30];
  sprintf(buffer, "<MqDumpS-%p>", dump);
  Tcl_CreateObjCommand (interp, buffer, NS(MqDumpS_Cmd), dump, NS(MqDumpS_Free));

  Tcl_SetResult (interp, buffer, TCL_VOLATILE);
  Tcl_CreateExitHandler (NS(MqDumpS_Free), dump);
}
