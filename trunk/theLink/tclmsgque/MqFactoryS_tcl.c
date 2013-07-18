/**
 *  \file       theLink/tclmsgque/MqFactoryS_tcl.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_tcl.h"

/*****************************************************************************/
/*                                                                           */
/*                             factory-static                                */
/*                                                                           */
/*****************************************************************************/

#define MqFactoryS_ARGS Tcl_Interp * interp, struct MqFactoryS * const item, int skip, int objc, struct Tcl_Obj *const *objv

#define FS(n)    Tclmsgque_MqFactoryS_ ## n

typedef int (
  *LookupKeywordF
) (
  Tcl_Interp  *		      interp,
  struct MqFactoryS * const   item,
  int			      skip,
  int			      objc,
  struct Tcl_Obj *const *     objv
);

struct LookupKeyword {
  const char	  *key;
  LookupKeywordF  keyF;
};

static int FS(New) (MqFactoryS_ARGS);
static int FS(Copy) (MqFactoryS_ARGS);

static int FS(Cmd) (
  ClientData clientData,
  Tcl_Interp * interp,
  int objc,
  Tcl_Obj * const objv[]
)
{
  int index;
  struct MqFactoryS * item = (struct MqFactoryS *) clientData;

  const static struct LookupKeyword keys[] = {
    { "New",	    FS(New)	  },
    { "Copy",	    FS(Copy)	  },
    { NULL,	    NULL	  }
  };

  if (objc < 2) {
    Tcl_WrongNumArgs (interp, 1, objv, "subcommand ...");
    return TCL_ERROR;
  }

  TclErrorCheck (
    Tcl_GetIndexFromObjStruct (
      interp, objv[1], &keys, sizeof(struct LookupKeyword), "subcommand", 0, &index
    )
  );

  return (*keys[index].keyF) (interp, item, 2, objc, objv);
}

static void FS(Free) (
  ClientData clientData
)
{
  struct MqFactoryS *item = (struct MqFactoryS *) clientData;
  Tcl_DeleteExitHandler (NS(MqFactoryS_Free), item);
}

static int FS(Create) (
  Tcl_Interp * interp,
  struct MqFactoryS * item
)
{
  if (item == NULL) {
    Tcl_SetResult (interp, "unable to create the factory object", TCL_STATIC);
    return TCL_ERROR;
  } else {
    char buffer[100];
    sprintf(buffer, "<MqFactoryS-%p>", item);
    Tcl_CreateObjCommand (interp, buffer, FS(Cmd), item, FS(Free));
    Tcl_SetResult (interp, buffer, TCL_VOLATILE);
    Tcl_CreateExitHandler (FS(Free), item);
    return TCL_OK;
  }
}

static int FS(Copy) (MqFactoryS_ARGS)
{
  MQ_CST ident;
  CHECK_C(ident)
  CHECK_NOARGS
  return FS(Create) (interp, MqFactoryCopy(item, ident));
error:
  return TCL_ERROR;
}

static int FS(New) (MqFactoryS_ARGS)
{
  struct MqS * mqctx;
  CHECK_NOARGS
  mqctx = MqFactoryNew (MQ_ERROR_PRINT, (MQ_PTR) interp, item);
  if (mqctx == NULL) goto error;
  Tcl_SetObjResult(interp, (Tcl_Obj*) mqctx->self);
  RETURN_TCL;
}

/*****************************************************************************/
/*                                                                           */
/*                             factory-static                                */
/*                                                                           */
/*****************************************************************************/

static void NS(FactoryFree) (
  MQ_PTR *dataP
)
{
  Tcl_DecrRefCount ( (Tcl_Obj*) *dataP);
  *dataP = NULL;
}

static void NS(FactoryCopy) (
  MQ_PTR *dataP
)
{
  Tcl_IncrRefCount ( (Tcl_Obj*) *dataP);
}

int NS(FactoryAdd) (TCL_ARGS)
{
  int skip = 2;
  MQ_CST ident;
  Tcl_Obj *factory;
  if (objc == skip+1) {
    CHECK_PROC(factory, "FactoryAdd ?ident? factory-proc")
    ident = VAL2CST(factory);
  } else {
    CHECK_C(ident)
    CHECK_PROC(factory, "FactoryAdd ?ident? factory-proc")
  }
  CHECK_NOARGS
  Tcl_IncrRefCount(factory);
  return FS(Create) (interp, MqFactoryAdd (MQ_ERROR_PRINT, ident, 
    NS(FactoryCreate), factory, NS(FactoryFree), NS(FactoryCopy), 
    NS(FactoryDelete), NULL, NULL, NULL)
  );
error:
  return TCL_ERROR;
}

int NS(FactoryDefault) (TCL_ARGS)
{
  int skip = 2;
  MQ_CST ident;
  Tcl_Obj *factory = NULL;
  CHECK_C(ident)
  CHECK_PROC_OPT(factory, "FactoryDefault ident ?factory-proc?")
  CHECK_NOARGS
  if (factory) {
    Tcl_IncrRefCount(factory);
    return FS(Create) (interp, MqFactoryDefault(MQ_ERROR_PRINT, ident, NS(FactoryCreate), 
      factory, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL)
    );
  } else {
    return FS(Create) (interp, MqFactoryDefault(MQ_ERROR_PRINT, ident, NS(FactoryCreate), 
      NULL, NULL, NULL, NS(FactoryDelete), NULL, NULL, NULL)
    );
  }
error:
  return TCL_ERROR;
}

int NS(FactoryGet) (TCL_ARGS)
{
  int skip = 2;
  MQ_CST ident = NULL;
  CHECK_C_OPT(ident)
  CHECK_NOARGS
  return FS(Create) (interp, MqFactoryGet(ident));
error:
  return TCL_ERROR;
}

int NS(FactoryGetCalled) (TCL_ARGS)
{
  int skip = 2;
  MQ_CST ident = NULL;
  CHECK_C_OPT(ident)
  CHECK_NOARGS
  return FS(Create) (interp, MqFactoryGetCalled(ident));
error:
  return TCL_ERROR;
}

int NS(FactoryDefaultIdent) (TCL_ARGS)
{
  int skip = 2;
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR) MqFactoryDefaultIdent(), TCL_STATIC);
  RETURN_TCL
}

/*****************************************************************************/
/*                                                                           */
/*                             factory-context                               */
/*                                                                           */
/*****************************************************************************/

int NS(FactoryCtxSet) (MqS_ARGS)
{
  struct MqFactoryS *factory;
  SETUP_mqctx;
  CHECK_FACTORY(factory)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqFactoryCtxSet (mqctx, factory));
  RETURN_TCL
}

int NS(FactoryCtxGet) (MqS_ARGS)
{
  CHECK_NOARGS
  return FS(Create) (interp, MqFactoryCtxGet(MQCTX));
error:
  return TCL_ERROR;
}

int NS(FactoryCtxIdentSet) (MqS_ARGS)
{
  MQ_CST ident;
  SETUP_mqctx;
  CHECK_C(ident)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqFactoryCtxIdentSet (mqctx, ident));
  RETURN_TCL
}

int NS(FactoryCtxIdentGet) (MqS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR) MqFactoryCtxIdentGet(&tclctx->mqctx), TCL_STATIC);
  RETURN_TCL
}

