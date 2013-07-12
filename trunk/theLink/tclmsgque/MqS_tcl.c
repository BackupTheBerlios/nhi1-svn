/**
 *  \file       theLink/tclmsgque/MqS_tcl.c
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
int NS(RenameTo) (MqS_ARGS)
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
/*                             public storage                                */
/*                                                                           */
/*****************************************************************************/

static int NS(StorageOpen) (MqS_ARGS)
{
  SETUP_mqctx
  MQ_CST storageDir = NULL;
  CHECK_C(storageDir)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqStorageOpen (mqctx, storageDir));
  RETURN_TCL
}

static int NS(StorageClose) (MqS_ARGS)
{
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqStorageClose (mqctx));
  RETURN_TCL
}

static int NS(StorageInsert) (MqS_ARGS)
{
  MQ_WID transId;
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqStorageInsert (mqctx, &transId));
  Tcl_SetObjResult(interp, Tcl_NewWideIntObj(transId));
  RETURN_TCL
}

static int NS(StorageCount) (MqS_ARGS)
{
  MQ_WID count;
  SETUP_mqctx
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqStorageCount (mqctx, &count));
  Tcl_SetObjResult(interp, Tcl_NewWideIntObj(count));
  RETURN_TCL
}

static int NS(StorageSelect) (MqS_ARGS)
{
  MQ_WID transId = 0LL;
  SETUP_mqctx
  CHECK_W_OPT(transId)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqStorageSelect (mqctx, &transId));
  Tcl_SetObjResult(interp, Tcl_NewWideIntObj(transId));
  RETURN_TCL
}

static int NS(StorageDelete) (MqS_ARGS)
{
  MQ_WID transId;
  SETUP_mqctx
  CHECK_W(transId)
  CHECK_NOARGS
  ErrorMqToTclWithCheck (MqStorageDelete (mqctx, transId));
  RETURN_TCL
}

/*****************************************************************************/
/*                                                                           */
/*                                 public                                    */
/*                                                                           */
/*****************************************************************************/

static int NS(LogC) (MqS_ARGS)
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

static int NS(ContextGetBuffer) (MqS_ARGS)
{
  CHECK_NOARGS
  NS(MqBufferS_New) (interp, MqContextGetBuffer(MQCTX));
  RETURN_TCL
}

static int NS(Exit) (MqS_ARGS)
{
  CHECK_NOARGS
  MqExit (MQCTX);
  RETURN_TCL
}

static int NS(Delete) (MqS_ARGS)
{
  CHECK_NOARGS
  Tcl_DeleteCommandFromToken (interp, tclctx->command);
  RETURN_TCL
}

static int NS(dict) (MqS_ARGS)
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

int NS(ReadY) (MqS_ARGS);
int NS(ReadO) (MqS_ARGS);
int NS(ReadS) (MqS_ARGS);
int NS(ReadI) (MqS_ARGS);
int NS(ReadF) (MqS_ARGS);
int NS(ReadW) (MqS_ARGS);
int NS(ReadD) (MqS_ARGS);
int NS(ReadC) (MqS_ARGS);
int NS(ReadB) (MqS_ARGS);
int NS(ReadN) (MqS_ARGS);
int NS(ReadDUMP) (MqS_ARGS);
int NS(ReadLOAD) (MqS_ARGS);
int NS(ReadU) (MqS_ARGS);
int NS(ReadL_START) (MqS_ARGS);
int NS(ReadL_END) (MqS_ARGS);
int NS(ReadT_START) (MqS_ARGS);
int NS(ReadT_END) (MqS_ARGS);
int NS(ReadGetNumItems) (MqS_ARGS);
int NS(ReadItemExists) (MqS_ARGS);
int NS(ReadUndo) (MqS_ARGS);
int NS(ReadALL) (MqS_ARGS);
int NS(ReadProxy) (MqS_ARGS);
int NS(ReadForward) (MqS_ARGS);

int NS(SendSTART) (MqS_ARGS);
int NS(SendEND) (MqS_ARGS);
int NS(SendEND_AND_WAIT) (MqS_ARGS);
int NS(SendEND_AND_CALLBACK) (MqS_ARGS);
int NS(SendRETURN) (MqS_ARGS);
int NS(SendERROR) (MqS_ARGS);
int NS(SendY) (MqS_ARGS);
int NS(SendO) (MqS_ARGS);
int NS(SendS) (MqS_ARGS);
int NS(SendI) (MqS_ARGS);
int NS(SendF) (MqS_ARGS);
int NS(SendW) (MqS_ARGS);
int NS(SendD) (MqS_ARGS);
int NS(SendC) (MqS_ARGS);
int NS(SendB) (MqS_ARGS);
int NS(SendN) (MqS_ARGS);
int NS(SendU) (MqS_ARGS);
int NS(SendL_START) (MqS_ARGS);
int NS(SendL_END) (MqS_ARGS);
int NS(SendT_START) (MqS_ARGS);
int NS(SendT_END) (MqS_ARGS);
int NS(SendAll) (MqS_ARGS);

int NS(ConfigReset) (MqS_ARGS);
int NS(ConfigSetBuffersize) (MqS_ARGS);
int NS(ConfigSetDebug) (MqS_ARGS);
int NS(ConfigSetTimeout) (MqS_ARGS);
int NS(ConfigSetName) (MqS_ARGS);
int NS(ConfigSetSrvName) (MqS_ARGS);
int NS(ConfigSetStorage) (MqS_ARGS);
int NS(ConfigSetIsSilent) (MqS_ARGS);
int NS(ConfigSetIsServer) (MqS_ARGS);
int NS(ConfigSetIsString) (MqS_ARGS);
int NS(ConfigSetIgnoreExit) (MqS_ARGS);
int NS(ConfigSetEvent) (MqS_ARGS);
int NS(ConfigSetServerSetup) (MqS_ARGS);
int NS(ConfigSetServerCleanup) (MqS_ARGS);
int NS(ConfigSetBgError) (MqS_ARGS);
int NS(ConfigSetIoUdsFile) (MqS_ARGS);
int NS(ConfigSetIoTcp) (MqS_ARGS);
int NS(ConfigSetIoPipeSocket) (MqS_ARGS);
int NS(ConfigSetStartAs) (MqS_ARGS);
int NS(ConfigSetDaemon) (MqS_ARGS);
int NS(ConfigGetIsString) (MqS_ARGS);
int NS(ConfigGetIsSilent) (MqS_ARGS);
int NS(ConfigGetIsServer) (MqS_ARGS);
int NS(ConfigGetDebug) (MqS_ARGS);
int NS(ConfigGetBuffersize) (MqS_ARGS);
int NS(ConfigGetTimeout) (MqS_ARGS);
int NS(ConfigGetName) (MqS_ARGS);
int NS(ConfigGetSrvName) (MqS_ARGS);
int NS(ConfigGetStorage) (MqS_ARGS);
int NS(ConfigGetIoUdsFile) (MqS_ARGS);
int NS(ConfigGetIoTcpHost) (MqS_ARGS);
int NS(ConfigGetIoTcpPort) (MqS_ARGS);
int NS(ConfigGetIoTcpMyHost) (MqS_ARGS);
int NS(ConfigGetIoTcpMyPort) (MqS_ARGS);
int NS(ConfigGetIoPipeSocket) (MqS_ARGS);
int NS(ConfigGetStartAs) (MqS_ARGS);
int NS(ConfigGetStatusIs) (MqS_ARGS);

int NS(LinkIsParent) (MqS_ARGS);
int NS(LinkIsConnected) (MqS_ARGS);
int NS(LinkGetCtxId) (MqS_ARGS);
int NS(LinkGetParent) (MqS_ARGS);
int NS(LinkGetTargetIdent) (MqS_ARGS);
int NS(LinkCreate) (MqS_ARGS);
int NS(LinkCreateChild) (MqS_ARGS);
int NS(LinkDelete) (MqS_ARGS);
int NS(LinkConnect) (MqS_ARGS);

int NS(ServiceGetToken) (MqS_ARGS);
int NS(ServiceIsTransaction) (MqS_ARGS);
int NS(ServiceGetFilter) (MqS_ARGS);
int NS(ServiceProxy) (MqS_ARGS);
int NS(ServiceStorage) (MqS_ARGS);
int NS(ServiceCreate) (MqS_ARGS);
int NS(ServiceDelete) (MqS_ARGS);
int NS(ProcessEvent) (MqS_ARGS);

int NS(ErrorC) (MqS_ARGS);
int NS(ErrorSet) (MqS_ARGS);
int NS(ErrorSetCONTINUE) (MqS_ARGS);
int NS(ErrorSetEXIT) (MqS_ARGS);
int NS(ErrorIsEXIT) (MqS_ARGS);
int NS(ErrorGetText) (MqS_ARGS);
int NS(ErrorGetNum) (MqS_ARGS);
int NS(ErrorGetCode) (MqS_ARGS);
int NS(ErrorReset) (MqS_ARGS);
int NS(ErrorPrint) (MqS_ARGS);

int NS(SlaveWorker) (MqS_ARGS);
int NS(SlaveCreate) (MqS_ARGS);
int NS(SlaveDelete) (MqS_ARGS);
int NS(SlaveGet) (MqS_ARGS);
int NS(SlaveGetMaster) (MqS_ARGS);
int NS(SlaveIs) (MqS_ARGS);

int NS(FactoryCtxSet) (MqS_ARGS);
int NS(FactoryCtxGet) (MqS_ARGS);
int NS(FactoryCtxIdentSet) (MqS_ARGS);
int NS(FactoryCtxIdentGet) (MqS_ARGS);

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
    { "ReadDUMP",	      NS(ReadDUMP)		},
    { "ReadLOAD",	      NS(ReadLOAD)		},
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
    { "ReadForward",	      NS(ReadForward)		},

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
    { "ConfigSetStorage",	  NS(ConfigSetStorage)	      },
    { "ConfigSetIsSilent",	  NS(ConfigSetIsSilent)	      },
    { "ConfigSetIsServer",	  NS(ConfigSetIsServer)	      },
    { "ConfigSetIsString",	  NS(ConfigSetIsString)	      },
    { "ConfigSetIgnoreExit",	  NS(ConfigSetIgnoreExit)     },
    { "ConfigSetEvent",		  NS(ConfigSetEvent)	      },
    { "ConfigSetServerSetup",	  NS(ConfigSetServerSetup)    },
    { "ConfigSetServerCleanup",	  NS(ConfigSetServerCleanup)  },
    { "ConfigSetBgError",	  NS(ConfigSetBgError)	      },
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
    { "ConfigGetStorage",	  NS(ConfigGetStorage)	      },
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
    { "ServiceStorage",		  NS(ServiceStorage)	      },
    { "ServiceCreate",		  NS(ServiceCreate)	      },
    { "ServiceDelete",		  NS(ServiceDelete)	      },
    { "ProcessEvent",		  NS(ProcessEvent)	      },

// CONTEXT

    { "RenameTo",		  NS(RenameTo)		      },
    { "Exit",			  NS(Exit)		      },
    { "Delete",			  NS(Delete)		      },
    { "LogC",			  NS(LogC)		      },
    { "ContextGetBuffer",	  NS(ContextGetBuffer)	      },

    { "dict",			  NS(dict)		      },

// Storage
    { "StorageOpen",		  NS(StorageOpen)	      },
    { "StorageClose",		  NS(StorageClose)	      },
    { "StorageInsert",		  NS(StorageInsert)	      },
    { "StorageCount",		  NS(StorageCount)	      },
    { "StorageSelect",		  NS(StorageSelect)	      },
    { "StorageDelete",		  NS(StorageDelete)	      },

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
    { "ErrorSetEXIT",	          NS(ErrorSetEXIT)	      },
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

// FACTORY
//
    { "FactoryCtxSet",		  NS(FactoryCtxSet)	      },
    { "FactoryCtxGet",		  NS(FactoryCtxGet)	      },
    { "FactoryCtxIdentSet",	  NS(FactoryCtxIdentSet)      },
    { "FactoryCtxIdentGet",	  NS(FactoryCtxIdentGet)      },

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
    struct MqBufferS * const buf = mqctx->ctxbuf;

    // create tcl command
    mqctx->threadData = (MQ_PTR)interp;
    MqBufferSetV(buf, "<MqS-%p>", mqctx);
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


