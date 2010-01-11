/**
 *  \file       theLink/pymsgque/MqS_python.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_python.h"

#define MSGQUE msgque

static PyObject *
NS(MqS_new)(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  MqS_Obj *self = (MqS_Obj*)type->tp_alloc(type, 0);
  MqContextInit (&self->context, 0, NULL);
  Py_INCREF(self);
//MqDLogX(&self->context,__func__,0,"self<%p>, refCount<%li>\n", self, ((PyObject*)self)->ob_refcnt);
  MqConfigSetSelf (&self->context, self);
  self->context.setup.Child.fCreate  = MqLinkDefault;
  self->context.setup.Parent.fCreate = MqLinkDefault;
  self->context.setup.fProcessExit = NS(ProcessExit);
  MqConfigSetIgnoreThread (&self->context, MQ_YES);
  return (PyObject *)self;
}

static void
NS(MqS_dealloc)(MqS_Obj* self)
{
  MqContextFree (&self->context);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

//////////////////////////////////////////////////////////////////////////////////
///
///				 Method definition
///
//////////////////////////////////////////////////////////////////////////////////

// from msgque_python.c

PyObject* NS(ProcessEvent)	    ( PyObject*, PyObject*, PyObject* );
PyObject* NS(Delete)		    ( PyObject*		   );
PyObject* NS(Exit)		    ( PyObject*            );

#define ProcessEvent_DOC	    "[[timeout(sec),wait(bool)[,forever(bool)]]]\nStart the eventloop and wait for incomming messages."
#define Delete_DOC		    "[noARG] Delete the LibMsgque object, but keep the PyMqS object alive."
#define Exit_DOC		    "exit the application or thread"

// from link_python.c

PyObject* NS(LinkCreate)	    ( PyObject*, PyObject* );
PyObject* NS(LinkCreateChild)	    ( PyObject*, PyObject* );
PyObject* NS(LinkDelete)	    ( PyObject*            );
PyObject* NS(LinkIsParent)	    ( PyObject* );
PyObject* NS(LinkIsConnected)	    ( PyObject* );
PyObject* NS(LinkGetParent)	    ( PyObject* );
PyObject* NS(LinkGetCtxId)	    ( PyObject* );

#define LinkCreate_DOC		    "[config, args] create a new parent-object-link"
#define LinkCreateChild_DOC	    "[parent, args] create a new child-object-link"
#define LinkDelete_DOC		    "delete the object-link"
#define LinkIsParent_DOC	    "[noARG] boolean, 'True' if the object belongs to a 'parent'."
#define LinkIsConnected_DOC	    "[noARG] boolean, 'True' if the object-link is up and running'."
#define LinkGetParent_DOC	    "[noARG] return the PyMqS 'parent' object of the 'current' object."
#define LinkGetCtxId_DOC	    "[noARG] return the PyMqS 'identifer' (int) of the object."

// from send_python.c

PyObject* NS(SendSTART)		    ( PyObject*		    );
PyObject* NS(SendEND)		    ( PyObject*, PyObject*  );
PyObject* NS(SendEND_AND_WAIT)	    ( PyObject*, PyObject*  );
PyObject* NS(SendEND_AND_CALLBACK)  ( PyObject*, PyObject*  );
PyObject* NS(SendRETURN)	    ( PyObject*		    );
PyObject* NS(SendERROR)		    ( PyObject*		    );
PyObject* NS(SendL_START)	    ( PyObject*		    );
PyObject* NS(SendL_END)		    ( PyObject*		    );
PyObject* NS(SendY)		    ( PyObject*, PyObject*  );
PyObject* NS(SendO)		    ( PyObject*, PyObject*  );
PyObject* NS(SendS)		    ( PyObject*, PyObject*  );
PyObject* NS(SendI)		    ( PyObject*, PyObject*  );
PyObject* NS(SendF)		    ( PyObject*, PyObject*  );
PyObject* NS(SendW)		    ( PyObject*, PyObject*  );
PyObject* NS(SendD)		    ( PyObject*, PyObject*  );
PyObject* NS(SendC)		    ( PyObject*, PyObject*  );
PyObject* NS(SendB)		    ( PyObject*, PyObject*  );
PyObject* NS(SendN)		    ( PyObject*, PyObject*  );
PyObject* NS(SendBDY)		    ( PyObject*, PyObject*  );
PyObject* NS(SendU)		    ( PyObject*, PyObject*  );

#define SendSTART_DOC		  "[noARG] Start to build a SEND package."
#define	SendEND_DOC		  "[token] Send package, no answer expected."
#define	SendEND_AND_CALLBACK_DOC  "[token, callback] Send package and expect answer using a callback."
#define	SendEND_AND_WAIT_DOC	  "[token, timeout] Send package and wait for an answer."
#define	SendRETURN_DOC		  "[noARG] Answer a service call."
#define	SendERROR_DOC		  "[noARG] send an error asyncrone to the link target."
#define SendY_DOC		  "[value] Send a int as (1 byte signed char)."
#define SendO_DOC		  "[value] Send a boolean"
#define SendS_DOC		  "[value] Send a int as (2 byte short integer)."
#define SendI_DOC		  "[value] Send a int as (4 byte integer)."
#define SendF_DOC		  "[value] Send a float as (4 byte float)."
#define SendW_DOC		  "[value] Send a int as (8 byte wide integer)."
#define SendD_DOC		  "[value] Send a double as (8 byte double)."
#define SendC_DOC		  "[value] Send a utf8 string as (char array with \0 at the end)."
#define SendB_DOC		  "[value] Send a byte array as (unsigned char array, \0 is allowed)."
#define SendN_DOC		  "[value] Send a libmsgque package item"
#define SendBDY_DOC		  "[value] Send a libmsgque package body"
#define SendU_DOC		  "[value] Send a PyMqS buffer object."
#define	SendL_START_DOC		  "[noARG] Start to Send an embedded List-Item."
#define	SendL_END_DOC		  "[noARG] End to Send an embedded List-Item."

// from service_python.c

PyObject* NS(ServiceCreate)	      ( PyObject*, PyObject*  );
PyObject* NS(ServiceProxy)	      ( PyObject*, PyObject*  );
PyObject* NS(ServiceDelete)	      ( PyObject*, PyObject*  );
PyObject* NS(ServiceGetToken)	      ( PyObject* );
PyObject* NS(ServiceIsTransaction)    ( PyObject* );
PyObject* NS(ServiceGetFilter)	      ( PyObject*, PyObject* );

#define ServiceCreate_DOC	      "[token, callable] Create a new service"
#define ServiceProxy_DOC	      "[token, ?id?] Create a proxy service"
#define ServiceDelete_DOC	      "[token] Delete a service"
#define ServiceIsTransaction_DOC      "[noARG] boolean, 'True' if the service is part of a transaction."
#define ServiceGetToken_DOC	      "[noARG] return the PyMqS 'token' (str) of the current transaction."
#define ServiceGetFilter_DOC	      "[noARG] return the PyMqS 'filter' object of the 'current' object."

// from slave_python.c

PyObject* NS(SlaveWorker)	      ( PyObject*, PyObject*  );
PyObject* NS(SlaveCreate)	      ( PyObject*, PyObject*  );
PyObject* NS(SlaveDelete)	      ( PyObject*, PyObject*  );
PyObject* NS(SlaveGet)		      ( PyObject*, PyObject*  );
PyObject* NS(SlaveIs)		      ( PyObject* );
PyObject* NS(SlaveGetMaster)	      ( PyObject* );

#define SlaveWorker_DOC		      "[id, worker arguments] Create a new worker using arguments"
#define SlaveCreate_DOC		      "[id, slave-context] Create a new master/slave link"
#define SlaveDelete_DOC		      "[id] Delete slave of id"
#define SlaveGet_DOC		      "[id] Return slave-context of id"
#define SlaveIs_DOC		      "[noARG] boolean, 'True' if the object belongs to a 'slave'."
#define SlaveGetMaster_DOC	      "[noARG] return the PyMqS 'master' object of the 'current' object."

// from service_python.c

PyObject* NS(ReadY)		  ( PyObject* );
PyObject* NS(ReadO)		  ( PyObject* );
PyObject* NS(ReadS)		  ( PyObject* );
PyObject* NS(ReadI)		  ( PyObject* );
PyObject* NS(ReadF)		  ( PyObject* );
PyObject* NS(ReadW)		  ( PyObject* );
PyObject* NS(ReadD)		  ( PyObject* );
PyObject* NS(ReadC)		  ( PyObject* );
PyObject* NS(ReadB)		  ( PyObject* );
PyObject* NS(ReadN)		  ( PyObject* );
PyObject* NS(ReadBDY)		  ( PyObject* );
PyObject* NS(ReadU)		  ( PyObject* );
PyObject* NS(ReadL_START)	  ( PyObject*, PyObject* );
PyObject* NS(ReadL_END)		  ( PyObject* );
PyObject* NS(ReadGetNumItems)	  ( PyObject* );
PyObject* NS(ReadItemExists)	  ( PyObject* );
PyObject* NS(ReadUndo)		  ( PyObject* );
PyObject* NS(ReadProxy)		  ( PyObject*, PyObject* );

#define ReadY_DOC		  "[noARG] Read a int (1 byte signed char) from the current PyMqS package."
#define ReadO_DOC		  "[noARG] Read a boolean (1 byte boolean) from the current PyMqS package."
#define ReadS_DOC		  "[noARG] Read a int (2 byte signed short) from the current PyMqS package."
#define ReadI_DOC		  "[noARG] Read a int (4 byte singed integer) from the current PyMqS package."
#define ReadF_DOC		  "[noARG] Read a float (4 byte float) from the current PyMqS package."
#define ReadW_DOC		  "[noARG] Read a int (8 byte singed long long integer) from the current PyMqS package."
#define ReadD_DOC		  "[noARG] Read a float (8 byte doubel) from the current PyMqS package."
#define ReadC_DOC		  "[noARG] Read a a UTF8 string from the current PyMqS package."
#define ReadB_DOC		  "[noARG] Read a byte array from the current PyMqS package."
#define ReadN_DOC		  "[noARG] Read a PyMqS package item."
#define ReadBDY_DOC		  "[noARG] Read the PyMqS package body."
#define ReadU_DOC		  "[noARG] Read a PyMqS buffer from the current PyMqS package."
#define ReadL_START_DOC		  "[noARG] Start to read an embedded list item from the current PyMqS package."
#define ReadL_END_DOC		  "[noARG] End to read an embedded list item from the current PyMqS package."
#define ReadGetNumItems_DOC	  "[noARG] Get the number of items left in the PyMqS data package."
#define ReadItemExists_DOC	  "[noARG] Is an additional item available in the PyMqS data package."
#define ReadUndo_DOC		  "[noARG] Undo the last Read? operation."
#define ReadProxy_DOC		  "[send-context] forward a message to the 'send-context'."

// from config_python.c


PyObject* NS(ConfigSetBuffersize)     ( PyObject*, PyObject* );
PyObject* NS(ConfigSetDebug)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetTimeout)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetName)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetSrvName)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetIdent)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigCheckIdent)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetIoUds)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetIoTcp)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetIoPipe)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetStartAs)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetDaemon)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetIsString)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetIgnoreExit)     ( PyObject*, PyObject* );
PyObject* NS(ConfigSetIsSilent)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetIsServer)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetServerSetup)    ( PyObject*, PyObject* );
PyObject* NS(ConfigSetServerCleanup)  ( PyObject*, PyObject* );
PyObject* NS(ConfigSetBgError)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetEvent)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigSetFactory)	      ( PyObject*, PyObject* );
PyObject* NS(ConfigGetDebug)	      ( PyObject* );
PyObject* NS(ConfigGetTimeout)	      ( PyObject* );
PyObject* NS(ConfigGetBuffersize)     ( PyObject* );
PyObject* NS(ConfigGetIsString)	      ( PyObject* );
PyObject* NS(ConfigGetIsSilent)	      ( PyObject* );
PyObject* NS(ConfigGetIsServer)	      ( PyObject* );
PyObject* NS(ConfigGetName)	      ( PyObject* );
PyObject* NS(ConfigGetSrvName)	      ( PyObject* );
PyObject* NS(ConfigGetIdent)	      ( PyObject* );
PyObject* NS(ConfigGetIoUdsFile)      ( PyObject* );
PyObject* NS(ConfigGetIoTcpHost)      ( PyObject* );
PyObject* NS(ConfigGetIoTcpPort)      ( PyObject* );
PyObject* NS(ConfigGetIoTcpMyHost)    ( PyObject* );
PyObject* NS(ConfigGetIoTcpMyPort)    ( PyObject* );
PyObject* NS(ConfigGetIoPipeSocket)   ( PyObject* );
PyObject* NS(ConfigGetStartAs)	      ( PyObject* );
PyObject* NS(ConfigGetDaemon)	      ( PyObject* );


#define ConfigSetBuffersize_DOC	    "[bytes]"
#define ConfigSetDebug_DOC	    "[integer (0-9)]"
#define ConfigSetTimeout_DOC	    "[seconds]"
#define ConfigSetName_DOC	    "[string]"
#define ConfigSetSrvName_DOC	    "[string]"
#define ConfigSetIdent_DOC	    "[string]"
#define ConfigCheckIdent_DOC	    "[string]"
#define ConfigSetIoUds_DOC	    "[file-name]"
#define ConfigSetIoTcp_DOC	    "[hots, port, myhost, myport]"
#define ConfigSetIoPipe_DOC	    "[socket]"
#define ConfigSetStartAs_DOC	    "[integer (0-3)]"
#define ConfigSetDaemon_DOC	    "[pidfile]"
#define ConfigSetIsString_DOC	    "[boolean]"
#define ConfigSetIgnoreExit_DOC	    "[boolean]"
#define ConfigSetIsSilent_DOC	    "[boolean]"
#define ConfigSetIsServer_DOC	    "[boolean]"
#define ConfigSetServerSetup_DOC    "[callable method]"
#define ConfigSetServerCleanup_DOC  "[callable method]"
#define ConfigSetBgError_DOC	    "[callable method]"
#define ConfigSetEvent_DOC	    "[callable method]"
#define ConfigSetFactory_DOC	    "[callable method]"
#define ConfigGetIsString_DOC	    "[noARG] boolean, 'True' if the object is using the 'string' configuration."
#define ConfigGetIsSilent_DOC	    "[noARG] boolean, 'True' if the object is using the 'silent' configuration."
#define ConfigGetIsServer_DOC	    "[noARG] boolean, 'True' if the object belongs to a 'server'."
#define ConfigGetDebug_DOC	    "[noARG] Return the debug level (int from 0 up to 9)."
#define ConfigGetTimeout_DOC	    "[noARG] Return the user defined timeout is sec."
#define ConfigGetBuffersize_DOC	    "[noARG] Return the buffersize from the underlying socket connection."
#define ConfigGetName_DOC	    "[noARG] return the PyMqS 'name' (str) of the object."
#define ConfigGetSrvName_DOC	    "[noARG] return the PyMqS 'srvname' (str) of the object."
#define ConfigGetIdent_DOC	    "[noARG] return the PyMqS 'ident' (str) of the object."
#define ConfigGetIoUdsFile_DOC	    "[noARG] return the PyMqS 'uds-file-name' (str) of the object."
#define ConfigGetIoTcpHost_DOC	    "[noARG] return the PyMqS 'tcp-host-name' (str) of the object."
#define ConfigGetIoTcpPort_DOC	    "[noARG] return the PyMqS 'tcp-port-name' (str) of the object."
#define ConfigGetIoTcpMyHost_DOC    "[noARG] return the PyMqS 'tcp-myhost-name' (str) of the object."
#define ConfigGetIoTcpMyPort_DOC    "[noARG] return the PyMqS 'tcp-myport-name' (str) of the object."
#define ConfigGetIoPipeSocket_DOC   "[noARG] return the PyMqS 'pipe-socket-file-descriptor' (int) of the object."
#define ConfigGetStartAs_DOC	    "[noARG] return the PyMqS 'start-kind-value' (int) of the object."

// from error_python.c

PyObject* NS(ErrorC)		    ( PyObject*, PyObject*  );
PyObject* NS(ErrorSet)		    ( PyObject*		    );
PyObject* NS(ErrorSetCONTINUE)	    ( PyObject*		    );
PyObject* NS(ErrorRaise)	    ( PyObject*		    );
PyObject* NS(ErrorReset)	    ( PyObject*		    );
PyObject* NS(ErrorPrint)	    ( PyObject*		    );
PyObject* NS(ErrorGetNum)	    ( PyObject*		    );
PyObject* NS(ErrorGetText)	    ( PyObject*		    );

#define ErrorC_DOC		    "[function-name, error-number, error-message] (testing only) create a PyMqS error"
#define ErrorSet_DOC		    "set a 'libmsgque' error from a python 'MqSException' error"
#define ErrorSetCONTINUE_DOC	    "signal that the event-function has finished the work"
#define ErrorRaise_DOC		    "convert and raise an error from 'pymsgque' into 'python'"
#define ErrorReset_DOC		    "clear a 'pymsgque' error"
#define ErrorPrint_DOC		    "print a 'pymsgque' error to stderr"
#define ErrorGetNum_DOC		    "return the 'pymsgque' error number"
#define ErrorGetText_DOC	    "return the 'pymsgque' error text"


// Fill the Struct-Array

#define ARG(N,M) { #N , (PyCFunction) NS(N), M, N ## _DOC}
static PyMethodDef NS(MqS_Methods)[] = {

    ARG(ProcessEvent,		METH_VARARGS | METH_KEYWORDS),
    ARG(Delete,			METH_NOARGS),
    ARG(Exit,			METH_NOARGS),

    ARG(LinkCreate,		METH_VARARGS),
    ARG(LinkCreateChild,	METH_VARARGS),
    ARG(LinkDelete,		METH_NOARGS),
    ARG(LinkIsParent,		METH_NOARGS),
    ARG(LinkIsConnected,	METH_NOARGS),
    ARG(LinkGetParent,		METH_NOARGS),
    ARG(LinkGetCtxId,		METH_NOARGS),

    ARG(SendSTART,		METH_NOARGS),
    ARG(SendEND,		METH_VARARGS),
    ARG(SendEND_AND_WAIT,	METH_VARARGS),
    ARG(SendEND_AND_CALLBACK,	METH_VARARGS),
    ARG(SendRETURN,		METH_NOARGS),
    ARG(SendERROR,		METH_NOARGS),
    ARG(SendL_START,		METH_NOARGS),
    ARG(SendL_END,		METH_NOARGS),
    ARG(SendY,			METH_VARARGS),
    ARG(SendO,			METH_VARARGS),
    ARG(SendS,			METH_VARARGS),
    ARG(SendI,			METH_VARARGS),
    ARG(SendF,			METH_VARARGS),
    ARG(SendW,			METH_VARARGS),
    ARG(SendD,			METH_VARARGS),
    ARG(SendC,			METH_VARARGS),
    ARG(SendB,			METH_VARARGS),
    ARG(SendN,			METH_VARARGS),
    ARG(SendBDY,		METH_VARARGS),
    ARG(SendU,			METH_VARARGS),

    ARG(ReadL_START,		METH_VARARGS),
    ARG(ReadL_END,		METH_NOARGS),
    ARG(ReadY,			METH_NOARGS),
    ARG(ReadO,			METH_NOARGS),
    ARG(ReadS,			METH_NOARGS),
    ARG(ReadI,			METH_NOARGS),
    ARG(ReadF,			METH_NOARGS),
    ARG(ReadW,			METH_NOARGS),
    ARG(ReadD,			METH_NOARGS),
    ARG(ReadC,			METH_NOARGS),
    ARG(ReadB,			METH_NOARGS),
    ARG(ReadN,			METH_NOARGS),
    ARG(ReadBDY,		METH_NOARGS),
    ARG(ReadU,			METH_NOARGS),
    ARG(ReadGetNumItems,	METH_NOARGS),
    ARG(ReadItemExists,		METH_NOARGS),
    ARG(ReadUndo,		METH_NOARGS),
    ARG(ReadProxy,		METH_VARARGS),

    ARG(ServiceCreate,		METH_VARARGS),
    ARG(ServiceProxy,		METH_VARARGS),
    ARG(ServiceDelete,		METH_VARARGS),
    ARG(ServiceGetToken,	METH_NOARGS),
    ARG(ServiceGetFilter,	METH_VARARGS),
    ARG(ServiceIsTransaction,	METH_NOARGS),

    ARG(SlaveWorker,		METH_VARARGS),
    ARG(SlaveCreate,		METH_VARARGS),
    ARG(SlaveDelete,		METH_VARARGS),
    ARG(SlaveGet,		METH_VARARGS),

    ARG(ConfigSetBuffersize,	METH_O),
    ARG(ConfigSetDebug,		METH_O),
    ARG(ConfigSetTimeout,	METH_O),
    ARG(ConfigSetName,		METH_O),
    ARG(ConfigSetSrvName,	METH_O),
    ARG(ConfigSetIdent,		METH_O),
    ARG(ConfigCheckIdent,	METH_O),
    ARG(ConfigSetIoUds,		METH_O),
    ARG(ConfigSetIoTcp,		METH_VARARGS),
    ARG(ConfigSetIoPipe,	METH_O),
    ARG(ConfigSetStartAs,	METH_O),
    ARG(ConfigSetDaemon,	METH_O),
    ARG(ConfigSetIsSilent,	METH_O),
    ARG(ConfigSetIsServer,	METH_O),
    ARG(ConfigSetIsString,	METH_O),
    ARG(ConfigSetIgnoreExit,	METH_O),
    ARG(ConfigSetServerSetup,	METH_O),
    ARG(ConfigSetServerCleanup, METH_O),
    ARG(ConfigSetBgError,	METH_O),
    ARG(ConfigSetEvent,		METH_O),
    ARG(ConfigSetFactory,	METH_O),
    ARG(ConfigGetIsString,	METH_NOARGS),
    ARG(ConfigGetIsSilent,	METH_NOARGS),
    ARG(ConfigGetIsServer,	METH_NOARGS),
    ARG(SlaveIs,	METH_NOARGS),
    ARG(ConfigGetDebug,		METH_NOARGS),
    ARG(ConfigGetTimeout,	METH_NOARGS),
    ARG(ConfigGetBuffersize,	METH_NOARGS),
    ARG(ConfigGetName,		METH_NOARGS),
    ARG(ConfigGetSrvName,	METH_NOARGS),
    ARG(ConfigGetIdent,		METH_NOARGS),
    ARG(ConfigGetIoUdsFile,	METH_NOARGS),
    ARG(ConfigGetIoTcpHost,	METH_NOARGS),
    ARG(ConfigGetIoTcpPort,	METH_NOARGS),
    ARG(ConfigGetIoTcpMyHost,	METH_NOARGS),
    ARG(ConfigGetIoTcpMyPort,	METH_NOARGS),
    ARG(ConfigGetIoPipeSocket,	METH_NOARGS),
    ARG(ConfigGetStartAs,	METH_NOARGS),
    ARG(SlaveGetMaster,	METH_NOARGS),

    ARG(ErrorC,			METH_VARARGS),
    ARG(ErrorSet,		METH_NOARGS),
    ARG(ErrorSetCONTINUE,	METH_NOARGS),
    ARG(ErrorRaise,		METH_NOARGS),
    ARG(ErrorReset,		METH_NOARGS),
    ARG(ErrorPrint,		METH_NOARGS),
    ARG(ErrorGetNum,		METH_NOARGS),
    ARG(ErrorGetText,		METH_NOARGS),
    
    {NULL, NULL, 0, NULL}        /* Sentinel */
};
#undef ARG


#define MqS_DOC	"The msgque project is an infrastructure to link software together to\n\
act like a single software. To link mean distributing work from one\n\
software to an other software an wait or not wait for an answer. The\n\
linking is done using unix or inet domain sockets and is based on\n\
packages send from one software to an other software and back. The\n\
tclmsgque project is used to handle all the different aspects for setup\n\
and maintain the link and is responsible for:\n\
\n\
o starting and stopping the server application\n\
o starting and stopping the communication interface\n\
o sending and receiving package data\n\
o reading and writing data from or into a package\n\
o setup and maintain the event-handling for an asynchronous transfer\n\
o propagate the error messages from the server to the client"


PyTypeObject NS(MqS) = {
  {PyObject_HEAD_INIT(NULL)},
  "pymsgque.MqS",		  /* tp_name */
  sizeof(MqS_Obj),		  /* tp_basicsize */
  0,				  /* tp_itemsize */
  (destructor)NS(MqS_dealloc),	  /* tp_dealloc */
  0,				  /* tp_print */
  0,				  /* tp_getattr */
  0,				  /* tp_setattr */
  0,				  /* tp_reserved */
  0,				  /* tp_repr */
  0,				  /* tp_as_number */
  0,				  /* tp_as_sequence */
  0,				  /* tp_as_mapping */
  0,				  /* tp_hash  */
  0,				  /* tp_call */
  0,				  /* tp_str */
  0,				  /* tp_getattro */
  0,				  /* tp_setattro */
  0,				  /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | 
  Py_TPFLAGS_BASETYPE,		  /* tp_flags */
  MqS_DOC,			  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  NS(MqS_Methods),		  /* tp_methods */
  0,				  /* tp_members */
  0,				  /* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  0,				  /* tp_init */
  0,				  /* tp_alloc */
  NS(MqS_new),			  /* tp_new */
};









