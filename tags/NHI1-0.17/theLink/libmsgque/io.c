/**
 *  \file       theLink/libmsgque/io.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"
#include "sys_com.h"

BEGIN_C_DECLS

/// \brief undefined socket, initial value for #MqIoS:sockP
static MQ_SOCK sockUndef = -1;

extern struct MqBufferLS * MqInitBuf;

/*****************************************************************************/
/*                                                                           */
/*                                io_init                                    */
/*                                                                           */
/*****************************************************************************/

#define MQ_CONTEXT_S io->context
#define MQ_GENERIC_S io->generic

enum MqErrorE
pIoCreate (
  struct MqS * const context,
  struct MqIoS ** const out
)
{
  register struct MqIoS * const io = *out ? *out : 
    (*out = (struct MqIoS *) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*io)));

  io->context = context;
  // parent and child get both this initial value
  // !!attention if child get an other value then it will wait forever (SysWait)
  // during child cleanup
  io->id.type = MQ_ID_UNUSED;
  io->config = &context->config.io;

  // search for communication type
  if (MQ_IS_PARENT (context)) {

    // init parent and set to "non-connected"
    io->sockP = (MQ_SOCK *)&sockUndef;
    context->link.bits.isConnected = MQ_NO; // just to be sure

    // create event-structure
    MqErrorCheck (pEventCreate (context, &io->event));

    // create communication layer
    switch (io->config->com) {
      case MQ_IO_UDS:
        MqErrorCheck (UdsCreate  (io, &io->iocom.udsSP));
        break;
      case MQ_IO_TCP:
        MqErrorCheck (TcpCreate  (io, &io->iocom.tcpSP));
        break;
      case MQ_IO_PIPE:
        MqErrorCheck (PipeCreate (io, &io->iocom.pipeSP));
        break;
    }
  } else {
    struct MqIoS * parent = context->config.parent->link.io;
    io->sockP = parent->sockP;
    // without "event" a client is not able to do any communication
    io->event = parent->event;
  }

  // reset fdset
  FD_ZERO (&io->fdset);

  // create the server socket
  if (MQ_IS_SERVER_PARENT (context)) {
    switch (io->config->com) {
      case MQ_IO_UDS:
        MqErrorCheck (UdsServer (io->iocom.udsSP));
        break;
      case MQ_IO_TCP:
        MqErrorCheck (TcpServer (io->iocom.tcpSP));
        break;
      case MQ_IO_PIPE:
        MqErrorCheck (PipeServer (io->iocom.pipeSP));
        break;
    }
  }

error:
  return MqErrorStack (context);
}

void
pIoShutdown (
  struct MqIoS * const io
)
{
  if (unlikely(io == NULL)) {
    return;
  } else {
    pEventDel (__func__, io->context);
  }
}

void
pIoCloseSocket (
  MQ_CST caller,
  struct MqIoS * const io
)
{
  if (unlikely(io == NULL)) {
    return;
  } else {
    struct MqS * const context = io->context;
    pEventDel (__func__, context);
    SysCloseSocket (context, caller, MQ_YES, io->sockP);
    // set "initial" parent and all child to "NO"
    pLinkDisConnect (context->link.ctxIdP);
  }
}

void
pIoDelete (
  struct MqIoS ** const ioP
)
{
  if (unlikely(ioP == NULL || *ioP == NULL)) {
    return;               // nothing to do
  } else {
    struct MqIoS const * const io = *ioP;
    struct MqS const * const context = io->context;

    // ...
    if (MQ_IS_PARENT (context)) {

      // wait for started thread because to exit ithe main process image will
      // exit the thread too, and the thread propably has no chance for
      // proper cleanup -> this is only used in FILTER mode
      MqSysWait (MQ_ERROR_IGNORE, &io->id);

      // drop event
      pEventDelete (context);

      // cleanup the io data
      switch (io->config->com) {
	case MQ_IO_UDS:
	  UdsDelete ((struct UdsS **) & io->iocom);
	  break;
	case MQ_IO_TCP:
	  TcpDelete ((struct TcpS **) & io->iocom);
	  break;
	case MQ_IO_PIPE:
	  PipeDelete ((struct PipeS **) & io->iocom);
	  break;
      }
    }

    MqSysFree (*ioP);
    return;
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                io_atom                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
pIoRead (
  struct MqIoS const * const io,
  register struct MqBufferS * const buf,
  MQ_SIZE const numBytes
)
{
  // alloc space
  pBufferNewSize (buf, numBytes);
  // reset the buffer
  buf->cur.B = buf->data;
  // read into the buffer
  MqErrorReturn (SysRecv (io->context, *io->sockP, buf->data, numBytes, &buf->cursize, io->config->timeout));
}

enum MqErrorE
pIoSend (
  struct MqIoS const * const io,
  struct MqBufferS const * const buf
)
{
  MqErrorReturn (SysSend (io->context, *io->sockP, buf->data, buf->cursize, 0, io->config->timeout));
}

enum MqErrorE
pIoConnect (
  struct MqIoS * const io
)
{
  switch (io->config->com) {
    case MQ_IO_UDS:
      MqErrorCheck (UdsConnect (io->iocom.udsSP));
      break;
    case MQ_IO_TCP:
      MqErrorCheck (TcpConnect (io->iocom.tcpSP));
      break;
    case MQ_IO_PIPE:
      MqErrorCheck (PipeConnect (io->iocom.pipeSP));
      break;
  }

error:
  return MqErrorStack (io->context);
}

/*
enum MqErrorE
pIoSelect (
  struct MqIoS * const io,
  enum MqIoSelectE const typ,
  struct timeval * const timeout
)
{
  // set in GenericCreateSocket or in PipeCreate -> SysConnect need this
  register MQ_SOCK const sock = *io->sockP;
  fd_set *fds, *readfds = NULL, *writefds = NULL;

  if (sock < 0)
    return MQ_CONTINUE;	    // nothing found

  fds = &io->fdset;

  // build fds
  FD_SET (sock, fds);

  // compose
  switch (typ) {
    case MQ_SELECT_RECV:
      readfds = fds;
      break;
    case MQ_SELECT_SEND:
      writefds = fds;
      break;
  }

  // do the select
  MqErrorReturn (SysSelect (io->context, (sock+1), readfds, writefds, timeout));
}
*/

enum MqErrorE
pIoSelectAll (
  struct MqIoS * const io,
  enum MqIoSelectE const typ,
  struct timeval * const timeout
)
{
  return pEventCheck (io->context, io->event, typ, timeout);
}

enum MqErrorE
pIoSelectStart (
  struct MqS * const context,
  struct timeval const * const timeout
)
{
  return pEventStart (context, context->link.io->event, timeout);
}

/*****************************************************************************/
/*                                                                           */
/*                                 io_misc                                   */
/*                                                                           */
/*****************************************************************************/

static void
sIoFillArgvC (
  struct MqIoS * const io,	    ///< [in] the current io object
  struct MqBufferS **start1,	    ///< [in] pointer to alfa1
  struct MqBufferS **end1,	    ///< [in] end pointer to alfa1
  struct MqBufferLS * alfa2,	    ///< [in] after first #MQ_ALFA
  const MQ_SOCK sock,		    ///< [in] socket for the \e new parent
  char ** arg,			    ///< [out] the new argv array to fill
  MQ_CST startAs,		    ///< [in] start as specifier
  MQ_CST storage		    ///< [in] storage file-name
) {
  struct MqS const * const context = MQ_CONTEXT_S;
  struct MqBufferS **start, **end;

  // 3. add PIPE arguments
  *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "---duplicate");
  *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "---status-is-spawn");
  *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "--socket");
  *arg = (char*) MqSysMalloc(MQ_ERROR_PANIC,20);
  mq_snprintf((char*)*arg++,20,"%i",sock);

  // 4. add context stuff
  if (io->config->buffersize != buffersize_DEFAULT) {
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "--buffersize");
    *arg = (char*) MqSysMalloc(MQ_ERROR_PANIC,20);
    mq_snprintf((char*)*arg++,20,"%i",io->config->buffersize);
  }
  if (io->config->timeout != timeout_DEFAULT) {
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "--timeout");
    *arg = (char*) MqSysMalloc(MQ_ERROR_PANIC,20);
    mq_snprintf((char*)*arg++,20,MQ_FORMAT_W,io->config->timeout);
  }
  if (storage != NULL && strcmp(storage,storage_DEFAULT)) {
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "--storage");
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, storage);
  }
  if (context->config.isSilent) {
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "--silent");
  }
  if (context->config.debug) {
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "--debug");
    *arg = (char*) MqSysMalloc(MQ_ERROR_PANIC,5);
    mq_snprintf ((char*)*arg++, 5, "%i", context->config.debug);
  }
  if (startAs) {
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, startAs);
  }

  // 5. all alfa1 arguments starting with the first option
  for (; start1 < end1; start1++) {
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, (*start1)->cur.C);
  }

  // 6. copy the left over parts starting with the first MQ_ALFA
  if (alfa2) {
    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, MQ_ALFA_STR);
    start = alfa2->data;
    end = start+alfa2->cursize;
    for (; start < end; start++) {
      *arg++ = MqSysStrDup(MQ_ERROR_PANIC, (*start)->cur.C);
    }
  }
  *arg = NULL;
}

#if defined(MQ_HAS_THREAD) || defined(HAVE_FORK)
static void
sIoFillArgvU (
  struct MqIoS * const io,	    ///< [in] the current io object
  const MQ_SOCK sock,		    ///< [in] socket for the \e new parent
  struct MqBufferLS * argv,	    ///< [out] the new argv array to fill
  MQ_CST startAs,		    ///< [in] start as specifier
  MQ_CST storage		    ///< [in] storage file-name
) {
  struct MqS const * const context = io->context;
  MqBufferLAppendC(argv, "---duplicate");
  MqBufferLAppendC(argv, "--socket");
  MqBufferLAppendI(argv, sock);
  if (io->config->buffersize != buffersize_DEFAULT) {
    MqBufferLAppendC(argv, "--buffersize");
    MqBufferLAppendI(argv, io->config->buffersize);
  }
  if (io->config->timeout != timeout_DEFAULT) {
    MqBufferLAppendC(argv, "--timeout");
    MqBufferLAppendW(argv, io->config->timeout);
  }
  if (storage != NULL && strcmp(storage,storage_DEFAULT)) {
    MqBufferLAppendC(argv, "--storage");
    MqBufferLAppendC(argv, storage);
  }
  if (context->config.isSilent) {
    MqBufferLAppendC(argv, "--silent");
  }
  if (context->config.debug) {
    MqBufferLAppendC(argv, "--debug");
    MqBufferLAppendI(argv, context->config.debug);
  }
  if (startAs) {
    MqBufferLAppendC(argv, startAs);
  }
}
#endif

static void sIoFreeArgvArray (
  char *** argvP		    ///< [in,out] a pointer to the argv array
) {
  char ** arg = *argvP;
  
  if (arg == NULL) return;

  // 1. free the arguments
  for (;*arg != NULL;arg++)
    MqSysFree(*arg);

  // 2. free the array itself
  MqSysFree(*argvP);
}

enum MqErrorE
pIoStartServer (
  struct MqIoS * const io,
  enum IoStartServerE startType,
  MQ_SOCK * sockP,
  struct MqIdS * idP
) {
  struct MqS * const context = io->context;
  MQ_CST storage = context->config.storage;
#if defined(MQ_HAS_THREAD) || defined(HAVE_FORK)
  struct MqFactoryS * factory = NULL;
  struct MqBufferLS * alfa1 = NULL;
  struct MqBufferLS * alfa2 = NULL;
  int start_as_pipe = 0;
#endif
#if defined(MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
    int thread_status = PTHREAD_CREATE_DETACHED;
# else
    int thread_status = 0;
# endif
#endif

  MQ_CST  name = NULL;
  MQ_CST  displayname = NULL;
  MQ_BUF  tmpName = NULL;

  // select the code
rescan:
  switch (startType) {
    case MQ_START_SERVER_AS_PIPE: {
	MQ_BOL del_first_on_spawn = MQ_NO;
	start_as_pipe = 1;
	// storage is local only generic server startup apps should get storage localtion
	// from starting-app
	storage = NULL;
	// alfa is owned by this proc
	alfa1 = MqBufferLDup(context->link.alfa);
//printLC("MQ_START_SERVER_AS_PIPE:")
//printULS(alfa1)
	// case 2: this is used if the function is called from MqLinkCreate. The
	// context->alfa argument has the startup arguments of the MQ_IO_PIPE server.
	// MQ_IO_TCP and MQ_IO_UDS is ignored and it is an error if context->alfa is
	// available too.
	if (io->config->com != MQ_IO_PIPE) {
	  // for non "PIPE" io the "alfa" have to be empty
	  if (alfa1 != NULL) {
	    MqBufferLDelete(&alfa1);
	    return MqErrorDbV2 (context, MQ_ERROR_CAN_NOT_START_NON_PIPE_SERVER, pIoLogCom(io->config->com));
	  }
	  // for non "PIPE" io we have nothing to do
	  return MQ_OK;
	}
	if (alfa1 == NULL) {
	  return MqErrorDb2 (context, MQ_ERROR_CAN_NOT_START_CLIENT);
	}
	// setup alfa1/2
	pBufferLSplitAlfa(&alfa1, &alfa2);
	// get the socket from the pipe
	sockP = PipeGetServerSocket(io->iocom.pipeSP);
	// get the id storage
	idP = &io->id;
	// this is the first entry in alfa1
	name = displayname = alfa1->data[0]->cur.C;

//printLC(name)

	// check if we use the "WORKER" keyword
	if (context->link.bits.isWORKER) {
	  // replace "WORKER" with "MqInitBuf" data
	  if (MqInitBuf && MqInitBuf->cursize >= 1) {
	    name = MqInitBuf->data[0]->cur.C;
	    del_first_on_spawn = MQ_YES;
	  }
	  // add startup entry function
	  factory = context->setup.factory;
	} else if (
	  // check for factory using "name"
	  (factory = MqFactoryGet(name)) != NULL
	) {
	  // -> found

	  // well we need the "name-of-the-executable" binary name
	  // > atool split ... @ cut ... @ join ...
	  // name is "cut" and next line will replace the name with "atool"
	  if (MqInitBuf && MqInitBuf->cursize >= 1) {
	    name = MqInitBuf->data[0]->cur.C;
	    del_first_on_spawn = MQ_YES;
	  }
	}

//printLC(name)
//printLP(factory)

	if (factory) {
#if defined(HAVE_FORK)
	  // continue with "fork"
	  if (context->config.ignoreFork == MQ_NO && (
		context->config.startAs == MQ_START_DEFAULT || 
		context->config.startAs == MQ_START_FORK)) {
//printLC("fork")
	    startType = MQ_START_SERVER_AS_FORK;
	    MqDLogC(context,8,"PIPE: MQ_START_SERVER_AS_FORK\n");
	    goto rescan;
	  }
#endif
#if defined(MQ_HAS_THREAD)
	  // continue with "thread"
	  if (context->config.ignoreThread == MQ_NO && (
		context->config.startAs == MQ_START_DEFAULT || 
		context->config.startAs == MQ_START_THREAD)) {
//printLC("thread")
	    startType = MQ_START_SERVER_AS_THREAD;
	    MqDLogC(context,8,"PIPE: MQ_START_SERVER_AS_THREAD\n");
#if defined(HAVE_PTHREAD)
	    thread_status = PTHREAD_CREATE_JOINABLE;
#endif
	    goto rescan;
	  }
#endif
	}
	// continue with "spawn" (works allways)
	if (context->config.ignoreSpawn == MQ_YES) {
	  MqErrorDbV2(context, MQ_ERROR_CAN_NOT_START_NON_PIPE_SERVER,StringOrUnknown(name));
	}
//printLC("spawn")
	startType = MQ_START_SERVER_AS_SPAWN;
	MqDLogC(context,8,"PIPE: MQ_START_SERVER_AS_SPAWN\n");
	// only SPAWN need all arguments from MqInitBuf

	//if (context->setup.factory && context->setup.factory->called && factory != NULL) {
	if (factory != NULL) {
	  // delete "ITEM" itself on position "0" -> if not called
	  // can not delete the buffer because "alfa1->data[0]" is used in "name" and "displayname".
	  // We are using the "called" flag from the factory-in-use and !not! from
	  // the factory-who-will-be-used. This trick works because the factory-in-use is always
	  // a spawned-factory too.
	  if (del_first_on_spawn && context->setup.factory && !context->setup.factory->called) {
	    MqErrorCheck (MqBufferLGetU (context, alfa1, 0, &tmpName));
	  }
//printULS(MqInitBuf)
	  MqBufferLAppendL(alfa1, MqInitBuf, 0);
	}
	goto rescan;
      }
      break;
#if defined(MQ_HAS_THREAD)
    case MQ_START_SERVER_AS_THREAD: {
	struct MqBufferLS * options;
//printLC("MQ_START_SERVER_AS_THREAD:")
	if (alfa2 == NULL && start_as_pipe != 1)
	  alfa2 = MqBufferLDup (context->link.alfa);

	// copy the configuration from the "PARENT" server
	if (!start_as_pipe) {
	  factory = context->setup.factory;
	}

	// MqMainToolName set in "sMqCheckArg" or at application main startup
	if (name == NULL) {
	  if (MqInitBuf == NULL) {
	    MqErrorDb2(context, MQ_ERROR_NO_INIT);
	    goto cleanup;
	  } else {
	    name = MqInitBuf->data[0]->cur.C;
	  }
	}

	// empty alfa1 (GenericServer) starup -> init with MqInitBuf data from above
	if (alfa1 == NULL) {
	  alfa1 = MqBufferLCreate(20);
	  MqBufferLAppendC(alfa1, name);
	}

	// extract all arguments after and including the first "-..." option
	options = pBufferLExtractOptions(alfa1);

	// if started from "GenericServer" get the "name" from the "inital-server"
	MqBufferLAppendC(alfa1, "--name");
	if (displayname) {
	  MqBufferLAppendC(alfa1, displayname);
	} else {
	  MqBufferLAppendC(alfa1, context->config.name);
	}

	// fill arg with system-arguments
	sIoFillArgvU(io,*sockP,alfa1,"--thread",storage);

	// move the options to the end of alfa1, delete options
	MqBufferLMove(alfa1, &options);

	// special in "thread" mode, the server socket now belongs to the "new-thread"
	// and not the "current-thread"
	*sockP = sockUndef;

//printLC(name)
//printULS(alfa1);
//printULS(alfa2);

	// start the server
	MqErrorCheck (MqSysServerThread (context, factory, &alfa1, &alfa2, name, thread_status, idP));
      }
      break;
#endif /* MQ_HAS_THREAD */
#if defined(HAVE_FORK)
    case MQ_START_SERVER_AS_FORK: {
	struct MqBufferLS * options;
//printLC("MQ_START_SERVER_AS_FORK:")
	if (alfa2 == NULL && start_as_pipe != 1)
	  alfa2 = MqBufferLDup (context->link.alfa);
	// copy the configuration from the "PARENT" server
	if (!start_as_pipe) {
	  factory = context->setup.factory;
	}

	// MqMainToolName set in "sMqCheckArg" or at application main startup
	if (name == NULL) {
	  if (MqInitBuf == NULL) {
	    MqErrorDb2(context, MQ_ERROR_NO_INIT);
	    goto cleanup;
	  } else {
	    name = MqInitBuf->data[0]->cur.C;
	  }
	}

	// empty alfa1 (GenericServer) starup -> init with MqInitBuf data
	if (alfa1 == NULL) {
	  alfa1 = MqBufferLCreate(20);
	  MqBufferLAppendC(alfa1, name);
	}

	// extract all arguments after and including the first "-..." option
	options = pBufferLExtractOptions(alfa1);

	// if started from "GenericServer" get the "name" from the "inital-server"
	MqBufferLAppendC(alfa1, "--name");
	if (displayname) {
	  MqBufferLAppendC(alfa1, displayname);
	} else {
	  MqBufferLAppendC(alfa1, context->config.name);
	}

	// fill arg with system-arguments
	sIoFillArgvU(io,*sockP,alfa1,"--fork",storage);

	// move the options to the end of alfa1, delete options
	MqBufferLMove(alfa1, &options);

//printULS(alfa1)

	// start the server
	MqErrorCheck (MqSysServerFork (context, factory, &alfa1, &alfa2, name, idP));
	// cleanup socket of the child
	MqErrorCheck(SysCloseSocket (context, __func__, MQ_NO, sockP));
      }
      break;
#endif   /* HAVE_FORK */
    case MQ_START_SERVER_AS_SPAWN: {
//printLC("MQ_START_SERVER_AS_SPAWN:")
	struct MqBufferS **start = NULL , **end = NULL;
	char **argV, **arg;

	if (alfa2 == NULL && start_as_pipe != 1)
	  alfa2 = MqBufferLDup (context->link.alfa);
	// add 20 item's as additional space
	argV = arg = (char **) MqSysMalloc (context, sizeof(char*) * (
	    (alfa1 != NULL ? alfa1->cursize : 0) + (alfa2 ? alfa2->cursize : 0) + 20 
	));

	// init main
	if (name == NULL) {
	  if (MqInitBuf == NULL) {
	    MqErrorDb2(context, MQ_ERROR_NO_INIT);
	    goto cleanup;
	  } else {
	    name = MqInitBuf->data[0]->cur.C;
	  }
	}

	// empty alfa1 (GenericServer) starup -> init with MqInitBuf data
	if (alfa1 == NULL) {
          int i;
	  for (i=0; i<MqInitBuf->cursize; i++) {
	    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, MqInitBuf->data[i]->cur.C);
	  }
	} else {
	  // copy everything befor the first MQ_ALFA or option
	  start = alfa1->data;
	  end = start+alfa1->cursize;
	  for (; start < end && !pMqCheckOpt(*start); start++) {
	    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, (*start)->cur.C);
	  }
	}

	// if started from "GenericServer" get the "name" from the "inital-server"
	if (start_as_pipe == 0) {
	  if (context->setup.factory && context->setup.factory->called) {
	    *arg++ = MqSysStrDup(MQ_ERROR_PANIC, context->setup.factory->ident);
	  }
	  *arg++ = MqSysStrDup(MQ_ERROR_PANIC, "--name");
	  *arg++ = MqSysStrDup(MQ_ERROR_PANIC, context->config.name);
	}

	// fill arg with system-arguments
	sIoFillArgvC(io,start,end,alfa2,*sockP,arg,"--spawn",storage);

	// start the server

/*
{
  int i;
  char ** xarg = argV;
  printLC(name)
  for (i=0; *xarg != NULL; xarg++, i++) {
    MqDLogV (context, 0, "alfa1[%2i]=%s\n",i, *xarg);
  }
}
*/

	MqErrorCheck (MqSysServerSpawn (context, argV, name, idP));
	// cleanup array's
	sIoFreeArgvArray(&argV);
	// cleanup socket of the child
	MqErrorCheck(SysCloseSocket (context, __func__, MQ_NO, sockP));
      }
      break;
#if defined(HAVE_FORK)
    case MQ_START_SERVER_AS_INLINE_FORK: {
//printLC("MQ_START_SERVER_AS_INLINE_FORK:")
	if (name == NULL) {
	  if (MqInitBuf == NULL) {
	    MqErrorDb2(context, MQ_ERROR_NO_INIT);
	    goto cleanup;
	  } else {
	    name = MqInitBuf->data[0]->cur.C;
	  }
	}
	// case: the function GenericServer is listen on a "tcl" or "uds" connection
	// and start for every incomming connection a new process with "fork". !no!
	// initialization have to be done because we reuse the current state.
	MqErrorCheck (SysFork (context, idP));
	// if pid != 0 -> this is the parent just continue with default "client" code
	if ((*idP).val == 0UL) {
	  // pid = 0 -> this is the fork child
	  context->statusIs = (enum MqStatusIsE) (context->statusIs | MQ_STATUS_IS_FORK);
	  // we don't need to cleanup argv1/2 because the INLINE_FORK is done
	  // with an empty "alfa"
	  MqErrorReset(context);
	  goto cleanup;
	}
	// cleanup socket of the child
	MqErrorCheck(SysCloseSocket (context, __func__, MQ_NO, sockP));
      }
      break;
#endif   /* HAVE_FORK */
  }

  // ok 2: inform the user
  if (unlikely(context != NULL && context->config.debug >= 4)) {
    MQ_STRB buf[50];
    // !attention "name" can be "alfa1[0]"
    MqLogV(context,__func__,4,"finish start server '%s' as '%s' with ID '%s'\n",
		name, pIoLogStartType(startType), pIoLogId(buf,50,*idP));
  }

  // cleanup alfa1/2
error:
  MqErrorStack (context);

cleanup:
  MqBufferDelete  (&tmpName);
  MqBufferLDelete (&alfa1);
  MqBufferLDelete (&alfa2);

  return MqErrorGetCodeI (context);
}

MQ_CST
pIoLogStartType (
  enum IoStartServerE startType
) {
  switch (startType) {
    case MQ_START_SERVER_AS_PIPE:	    return "pipe";
#if defined(HAVE_FORK)
    case MQ_START_SERVER_AS_FORK:	    return "fork";
    case MQ_START_SERVER_AS_INLINE_FORK:    return "fork";
#endif
#if defined(MQ_HAS_THREAD)
    case MQ_START_SERVER_AS_THREAD:	    return "thread";
#endif
    case MQ_START_SERVER_AS_SPAWN:	    return "spawn";
  }
  return "";
}

/// \brief helper to covert the #MqIdS into a human readable string
/// \retval the input-buffer (\e buf) filled with the string-representation of the \e id
MQ_STR
pIoLogId (
  MQ_STR buf,		    ///< [in,out] input-buffer used as result-buffer too
  MQ_INT size,		    ///< [in] size of the input-buffer
  struct MqIdS id	    ///< [in] value to print
) {
  if (id.type == MQ_ID_UNUSED)
    strcpy(buf,"unknown");
  else
    mq_snprintf(buf,size,MQ_FORMAT_W,(MQ_WID)id.val);
  return buf;
}

MQ_CST 
pIoLogCom (
  enum MqIoComE com
) {
  switch (com) {
    case MQ_IO_UDS:   return "UDS";
    case MQ_IO_TCP:   return "TCP";
    case MQ_IO_PIPE:  return "PIPE";
  }
  return "";
}

/*****************************************************************************/
/*                                                                           */
/*                           io_event_atom                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
pIoEventAdd(
  MQ_CST caller,		///< [in] who call this function?
  struct MqIoS * const io,	///< [in] io interface data
  MQ_SOCK * sockP		///< [in] socket pointer
)
{
  struct MqS * const context = MQ_CONTEXT_S;
  MQ_INT const buffersize = io->config->buffersize;
  const MQ_SOCK sock = *sockP;
  MQ_INT current = 0;
  socklen_t len;
  //const int yes=1;

//MqDLogV(context,__func__,0,"1. buffersize<%d>, current<%d>\n", buffersize, current);

  // 1. set the send-buffersize
  len = sizeof (MQ_INT);
  MqErrorCheck (SysGetSockOpt (context, sock, SOL_SOCKET, SO_SNDBUF, (MQ_sockopt_optval_T) &current, &len));

//MqDLogV(context,__func__,0,"2. buffersize<%d>, current<%d>\n", buffersize, current);

  if (current < buffersize) {
    len = sizeof (MQ_INT);
    MqErrorCheck (SysSetSockOpt (context, sock, SOL_SOCKET, SO_SNDBUF, (MQ_sockopt_optval_T) &buffersize, len));
    MqErrorCheck (SysGetSockOpt (context, sock, SOL_SOCKET, SO_SNDBUF, (MQ_sockopt_optval_T) &current, &len));
    MqDLogV (context, 5, "set SO_SNDBUF to '%d'\n", current);
  }

  io->config->buffersize = current;

  // 2. set the send-buffersize
  len = sizeof (MQ_INT);
  MqErrorCheck (SysGetSockOpt (context, sock, SOL_SOCKET, SO_RCVBUF, (MQ_sockopt_optval_T) &current, &len));

//MqDLogV(context,__func__,0,"3. buffersize<%d>, current<%d>\n", buffersize, current);

  if (current < buffersize) {
    len = sizeof (MQ_INT);
    MqErrorCheck (SysSetSockOpt (context, sock, SOL_SOCKET, SO_RCVBUF, (MQ_sockopt_optval_T) &buffersize, len));
    MqErrorCheck (SysGetSockOpt (context, sock, SOL_SOCKET, SO_RCVBUF, (MQ_sockopt_optval_T) &current, &len));
    MqDLogV (context, 5, "set SO_RCVBUF to '%d'\n", current);
  }

//MqDLogV(context,__func__,0,"4. buffersize<%d>, current<%d>\n", buffersize, current);

  // set keep alive
  // MqErrorCheck (SysSetSockOpt (context, sock, SOL_SOCKET, SO_KEEPALIVE, (MQ_sockopt_optval_T) &yes, sizeof(yes)));
    

  if (current < io->config->buffersize) {
    io->config->buffersize = current;
  }

  // 3. set to asyncrone
  MqErrorCheck (SysSetAsync (context, sock));

  // 4. save
  pEventAdd (__func__, io->context, sockP);

error:
  return MqErrorStack(context);
}

struct MqS const *
pIoGetMsgqueFromSocket (
  struct MqIoS * const	io,
  MQ_SOCK sock
)
{
  
  return pEventSocketFind(io->context,sock);
}

MQ_BOL 
pIoIsRemote (
  struct MqIoS * const	io
)
{
  switch (io->config->com) {
    case  MQ_IO_UDS:
    case  MQ_IO_TCP:   return MQ_YES;
    case  MQ_IO_PIPE:  return MQ_NO;
  }
  return MQ_NO;
}

#if _DEBUG

void
pIoLog (
  struct MqIoS * const  io,
  MQ_CST proc
)
{
  MqLogV(io->context, proc, 0, "IO: event<%p>\n", (void*) io->event);
}

#endif

END_C_DECLS
