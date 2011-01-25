/**
 *  \file       theLink/tclmsgque/factory_tcl.c
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

#define FACTORY_ARGS \
Tcl_Interp * interp, struct MqFactoryS * const item, int skip, int objc, struct Tcl_Obj *const *objv

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

static int FS(Copy) (FACTORY_ARGS)
{
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_DEFAULT_ERR;
  MQ_CST ident;
  CHECK_C(ident)
  CHECK_NOARGS
  MqFactoryErrorCheck(ret = MqFactoryCopy(item, ident));
  return TCL_OK;
error:
  Tcl_SetResult(interp, (MQ_STR) MqFactoryReturnMsg(ret), TCL_STATIC);
  return TCL_ERROR;
}

static int FS(Call) (FACTORY_ARGS)
{
  struct MqS * mqctx;
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_CALL_ERR;
  CHECK_NOARGS
  MqFactoryErrorCheck(ret = MqFactoryCall (item, (MQ_PTR) interp, &mqctx));
  Tcl_SetObjResult(interp, (Tcl_Obj*) mqctx->self);
  return TCL_OK;
error:
  Tcl_SetResult(interp, (MQ_STR) MqFactoryReturnMsg(ret), TCL_STATIC);
  return TCL_ERROR;
}

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
    { "Call",	    FS(Call)	  },
    { "New",	    FS(Call)	  },
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
  char buffer[100];
  sprintf(buffer, "<MqFactoryS-%p>", item);
  Tcl_CreateObjCommand (interp, buffer, FS(Cmd), item, FS(Free));

  Tcl_SetResult (interp, buffer, TCL_VOLATILE);
  Tcl_CreateExitHandler (FS(Free), item);

  return TCL_OK;
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
  struct MqFactoryS *item;
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_ADD_ERR;
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
  MqFactoryErrorCheck(
    ret = MqFactoryAdd (ident, 
      NS(FactoryCreate), factory, NS(FactoryFree), NS(FactoryCopy), 
      NS(FactoryDelete), NULL, NULL, NULL, 
      &item
    )
  );
  return FS(Create) (interp, item);
error:
  Tcl_SetResult(interp, (MQ_STR) MqFactoryReturnMsg(ret), TCL_STATIC);
  return TCL_ERROR;
}

int NS(FactoryDefault) (TCL_ARGS)
{
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_DEFAULT_ERR;
  int skip = 2;
  MQ_CST ident;
  Tcl_Obj *factory = NULL;
  CHECK_C(ident)
  CHECK_PROC_OPT(factory, "FactoryDefault ident ?factory-proc?")
  CHECK_NOARGS
  if (factory) {
    Tcl_IncrRefCount(factory);
    MqFactoryErrorCheck(ret = MqFactoryDefault(ident, NS(FactoryCreate), 
      factory, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL)
    );
  } else {
    MqFactoryErrorCheck(ret = MqFactoryDefault(ident, NS(FactoryCreate), 
      NULL, NULL, NULL, NS(FactoryDelete), NULL, NULL, NULL)
    );
  }
  return TCL_OK;
error:
  Tcl_SetResult(interp, (MQ_STR) MqFactoryReturnMsg(ret), TCL_STATIC);
  return TCL_ERROR;
}

int NS(FactoryGet) (TCL_ARGS)
{
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_GET_ERR;
  int skip = 2;
  MQ_CST ident = NULL;
  struct MqFactoryS * item;
  CHECK_C_OPT(ident)
  CHECK_NOARGS
  if ( (item = MqFactoryGet(ident)) == NULL) goto error;
  return FS(Create) (interp, item);
error:
  Tcl_SetResult(interp, (MQ_STR) MqFactoryReturnMsg(ret), TCL_STATIC);
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

int NS(FactoryCtxIdentSet) (NS_ARGS)
{
  SETUP_mqctx;
  MQ_CST ident;
  CHECK_C(ident)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqFactoryCtxIdentSet (mqctx, ident));
  RETURN_TCL
}

int NS(FactoryCtxIdentGet) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_SetResult(interp, (MQ_STR) MqFactoryCtxIdentGet(&tclctx->mqctx), TCL_STATIC);
  RETURN_TCL
}

