/**
 *  \file       theLink/tclmsgque/context_tcl.c
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

#define MQ_CONTEXT_S MQCTX

typedef int (
  *LookupKeywordF
) (
  Tcl_Interp  *		      interp,
  struct TclContextS * const  tclctx,
  int			      skip,
  int			      objc,
  struct Tcl_Obj *const *     objv
);

struct LookupKeyword {
  const char	  *key;
  LookupKeywordF  keyF;
};

/*****************************************************************************/
/*                                                                           */
/*                                 helper                                    */
/*                                                                           */
/*****************************************************************************/

/// \brief link \e Tcl event-queue with \libmsgque event-queue
///
/// \libmsgque using the \e -fevent option to get a 'C' procedure for calling an
/// external event-queue.
///
/// \param[in] tclctx object to work on
///
static void
NS(EventLink) (
  struct MqS * const tclctx
)
{
  Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT);
}

/// \brief helper: used to simplify the code
#define CheckForAdditionalArg(txt) \
    objc--; objv++; \
    if (objc<1) { \
	Tcl_SetResult(interp, "no additional argument available for '" #txt "'", TCL_STATIC); \
	return TCL_ERROR; \
    }

/*****************************************************************************/
/*                                                                           */
/*                                 subcommands                               */
/*                                                                           */
/*****************************************************************************/

static
int NS(RenameTo) (NS_ARGS)
{
  Tcl_Obj * lobjv[3], *new;
  int ret;

  CHECK_OBJ(new)
  CHECK_NOARGS

  lobjv[0] = Tcl_NewStringObj("rename",-1);
  lobjv[1] = ((Tcl_Obj*)tclctx->mqctx.self);
  lobjv[2] = new;

  Tcl_IncrRefCount(lobjv[0]);
  Tcl_IncrRefCount(new);
  ret = Tcl_EvalObjv (interp, 3, lobjv, TCL_EVAL_GLOBAL);
  Tcl_DecrRefCount(lobjv[0]);

  TclErrorCheck(ret);
  if (ret != TCL_OK) {
    Tcl_DecrRefCount(new);
    return TCL_ERROR;
  }
  Tcl_DecrRefCount(SELF);
  tclctx->mqctx.self = (void*)new;

  RETURN_TCL
}

static
int NS(LinkDelete) (NS_ARGS)
{
  CHECK_NOARGS
  MqLinkDelete (MQCTX);
  RETURN_TCL
}

static
int NS(ProcessEvent) (NS_ARGS)
{
  SETUP_mqctx
  int timeout = -2;
  int FLAGS = MQ_WAIT_NO;
  int iA;
  static const char *optA[] = { "-timeout", "-wait", NULL };
  static const char *optB[] = { "NO", "ONCE", "FOREVER", NULL };
  enum optE { TIMEOUT, WAIT };

  // look for options
  objc -= skip;
  objv += skip;
  while (objc) {
    TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[0], optA, "option", 0, &iA));
    switch ((enum optE) iA) {
      case WAIT:
        CheckForAdditionalArg (-wait);
	TclErrorCheck (Tcl_GetIndexFromObj (interp, objv[0], optB, "-wait", 0, &FLAGS));
	objv++;objc--;
	break;
      case TIMEOUT:
        CheckForAdditionalArg (-timeout);
        TclErrorCheck (Tcl_GetIntFromObj (interp, objv[0], &timeout));
	objv++;objc--;
        break;
    }
  }

  ErrorMqToTclWithCheck (MqProcessEvent (mqctx, timeout, FLAGS));
  RETURN_TCL
}

/*****************************************************************************/
/*                                                                           */
/*                              tclctx_basic                                */
/*                                                                           */
/*****************************************************************************/

static void
NS(ProcessExit) (
  MQ_INT num
)
{
  Tcl_Exit(num);
}

static void
NS(ThreadExit) (
  MQ_INT num
)
{
  Tcl_FinalizeThread();
}

static 
int NS(LinkCreate) (NS_ARGS)
{
  SETUP_mqctx
  struct MqBufferLS * args = NULL;

  // command-line arguments to MqBufferLS
  if (objc-skip > 0) {
    int i;
    args = MqBufferLCreate (objc-skip+1);
    MqBufferLAppendC (args, (const MQ_STR) Tcl_GetNameOfExecutable());
    for (i = skip; i < objc; i++) {
      MqBufferLAppendC (args, Tcl_GetString (objv[i]));
    }
  }

  // create Context
  ErrorMqToTclWithCheck (MqLinkCreate(mqctx, &args));
  RETURN_TCL
}

static 
int NS(LinkCreateChild) (NS_ARGS)
{
  // check for connected
  struct MqS * parent;

  // get the parent tclctx
  if (objc < 3 || NS(GetClientData) (interp, objv[skip], (MQ_PTR*) &parent) == TCL_ERROR) {
    Tcl_WrongNumArgs (interp, 2, objv, "parent ...");
    return TCL_ERROR;
  } else {
    SETUP_mqctx
    struct MqBufferLS *args = NULL;
    int i;

    skip++;

    // copy data entries
    MqErrorCheck (MqSetupDup (mqctx, parent));

    // command-line arguments to MqBufferLS
    if (objc-skip > 0) {
      args = MqBufferLCreate (objc-skip+1);
      MqBufferLAppendC (args, (const MQ_STR) Tcl_GetNameOfExecutable());
      for (i = skip; i < objc; i++) {
	MqBufferLAppendC (args, Tcl_GetString (objv[i]));
      }
    }

    // create Context
    ErrorMqToTclWithCheck (MqLinkCreateChild(mqctx, parent, &args));
    RETURN_TCL
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                 public                                    */
/*                                                                           */
/*****************************************************************************/

static int NS(Exit) (NS_ARGS)
{
  CHECK_NOARGS
  MqExit (MQCTX);
  RETURN_TCL
}

static int NS(Delete) (NS_ARGS)
{
  CHECK_NOARGS
  Tcl_DeleteCommandFromToken (interp, tclctx->command);
  RETURN_TCL
}

static int NS(dict) (NS_ARGS)
{
  enum commandE { SET, GET, UNSET, EXISTS };
  static MQ_CST commandA[] = { "set", "get", "unset", "exists", NULL };
  int index;

  if (objc < 4) {
    Tcl_WrongNumArgs (interp, 2, objv, "set|get|unset|exists key ...");
    return TCL_ERROR;
  }

  TclErrorCheck (Tcl_GetIndexFromObj(interp, objv[2], commandA, "dict command", 0, &index));

  if (tclctx->dict == NULL) {
    tclctx->dict = Tcl_NewDictObj();
    Tcl_IncrRefCount(tclctx->dict);
  }

  Tcl_ResetResult(interp);
  switch ((enum commandE) index) {
    case GET: {
      Tcl_Obj *dict = tclctx->dict;
      int idx;
      for (idx = 3; idx < objc; idx++) {
	TclErrorCheck (Tcl_DictObjGet (interp, dict, objv[3], &dict));
      }
      if (dict != NULL) Tcl_SetObjResult (interp, dict);
      break;
    }
    case SET: {
      if (objc < 5) {
	Tcl_WrongNumArgs (interp, 2, objv, "set key value");
	return TCL_ERROR;
      }
      if (objc > 5) {
	TclErrorCheck (Tcl_DictObjPutKeyList (interp, tclctx->dict, objc-4, objv+3, objv[objc-1]));
	Tcl_SetObjResult (interp, objv[objc-1]);
      } else { // objc == 5
	TclErrorCheck (Tcl_DictObjPut (interp, tclctx->dict, objv[3], objv[4]));
	Tcl_SetObjResult (interp, objv[4]);
      }
      break;
    }
    case UNSET: {
      if (objc > 4) {
	TclErrorCheck (Tcl_DictObjRemoveKeyList (interp, tclctx->dict, objc-3, objv+3));
      } else { // objc == 4
	TclErrorCheck (Tcl_DictObjRemove (interp, tclctx->dict, objv[3]));
      }
    }
    case EXISTS: {
      Tcl_Obj *dict = tclctx->dict;
      int idx;
      for (idx = 3; idx < objc; idx++) {
	TclErrorCheck (Tcl_DictObjGet (interp, dict, objv[3], &dict));
      }
      Tcl_SetObjResult (interp, Tcl_NewBooleanObj(dict != NULL));
      break;
    }
  }

  return TCL_OK;
}

// public interfaces

int NS(ReadY) (NS_ARGS);
int NS(ReadO) (NS_ARGS);
int NS(ReadS) (NS_ARGS);
int NS(ReadI) (NS_ARGS);
int NS(ReadF) (NS_ARGS);
int NS(ReadW) (NS_ARGS);
int NS(ReadD) (NS_ARGS);
int NS(ReadC) (NS_ARGS);
int NS(ReadB) (NS_ARGS);
int NS(ReadN) (NS_ARGS);
int NS(ReadBDY) (NS_ARGS);
int NS(ReadU) (NS_ARGS);
int NS(ReadL_START) (NS_ARGS);
int NS(ReadL_END) (NS_ARGS);
int NS(ReadGetNumItems) (NS_ARGS);
int NS(ReadItemExists) (NS_ARGS);
int NS(ReadUndo) (NS_ARGS);
int NS(ReadALL) (NS_ARGS);
int NS(ReadProxy) (NS_ARGS);

int NS(SendSTART) (NS_ARGS);
int NS(SendEND) (NS_ARGS);
int NS(SendEND_AND_WAIT) (NS_ARGS);
int NS(SendEND_AND_CALLBACK) (NS_ARGS);
int NS(SendRETURN) (NS_ARGS);
int NS(SendERROR) (NS_ARGS);
int NS(SendY) (NS_ARGS);
int NS(SendO) (NS_ARGS);
int NS(SendS) (NS_ARGS);
int NS(SendI) (NS_ARGS);
int NS(SendF) (NS_ARGS);
int NS(SendW) (NS_ARGS);
int NS(SendD) (NS_ARGS);
int NS(SendC) (NS_ARGS);
int NS(SendB) (NS_ARGS);
int NS(SendN) (NS_ARGS);
int NS(SendBDY) (NS_ARGS);
int NS(SendU) (NS_ARGS);
int NS(SendL_START) (NS_ARGS);
int NS(SendL_END) (NS_ARGS);
int NS(SendAll) (NS_ARGS);

int NS(ConfigReset) (NS_ARGS);
int NS(ConfigSetBuffersize) (NS_ARGS);
int NS(ConfigSetDebug) (NS_ARGS);
int NS(ConfigSetTimeout) (NS_ARGS);
int NS(ConfigSetName) (NS_ARGS);
int NS(ConfigSetSrvName) (NS_ARGS);
int NS(ConfigSetIdent) (NS_ARGS);
int NS(ConfigCheckIdent) (NS_ARGS);
int NS(ConfigSetIsSilent) (NS_ARGS);
int NS(ConfigSetIsServer) (NS_ARGS);
int NS(ConfigSetIsString) (NS_ARGS);
int NS(ConfigSetServerSetup) (NS_ARGS);
int NS(ConfigSetServerCleanup) (NS_ARGS);
int NS(ConfigSetBgError) (NS_ARGS);
int NS(ConfigSetFactory) (NS_ARGS);
int NS(ConfigSetIoUds) (NS_ARGS);
int NS(ConfigSetIoTcp) (NS_ARGS);
int NS(ConfigSetIoPipe) (NS_ARGS);
int NS(ConfigSetStartAs) (NS_ARGS);
int NS(ConfigSetDaemon) (NS_ARGS);
int NS(ConfigGetIsString) (NS_ARGS);
int NS(ConfigGetIsSilent) (NS_ARGS);
int NS(ConfigGetIsServer) (NS_ARGS);
int NS(ConfigGetIsParent) (NS_ARGS);
int NS(ConfigGetIsSlave) (NS_ARGS);
int NS(ConfigGetIsConnected) (NS_ARGS);
int NS(ConfigGetDebug) (NS_ARGS);
int NS(ConfigGetBuffersize) (NS_ARGS);
int NS(ConfigGetTimeout) (NS_ARGS);
int NS(ConfigGetToken) (NS_ARGS);
int NS(ConfigGetIsTransaction) (NS_ARGS);
int NS(ConfigGetCtxId) (NS_ARGS);
int NS(ConfigGetName) (NS_ARGS);
int NS(ConfigGetSrvName) (NS_ARGS);
int NS(ConfigGetIdent) (NS_ARGS);
int NS(ConfigGetParent) (NS_ARGS);
int NS(ConfigGetMaster) (NS_ARGS);
int NS(ConfigGetFilter) (NS_ARGS);
int NS(ConfigGetIoUdsFile) (NS_ARGS);
int NS(ConfigGetIoTcpHost) (NS_ARGS);
int NS(ConfigGetIoTcpPort) (NS_ARGS);
int NS(ConfigGetIoTcpMyHost) (NS_ARGS);
int NS(ConfigGetIoTcpMyPort) (NS_ARGS);
int NS(ConfigGetIoPipeSocket) (NS_ARGS);
int NS(ConfigGetStartAs) (NS_ARGS);

int NS(ServiceProxy) (NS_ARGS);
int NS(ServiceCreate) (NS_ARGS);
int NS(ServiceDelete) (NS_ARGS);

int NS(ErrorC) (NS_ARGS);
int NS(ErrorSet) (NS_ARGS);
int NS(ErrorGetText) (NS_ARGS);
int NS(ErrorGetNum) (NS_ARGS);
int NS(ErrorGetCode) (NS_ARGS);
int NS(ErrorReset) (NS_ARGS);
int NS(ErrorPrint) (NS_ARGS);

int NS(SlaveWorker) (NS_ARGS);
int NS(SlaveCreate) (NS_ARGS);
int NS(SlaveDelete) (NS_ARGS);
int NS(SlaveGet) (NS_ARGS);

int NS(MqS_Cmd) (
  ClientData clientData,
  Tcl_Interp * interp,
  int objc,
  Tcl_Obj * const objv[]
)
{
  int index;
  struct TclContextS * tclctx = (struct TclContextS *) clientData;

  const static struct LookupKeyword keys[] = {

// READ

    { "ReadY",		      NS(ReadY)			},
    { "ReadO",		      NS(ReadO)			},
    { "ReadS",		      NS(ReadS)			},
    { "ReadI",		      NS(ReadI)			},
    { "ReadF",		      NS(ReadF)			},
    { "ReadW",		      NS(ReadW)			},
    { "ReadD",		      NS(ReadD)			},
    { "ReadC",		      NS(ReadC)			},
    { "ReadB",		      NS(ReadB)			},
    { "ReadN",		      NS(ReadN)			},
    { "ReadBDY",	      NS(ReadBDY)		},
    { "ReadU",		      NS(ReadU)			},
    { "ReadL_START",	      NS(ReadL_START)		},
    { "ReadL_END",	      NS(ReadL_END)		},
    { "ReadGetNumItems",      NS(ReadGetNumItems)	},
    { "ReadItemExists",	      NS(ReadItemExists)	},
    { "ReadUndo",	      NS(ReadUndo)		},
    { "ReadAll",	      NS(ReadALL)		},
    { "ReadProxy",	      NS(ReadProxy)		},

// SEND

    { "SendSTART",	      NS(SendSTART)		},
    { "SendEND",	      NS(SendEND)		},
    { "SendEND_AND_WAIT",     NS(SendEND_AND_WAIT)	},
    { "SendEND_AND_CALLBACK", NS(SendEND_AND_CALLBACK)	},
    { "SendRETURN",	      NS(SendRETURN)		},
    { "SendERROR",	      NS(SendERROR)		},
    { "SendY",		      NS(SendY)			},
    { "SendO",		      NS(SendO)			},
    { "SendS",		      NS(SendS)			},
    { "SendI",		      NS(SendI)			},
    { "SendF",		      NS(SendF)			},
    { "SendW",		      NS(SendW)			},
    { "SendD",		      NS(SendD)			},
    { "SendC",		      NS(SendC)			},
    { "SendB",		      NS(SendB)			},
    { "SendN",		      NS(SendN)			},
    { "SendBDY",	      NS(SendBDY)		},
    { "SendU",		      NS(SendU)			},
    { "SendL_START",	      NS(SendL_START)		},
    { "SendL_END",	      NS(SendL_END)		},
    { "SendAll",	      NS(SendAll)		},

// CONFIG

    { "ConfigReset",		  NS(ConfigReset)	      },
    { "ConfigSetBuffersize",	  NS(ConfigSetBuffersize)     },
    { "ConfigSetDebug",		  NS(ConfigSetDebug)	      },
    { "ConfigSetTimeout",	  NS(ConfigSetTimeout)	      },
    { "ConfigSetName",		  NS(ConfigSetName)	      },
    { "ConfigSetSrvName",	  NS(ConfigSetSrvName)	      },
    { "ConfigSetIdent",		  NS(ConfigSetIdent)	      },
    { "ConfigCheckIdent",	  NS(ConfigCheckIdent)	      },
    { "ConfigSetIsSilent",	  NS(ConfigSetIsSilent)	      },
    { "ConfigSetIsServer",	  NS(ConfigSetIsServer)	      },
    { "ConfigSetIsString",	  NS(ConfigSetIsString)	      },
    { "ConfigSetServerSetup",	  NS(ConfigSetServerSetup)    },
    { "ConfigSetServerCleanup",	  NS(ConfigSetServerCleanup)  },
    { "ConfigSetBgError",	  NS(ConfigSetBgError)	      },
    { "ConfigSetFactory",	  NS(ConfigSetFactory)	      },
    { "ConfigSetIoUds",		  NS(ConfigSetIoUds)	      },
    { "ConfigSetIoTcp",		  NS(ConfigSetIoTcp)	      },
    { "ConfigSetIoPipe",	  NS(ConfigSetIoPipe)	      },
    { "ConfigSetStartAs",	  NS(ConfigSetStartAs)	      },
    { "ConfigSetDaemon",	  NS(ConfigSetDaemon)	      },
    { "ConfigGetIsString",	  NS(ConfigGetIsString)	      },
    { "ConfigGetIsSilent",	  NS(ConfigGetIsSilent)	      },
    { "ConfigGetIsServer",	  NS(ConfigGetIsServer)	      },
    { "ConfigGetIsParent",	  NS(ConfigGetIsParent)	      },
    { "ConfigGetIsSlave",	  NS(ConfigGetIsSlave)	      },
    { "ConfigGetIsConnected",	  NS(ConfigGetIsConnected)    },
    { "ConfigGetDebug",		  NS(ConfigGetDebug)	      },
    { "ConfigGetBuffersize",	  NS(ConfigGetBuffersize)     },
    { "ConfigGetTimeout",	  NS(ConfigGetTimeout)	      },
    { "ConfigGetDebug",		  NS(ConfigGetDebug)	      },
    { "ConfigGetToken",		  NS(ConfigGetToken)	      },
    { "ConfigGetIsTransaction",	  NS(ConfigGetIsTransaction)  },
    { "ConfigGetCtxId",		  NS(ConfigGetCtxId)	      },
    { "ConfigGetName",		  NS(ConfigGetName)	      },
    { "ConfigGetSrvName",	  NS(ConfigGetSrvName)	      },
    { "ConfigGetIdent",		  NS(ConfigGetIdent)	      },
    { "ConfigGetParent",	  NS(ConfigGetParent)	      },
    { "ConfigGetMaster",	  NS(ConfigGetMaster)	      },
    { "ConfigGetFilter",	  NS(ConfigGetFilter)	      },
    { "ConfigGetIoUdsFile",	  NS(ConfigGetIoUdsFile)      },
    { "ConfigGetIoTcpHost",	  NS(ConfigGetIoTcpHost)      },
    { "ConfigGetIoTcpPort",	  NS(ConfigGetIoTcpPort)      },
    { "ConfigGetIoTcpMyHost",	  NS(ConfigGetIoTcpMyHost)    },
    { "ConfigGetIoTcpMyPort",	  NS(ConfigGetIoTcpMyPort)    },
    { "ConfigGetIoPipeSocket",	  NS(ConfigGetIoPipeSocket)   },
    { "ConfigGetStartAs",	  NS(ConfigGetStartAs)	      },

// SERVICE

    { "ServiceProxy",	      NS(ServiceProxy)		},
    { "ServiceCreate",	      NS(ServiceCreate)		},
    { "ServiceDelete",	      NS(ServiceDelete)		},

// CONTEXT

    { "ProcessEvent",	      NS(ProcessEvent)		},
    { "RenameTo",	      NS(RenameTo)		},
    { "LinkCreate",	      NS(LinkCreate)		},
    { "LinkCreateChild",      NS(LinkCreateChild)	},
    { "LinkDelete",	      NS(LinkDelete)		},
    { "Exit",		      NS(Exit)			},
    { "Delete",		      NS(Delete)		},
    { "dict",		      NS(dict)			},

// ERROR

    { "ErrorC",		      NS(ErrorC)		},
    { "ErrorSet",	      NS(ErrorSet)		},
    { "ErrorGetText",	      NS(ErrorGetText)		},
    { "ErrorGetNum",	      NS(ErrorGetNum)		},
    { "ErrorGetCode",	      NS(ErrorGetCode)		},
    { "ErrorReset",	      NS(ErrorReset)		},
    { "ErrorPrint",	      NS(ErrorPrint)		},

// SLAVE

    { "SlaveWorker",	      NS(SlaveWorker)		},
    { "SlaveCreate",	      NS(SlaveCreate)		},
    { "SlaveDelete",	      NS(SlaveDelete)		},
    { "SlaveGet",	      NS(SlaveGet)		},

    { NULL,		      NULL			}
  };

  if (objc < 2) {
    Tcl_WrongNumArgs (interp, 1, objv, "subcommand ...");
    return TCL_ERROR;
  }

  TclErrorCheck (Tcl_GetIndexFromObjStruct (interp, objv[1], &keys, 
      sizeof(struct LookupKeyword), "subcommand", 0, &index));

  return (*keys[index].keyF) (interp, tclctx, 2, objc, objv);
}

void
NS(MqS_Free) (
  ClientData clientData
)
{
  if (clientData == NULL) {
    return;
  } else {
    struct TclContextS *tclctx = (struct TclContextS *) clientData;
    SETUP_self
    struct MqS * mqctx = (struct MqS *) tclctx;
    Tcl_Obj* dict = tclctx->dict;
    // we delete the command using "Tcl_DeleteObjCommand" the "Factory" is useless -> delete
    mqctx->setup.Factory.Delete.fCall = NULL;
    mqctx->setup.fEvent = NULL;
    // delete the context
    MqContextDelete(&mqctx);
    if (self != NULL)  Tcl_DecrRefCount(self);
    if (dict != NULL)  Tcl_DecrRefCount(dict);
  }
}

int
NS(MqS_Init) (
  Tcl_Interp * interp,
  int objc,
  Tcl_Obj * const objv[]
)
{
  struct TclContextS * tclctx = (struct TclContextS *) MqContextCreate(sizeof (*tclctx), NULL);
  struct MqBufferS * const buf = MQCTX->temp;

  if (2 != objc) {
    Tcl_WrongNumArgs(interp, 2, objv, "");
    goto error;
  }

  // create tcl command
  tclctx->mqctx.threadData = (MQ_PTR)interp;
  MqBufferSetV(buf, "<MqS-%p>", tclctx);
  tclctx->command = Tcl_CreateObjCommand (interp, buf->cur.C, NS(MqS_Cmd), tclctx, NS(MqS_Free));
  tclctx->mqctx.self = (void*) Tcl_NewStringObj(buf->cur.C,-1);
  Tcl_IncrRefCount(SELF);
  Tcl_SetObjResult (interp, SELF);

  // set configuration data
  tclctx->mqctx.setup.Child.fCreate   = MqDefaultLinkCreate;
  tclctx->mqctx.setup.Parent.fCreate  = MqDefaultLinkCreate;
  tclctx->mqctx.setup.fProcessExit    = NS(ProcessExit);
  tclctx->mqctx.setup.fThreadExit     = NS(ThreadExit);
  tclctx->mqctx.setup.Factory.Delete.fCall = NS(FactoryDelete);
  tclctx->mqctx.setup.fEvent	      = NS(EventLink);

  if (Tcl_GetVar2Ex(interp,"tcl_platform","threaded",TCL_GLOBAL_ONLY) != NULL) {
    MqConfigSetIgnoreFork (&tclctx->mqctx, MQ_YES);
  }

  RETURN_TCL
}

