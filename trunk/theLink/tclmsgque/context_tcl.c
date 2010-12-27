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

/*****************************************************************************/
/*                                                                           */
/*                                 public                                    */
/*                                                                           */
/*****************************************************************************/

static int NS(LogC) (NS_ARGS)
{
  MQ_CST str,proc;
  MQ_INT level;
  CHECK_C(proc)
  CHECK_I(level)
  CHECK_C(str)
  CHECK_NOARGS
  MqLogC(MQCTX, proc, level, str);
  RETURN_TCL
}

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
  enum commandE { SET, GET, UNSET, EXISTS, LAPPEND };
  static MQ_CST commandA[] = { "set", "get", "unset", "exists", "lappend", NULL };
  int index;

  if (objc < 4) {
    Tcl_WrongNumArgs (interp, 2, objv, "set|get|unset|exists|lappend key ...");
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
	Tcl_WrongNumArgs (interp, 2, objv, "set key... value");
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
    case LAPPEND: {
      Tcl_Obj *dict = tclctx->dict;
      int idx;
      if (objc < 5) {
	Tcl_WrongNumArgs (interp, 2, objv, "lappend key... value");
	return TCL_ERROR;
      }
      for (idx = 3; idx < objc-1; idx++) {
	TclErrorCheck (Tcl_DictObjGet (interp, dict, objv[3], &dict));
      }
      if (dict == NULL) {
	dict = objv[idx];
      } else {
	dict = Tcl_DuplicateObj(dict);
	TclErrorCheck (Tcl_ListObjAppendElement (interp, dict, objv[idx]));
      }
      if (objc > 5) {
	TclErrorCheck (Tcl_DictObjPutKeyList (interp, tclctx->dict, objc-4, objv+3, dict));
	Tcl_SetObjResult (interp, objv[objc-1]);
      } else { // objc == 5
	TclErrorCheck (Tcl_DictObjPut (interp, tclctx->dict, objv[3], dict));
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
int NS(ReadT_START) (NS_ARGS);
int NS(ReadT_END) (NS_ARGS);
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
int NS(SendT_START) (NS_ARGS);
int NS(SendT_END) (NS_ARGS);
int NS(SendAll) (NS_ARGS);

int NS(ConfigReset) (NS_ARGS);
int NS(ConfigSetBuffersize) (NS_ARGS);
int NS(ConfigSetDebug) (NS_ARGS);
int NS(ConfigSetTimeout) (NS_ARGS);
int NS(ConfigSetName) (NS_ARGS);
int NS(ConfigSetSrvName) (NS_ARGS);
int NS(FactoryCtxIdent) (NS_ARGS);
int NS(ConfigSetIsSilent) (NS_ARGS);
int NS(ConfigSetIsServer) (NS_ARGS);
int NS(ConfigSetIsString) (NS_ARGS);
int NS(ConfigSetIgnoreExit) (NS_ARGS);
int NS(ConfigSetEvent) (NS_ARGS);
int NS(ConfigSetServerSetup) (NS_ARGS);
int NS(ConfigSetServerCleanup) (NS_ARGS);
int NS(ConfigSetBgError) (NS_ARGS);
int NS(FactoryCtxNew) (NS_ARGS);
int NS(FactoryCtxDefault) (NS_ARGS);
int NS(ConfigSetIoUdsFile) (NS_ARGS);
int NS(ConfigSetIoTcp) (NS_ARGS);
int NS(ConfigSetIoPipeSocket) (NS_ARGS);
int NS(ConfigSetStartAs) (NS_ARGS);
int NS(ConfigSetDaemon) (NS_ARGS);
int NS(ConfigGetIsString) (NS_ARGS);
int NS(ConfigGetIsSilent) (NS_ARGS);
int NS(ConfigGetIsServer) (NS_ARGS);
int NS(ConfigGetDebug) (NS_ARGS);
int NS(ConfigGetBuffersize) (NS_ARGS);
int NS(ConfigGetTimeout) (NS_ARGS);
int NS(ConfigGetName) (NS_ARGS);
int NS(ConfigGetSrvName) (NS_ARGS);
int NS(ConfigGetIdent) (NS_ARGS);
int NS(ConfigGetIoUdsFile) (NS_ARGS);
int NS(ConfigGetIoTcpHost) (NS_ARGS);
int NS(ConfigGetIoTcpPort) (NS_ARGS);
int NS(ConfigGetIoTcpMyHost) (NS_ARGS);
int NS(ConfigGetIoTcpMyPort) (NS_ARGS);
int NS(ConfigGetIoPipeSocket) (NS_ARGS);
int NS(ConfigGetStartAs) (NS_ARGS);
int NS(ConfigGetStatusIs) (NS_ARGS);

int NS(LinkIsParent) (NS_ARGS);
int NS(LinkIsConnected) (NS_ARGS);
int NS(LinkGetCtxId) (NS_ARGS);
int NS(LinkGetParent) (NS_ARGS);
int NS(LinkGetTargetIdent) (NS_ARGS);
int NS(LinkCreate) (NS_ARGS);
int NS(LinkCreateChild) (NS_ARGS);
int NS(LinkDelete) (NS_ARGS);
int NS(LinkConnect) (NS_ARGS);

int NS(ServiceGetToken) (NS_ARGS);
int NS(ServiceIsTransaction) (NS_ARGS);
int NS(ServiceGetFilter) (NS_ARGS);
int NS(ServiceProxy) (NS_ARGS);
int NS(ServiceCreate) (NS_ARGS);
int NS(ServiceDelete) (NS_ARGS);
int NS(ProcessEvent) (NS_ARGS);

int NS(ErrorC) (NS_ARGS);
int NS(ErrorSet) (NS_ARGS);
int NS(ErrorSetCONTINUE) (NS_ARGS);
int NS(ErrorIsEXIT) (NS_ARGS);
int NS(ErrorGetText) (NS_ARGS);
int NS(ErrorGetNum) (NS_ARGS);
int NS(ErrorGetCode) (NS_ARGS);
int NS(ErrorReset) (NS_ARGS);
int NS(ErrorPrint) (NS_ARGS);

int NS(SlaveWorker) (NS_ARGS);
int NS(SlaveCreate) (NS_ARGS);
int NS(SlaveDelete) (NS_ARGS);
int NS(SlaveGet) (NS_ARGS);
int NS(SlaveGetMaster) (NS_ARGS);
int NS(SlaveIs) (NS_ARGS);

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
    { "ReadT_START",	      NS(ReadT_START)		},
    { "ReadT_END",	      NS(ReadT_END)		},
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
    { "SendT_START",	      NS(SendT_START)		},
    { "SendT_END",	      NS(SendT_END)		},
    { "SendAll",	      NS(SendAll)		},

// CONFIG

    { "ConfigReset",		  NS(ConfigReset)	      },
    { "ConfigSetBuffersize",	  NS(ConfigSetBuffersize)     },
    { "ConfigSetDebug",		  NS(ConfigSetDebug)	      },
    { "ConfigSetTimeout",	  NS(ConfigSetTimeout)	      },
    { "ConfigSetName",		  NS(ConfigSetName)	      },
    { "ConfigSetSrvName",	  NS(ConfigSetSrvName)	      },
    { "FactoryCtxIdent",	  NS(FactoryCtxIdent)	      },
    { "ConfigSetIsSilent",	  NS(ConfigSetIsSilent)	      },
    { "ConfigSetIsServer",	  NS(ConfigSetIsServer)	      },
    { "ConfigSetIsString",	  NS(ConfigSetIsString)	      },
    { "ConfigSetIgnoreExit",	  NS(ConfigSetIgnoreExit)     },
    { "ConfigSetEvent",		  NS(ConfigSetEvent)	      },
    { "ConfigSetServerSetup",	  NS(ConfigSetServerSetup)    },
    { "ConfigSetServerCleanup",	  NS(ConfigSetServerCleanup)  },
    { "ConfigSetBgError",	  NS(ConfigSetBgError)	      },
    { "FactoryCtxNew",		  NS(FactoryCtxNew)	      },
    { "FactoryCtxDefault",	  NS(FactoryCtxDefault)	      },
    { "ConfigSetIoUdsFile",	  NS(ConfigSetIoUdsFile)      },
    { "ConfigSetIoTcp",		  NS(ConfigSetIoTcp)	      },
    { "ConfigSetIoPipeSocket",	  NS(ConfigSetIoPipeSocket)   },
    { "ConfigSetStartAs",	  NS(ConfigSetStartAs)	      },
    { "ConfigSetDaemon",	  NS(ConfigSetDaemon)	      },
    { "ConfigGetIsString",	  NS(ConfigGetIsString)	      },
    { "ConfigGetIsSilent",	  NS(ConfigGetIsSilent)	      },
    { "ConfigGetIsServer",	  NS(ConfigGetIsServer)	      },
    { "ConfigGetDebug",		  NS(ConfigGetDebug)	      },
    { "ConfigGetBuffersize",	  NS(ConfigGetBuffersize)     },
    { "ConfigGetTimeout",	  NS(ConfigGetTimeout)	      },
    { "ConfigGetDebug",		  NS(ConfigGetDebug)	      },
    { "ConfigGetName",		  NS(ConfigGetName)	      },
    { "ConfigGetSrvName",	  NS(ConfigGetSrvName)	      },
    { "ConfigGetIdent",		  NS(ConfigGetIdent)	      },
    { "ConfigGetIoUdsFile",	  NS(ConfigGetIoUdsFile)      },
    { "ConfigGetIoTcpHost",	  NS(ConfigGetIoTcpHost)      },
    { "ConfigGetIoTcpPort",	  NS(ConfigGetIoTcpPort)      },
    { "ConfigGetIoTcpMyHost",	  NS(ConfigGetIoTcpMyHost)    },
    { "ConfigGetIoTcpMyPort",	  NS(ConfigGetIoTcpMyPort)    },
    { "ConfigGetIoPipeSocket",	  NS(ConfigGetIoPipeSocket)   },
    { "ConfigGetStartAs",	  NS(ConfigGetStartAs)	      },
    { "ConfigGetStatusIs",	  NS(ConfigGetStatusIs)	      },

// SERVICE

    { "ServiceGetFilter",	  NS(ServiceGetFilter)	      },
    { "ServiceIsTransaction",	  NS(ServiceIsTransaction)    },
    { "ServiceGetToken",	  NS(ServiceGetToken)	      },
    { "ServiceProxy",		  NS(ServiceProxy)	      },
    { "ServiceCreate",		  NS(ServiceCreate)	      },
    { "ServiceDelete",		  NS(ServiceDelete)	      },
    { "ProcessEvent",		  NS(ProcessEvent)	      },

// CONTEXT

    { "RenameTo",		  NS(RenameTo)		      },
    { "Exit",			  NS(Exit)		      },
    { "Delete",			  NS(Delete)		      },
    { "LogC",			  NS(LogC)		      },
    { "dict",			  NS(dict)		      },

// Link

    { "LinkIsParent",		  NS(LinkIsParent)	      },
    { "LinkIsConnected",	  NS(LinkIsConnected)	      },
    { "LinkGetCtxId",		  NS(LinkGetCtxId)	      },
    { "LinkGetParent",		  NS(LinkGetParent)	      },
    { "LinkGetTargetIdent",	  NS(LinkGetTargetIdent)      },
    { "LinkCreate",		  NS(LinkCreate)	      },
    { "LinkCreateChild",	  NS(LinkCreateChild)	      },
    { "LinkDelete",		  NS(LinkDelete)	      },
    { "LinkConnect",		  NS(LinkConnect)	      },

// ERROR

    { "ErrorC",			  NS(ErrorC)		      },
    { "ErrorSet",		  NS(ErrorSet)		      },
    { "ErrorSetCONTINUE",	  NS(ErrorSetCONTINUE)	      },
    { "ErrorIsEXIT",		  NS(ErrorIsEXIT)	      },
    { "ErrorGetText",		  NS(ErrorGetText)	      },
    { "ErrorGetNum",		  NS(ErrorGetNum)	      },
    { "ErrorGetCode",		  NS(ErrorGetCode)	      },
    { "ErrorReset",		  NS(ErrorReset)	      },
    { "ErrorPrint",		  NS(ErrorPrint)	      },

// SLAVE

    { "SlaveWorker",		  NS(SlaveWorker)	      },
    { "SlaveCreate",		  NS(SlaveCreate)	      },
    { "SlaveDelete",		  NS(SlaveDelete)	      },
    { "SlaveGet",		  NS(SlaveGet)		      },
    { "SlaveGetMaster",		  NS(SlaveGetMaster)	      },
    { "SlaveIs",		  NS(SlaveIs)		      },

    { NULL,			  NULL			      }
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
    mqctx->setup.factory = NULL;
    mqctx->setup.Event.fCall = NULL;
    // delete the context
    MqContextDelete(&mqctx);
    if (self != NULL && Tcl_IsShared(self)) {
      Tcl_DecrRefCount(self);
    }
    if (dict != NULL && Tcl_IsShared(self))  {
      Tcl_DecrRefCount(dict);
    }
  }
}

int
NS(MqS_Init) (
  Tcl_Interp * interp,
  int objc,
  Tcl_Obj * const objv[]
)
{
  struct MqS * tmpl = NULL;
  int skip = 2;

  CHECK_MQS_OPT(tmpl)
  CHECK_NOARGS

  {
    struct TclContextS * tclctx = (struct TclContextS *) MqContextCreate(sizeof (*tclctx), tmpl);
    struct MqS * mqctx = MQCTX;
    struct MqBufferS * const buf = mqctx->temp;

    // create tcl command
    mqctx->threadData = (MQ_PTR)interp;
    MqBufferSetV(buf, "<MqS-%p>", tclctx);
    tclctx->command = Tcl_CreateObjCommand (interp, buf->cur.C, NS(MqS_Cmd), tclctx, NS(MqS_Free));
    mqctx->self = (void*) Tcl_NewStringObj(buf->cur.C,-1);
    Tcl_IncrRefCount(SELF);
    Tcl_SetObjResult (interp, SELF);

    // set basic configuration data
    MqConfigSetSetup (mqctx, 
      MqLinkDefault, NULL, MqLinkDefault, NULL, NS(ProcessExit), NS(ThreadExit)
    );

    // tcl does !not! support fork for a threaded interpreter
    if (tmpl == NULL && Tcl_GetVar2Ex(interp,"tcl_platform","threaded",TCL_GLOBAL_ONLY) != NULL) {
      MqConfigSetIgnoreFork (mqctx, MQ_YES);
    }

    // tcl-special -> default-event-handler
    MqConfigSetEvent (mqctx, NS(EventLink), NULL, NULL, NULL);
  }

  RETURN_TCL
}

