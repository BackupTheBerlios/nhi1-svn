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

#include "io_private.h"
#include "error.h"
#include "buffer.h"
#include "bufferL.h"
#include "sys.h"
#include "sys_com.h"
#include "uds_io.h"
#include "tcp_io.h"
#include "pipe_io.h"
#include "log.h"
#include "send.h"
#include "event.h"

BEGIN_C_DECLS

/// \brief undefined socket, initial value for #MqIoS:sockP
static MQ_SOCK sockUndef = -1;

extern struct MqBufferLS * MqInitBuf;
extern MqFactorySelectorF MqFactorySelector;

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
  struct MqBufferLS * const alfa,
  struct MqIoS ** const out
)
{
  struct MqIoS * const io = *out = (struct MqIoS *) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*io));

  io->context = context;
  // parent and child get both this initial value
  // !!attention if child get an other value then it will wait forever (SysWait)
  // during child cleanup
  io->id.type = MQ_ID_UNUSED;
  io->config = &context->config.io;

  // search for communication type
  if (MQ_IS_PARENT (context)) {

    // init parent
    io->sockP = (MQ_SOCK *)&sockUndef;

    // create event-structure
    MqErrorCheck (pEventCreate (context, &io->event));

    // create communication layer
    switch (io->config->com) {
#if defined(MQ_IS_POSIX)
      case MQ_IO_UDS:
        MqErrorCheck (UdsCreate  (io, &io->iocom.udsSP));
        break;
#endif
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
#if defined(MQ_IS_POSIX)
      case MQ_IO_UDS:
        MqErrorCheck (UdsServer (alfa, io->iocom.udsSP));
        break;
#endif
      case MQ_IO_TCP:
        MqErrorCheck (TcpServer (alfa, io->iocom.tcpSP));
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
  if (unlikely(io == NULL)) return;
  pEventShutdown (io->context);
}

void
pIoCloseSocket (
  struct MqIoS * const io,
  MQ_CST const caller
)
{
  if (unlikely(io == NULL)) return;
  pIoShutdown (io);
  SysCloseSocket (io->context, caller, MQ_YES, io->sockP);
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
      SysWait (MQ_ERROR_IGNORE, &io->id);

      // drop event
      pEventDelete (context);

      // cleanup the io data
      switch (io->config->com) {
#if defined(MQ_IS_POSIX)
	case MQ_IO_UDS:
	  UdsDelete ((struct UdsS **) & io->iocom);
	  break;
#endif
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
#if defined(MQ_IS_POSIX)
    case MQ_IO_UDS:
      MqErrorCheck (UdsConnect (io->iocom.udsSP));
      break;
#endif
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
  struct MqIoS * const io,
  struct timeval const * const timeout,
  EventCreateF const proc
)
{
  return pEventStart (io->context, io->event, timeout, proc);
}

/*****************************************************************************/
/*                                                                           */
/*                                 io_misc                                   */
/*                                                                           */
/*****************************************************************************/

static void
sIoFillArgvC (
  struct MqIoS * const io,	    ///< [in] the current io object
  struct MqBufferLS * alfa2,	    ///< [in] after first #MQ_ALFA
  const MQ_SOCK sock,		    ///< [in] socket for the \e new parent
  char ** arg,			    ///< [out] the new argv array to fill
  MQ_CST startAs		    ///< [in] start as specifier
) {
  struct MqS const * const context = MQ_CONTEXT_S;
  struct MqBufferS **start, **end;

  // 3. add PIPE arguments
  *arg++ = mq_strdup("---duplicate");
  *arg++ = mq_strdup("--socket");
  *arg = (char*) MqSysMalloc(MQ_ERROR_PANIC,20);
  mq_snprintf((char*)*arg++,20,"%i",sock);
  *arg++ = mq_strdup("--buffersize");
  *arg = (char*) MqSysMalloc(MQ_ERROR_PANIC,20);
  mq_snprintf((char*)*arg++,20,"%i",io->config->buffersize);
  *arg++ = mq_strdup("--timeout");
  *arg = (char*) MqSysMalloc(MQ_ERROR_PANIC,20);
  mq_snprintf((char*)*arg++,20,"%lli",io->config->timeout);

  // 4. add context stuff
  if (context->config.isSilent) {
    *arg++ = mq_strdup("--silent");
  }
  if (context->config.debug) {
    *arg++ = mq_strdup("--debug");
    *arg = (char*) MqSysMalloc(MQ_ERROR_PANIC,5);
    mq_snprintf ((char*)*arg++, 5, "%i", context->config.debug);
  }
  if (startAs) {
    *arg++ = mq_strdup(startAs);
  }

  // 5. copy the left over parts starting with the first MQ_ALFA
  if (alfa2) {
    *arg++ = mq_strdup(MQ_ALFA_STR);
    start = alfa2->data;
    end = start+alfa2->cursize;
    for (; start < end; start++,arg++) {
      *arg = mq_strdup((*start)->cur.C);
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
  MQ_CST startAs		    ///< [in] start as specifier
) {
  struct MqS const * const context = io->context;
  MqBufferLAppendC(argv, "---duplicate");
  MqBufferLAppendC(argv, "--socket");
  MqBufferLAppendI(argv, sock);
  MqBufferLAppendC(argv, "--buffersize");
  MqBufferLAppendI(argv, io->config->buffersize);
  MqBufferLAppendC(argv, "--timeout");
  MqBufferLAppendW(argv, io->config->timeout);
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

enum MqErrorE
pIoStartServer (
  struct MqIoS * const io,
  enum IoStartServerE startType,
  MQ_SOCK * sockP,
  struct MqBufferLS ** alfaP,
  struct MqBufferLS * alfa2, // if alfaP!=NULL than alfa2 will be set by alfaP
  struct MqIdS * idP
) {
  struct MqS * const context = io->context;
#if defined(MQ_HAS_THREAD) || defined(HAVE_FORK)
  struct MqFactoryS factory = MqFactoryS_NULL;
  struct MqBufferLS * alfa1 = NULL;
  int start_as_pipe = 0;
#endif
#if defined(MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
    int thread_status = PTHREAD_CREATE_DETACHED;
# elif defined(MQ_IS_WIN32)
    int thread_status = 0;
# endif
#endif

  MQ_CST  name = NULL;

  // alfa is now owned by "pIoStartServer"
  if (alfaP != NULL) {
    alfa1 = *alfaP;
    *alfaP = NULL;
  }

/*
I0
printULS(alfa1)
printLP(alfa1->data)
printLP(alfa1->cur)
*/

  // select the code
rescan:
  switch (startType) {
    case MQ_START_SERVER_AS_PIPE: {
	start_as_pipe = 1;
//printLC("MQ_START_SERVER_AS_PIPE:")
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
	name = alfa1->data[0]->cur.C;
//printLC(name)

	// check if we use the "WORKER" keyword
	if (context->link.isWORKER) {
	  // replace "WORKER" with "MqInitBuf" data
	  name = MqInitBuf->data[0]->cur.C;
	  // replace "WORKER" itself on position "0"
	  MqErrorCheck (MqBufferLDeleteItem (context, alfa1, 0, 1, MQ_YES));
	  // add startup entry function
	  factory = context->setup.Factory;
	} else {
	  // does we use MqMainSelector and is the first entry in alfa1 found?
	  if (MqFactorySelector) factory = MqFactorySelector(name);

	  // if yes then use MqMainToolName (if available)
	  // > atool split ... @ cut ... @ join ...
	  // name is "cut" and next line will replace the name with "atool"
	  name = ( factory.Create.fCall && MqInitBuf ? MqInitBuf->data[0]->cur.C : name );
	}

//printLC(name)
//printLP(fFactory)

	if (factory.Create.fCall) {
#if defined(HAVE_FORK)
	  // continue with "fork"
	  if (context->config.ignoreFork == MQ_NO && (
		context->config.startAs == MQ_START_DEFAULT || 
		context->config.startAs == MQ_START_FORK)) {
//printLC("fork")
	    startType = MQ_START_SERVER_AS_FORK;
	    // if isWORKER than the startup is like a "GenericServer" and not like a "pipe"
	    if (context->link.isWORKER == MQ_YES) {
	      MqBufferLAppend(alfa1, MqBufferCreateC(MQ_ERROR_PANIC, name), 0);
	    }
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
	    // if isWORKER than the startup is like a "GenericServer" and not like a "pipe"
	    if (context->link.isWORKER == MQ_YES) {
	      MqBufferLAppend(alfa1, MqBufferCreateC(MQ_ERROR_PANIC, name), 0);
	    }
#if defined(HAVE_PTHREAD)
	    thread_status = PTHREAD_CREATE_JOINABLE;
#endif
	    goto rescan;
	  }
#endif
	}
	// continue with "spawn" (works allways)
	if (context->config.ignoreSpawn == MQ_YES) {
	  MqErrorDbV2(context, MQ_ERROR_CAN_NOT_START_NON_PIPE_SERVER,(name == NULL ? "unknown" : name));
	}
//printLC("spawn")
	startType = MQ_START_SERVER_AS_SPAWN;
	// only SPAWN need all arguments from MqInitBuf
	if (factory.Create.fCall != NULL) {
	  MqBufferLAppendL(alfa1, MqInitBuf, 0);
	}

	goto rescan;
      }
      break;
#if defined(MQ_HAS_THREAD)
    case MQ_START_SERVER_AS_THREAD: {
//printLC("MQ_START_SERVER_AS_THREAD:")
	// copy the configuration from the "PARENT" server
	if (!start_as_pipe) {
	  factory = context->setup.Factory;
	}

	// MqMainToolName set in "sMqCheckArg" or at application main startup
	if (name == NULL) {
	  if (MqInitBuf == NULL) {
	    return MqErrorDb2(context, MQ_ERROR_NO_INIT);
	  } else {
	    name = MqInitBuf->data[0]->cur.C;
	  }
	}

	// empty alfa1 (GenericServer) starup -> init with MqInitBuf data
	if (alfa1 == NULL) {
	  alfa1 = MqBufferLCreate(20);
	  MqBufferLAppendC(alfa1, name);
	}

	// fill arg with system-arguments
	sIoFillArgvU(io,*sockP,alfa1,"--thread");
	// start the server

/*
I2
MqBufferLLogS(context, alfa1, "alfa1");
MqBufferLLogS(context, alfa2, "alfa2");
*/

	MqErrorCheck (SysServerThread (context, factory, &alfa1, &alfa2, name, thread_status, idP));
      }
      break;
#endif /* MQ_HAS_THREAD */
#if defined(HAVE_FORK)
    case MQ_START_SERVER_AS_FORK: {
//printLC("MQ_START_SERVER_AS_FORK:")
	// copy the configuration from the "PARENT" server
	if (!start_as_pipe) {
	  factory = context->setup.Factory;
	}

	// MqMainToolName set in "sMqCheckArg" or at application main startup
	if (name == NULL) {
	  if (MqInitBuf == NULL) {
	    return MqErrorDb2(context, MQ_ERROR_NO_INIT);
	  } else {
	    name = MqInitBuf->data[0]->cur.C;
	  }
	}

	// empty alfa1 (GenericServer) starup -> init with MqInitBuf data
	if (alfa1 == NULL) {
	  alfa1 = MqBufferLCreate(20);
	  MqBufferLAppendC(alfa1, name);
	}

	// fill arg with system-arguments
	sIoFillArgvU(io,*sockP,alfa1,"--fork");

	// start the server
	MqErrorCheck (SysServerFork (context, factory, &alfa1, &alfa2, name, idP));
	// cleanup socket of the child
	MqErrorCheck(SysCloseSocket (context, __func__, MQ_NO, sockP));
      }
      break;
#endif   /* HAVE_FORK */
    case MQ_START_SERVER_AS_SPAWN: {
//printLC("MQ_START_SERVER_AS_SPAWN:")
	char **argV, **arg;
	// add 20 item's as additional space
	argV = arg = (char **) MqSysMalloc (context, sizeof(*alfa1) * (
	    (alfa1 != NULL ? alfa1->cursize : 0) + (alfa2 ? alfa2->cursize : 0) + 20 
	));

	// init main
	if (name == NULL) {
	  if (MqInitBuf == NULL) {
	    return MqErrorDb2(context, MQ_ERROR_NO_INIT);
	  } else {
	    name = MqInitBuf->data[0]->cur.C;
	  }
	}

	// empty alfa1 (GenericServer) starup -> init with MqInitBuf data
	if (alfa1 == NULL) {
          int i;
	  for (i=0; i<MqInitBuf->cursize; i++) {
	    *arg++ = mq_strdup(MqInitBuf->data[i]->cur.C);
	  }
	} else {
	  struct MqBufferS **start, **end;

	  // copy everything befor the first MQ_ALFA
	  start = alfa1->data;
	  end = start+alfa1->cursize;
	  for (; start < end; start++) {
	    *arg++ = mq_strdup((*start)->cur.C);
	  }
	}

	// fill arg with system-arguments
	sIoFillArgvC(io, alfa2, *sockP, arg, "--spawn");
	// start the server

/*
        {
int i;
char ** xarg = argV;
printLC(name)
for (i=0; *xarg != NULL; xarg++, i++) {
  MqDLogX (context, __func__, 0, "alfa1[%2i]=%s\n",i, *xarg);
}
        }
*/

	MqErrorCheck (SysServerSpawn (context, argV, name, idP));

	// cleanup array's
	SysFreeArgvArray (&argV);
	// cleanup socket of the child
	MqErrorCheck(SysCloseSocket (context, __func__, MQ_NO, sockP));
      }
      break;
#if defined(HAVE_FORK)
    case MQ_START_SERVER_AS_INLINE_FORK: {
//printLC("MQ_START_SERVER_AS_INLINE_FORK:")
	if (name == NULL && MqInitBuf != NULL) {
	  name = MqInitBuf->data[0]->cur.C;
	} else {
	  return MqErrorDb2(context, MQ_ERROR_NO_INIT);
	}
	// case: the function GenericServer is listen on a "tcl" or "uds" connection
	// and start for every incomming connection a new process with "fork". !no!
	// initialization have to be done because we reuse the current state.
	MqErrorCheck (SysFork (context, idP));
	// if pid != 0 -> this is the parent just continue with default "client" code
	if ((*idP).val.process == 0) {
	  // pid = 0 -> this is the fork child
	  context->statusIs = (enum MqStatusIsE) (context->statusIs | MQ_STATUS_IS_FORK);
	  // we don't need to cleanup argv1/2 because the INLINE_FORK is done
	  // with an empty "alfa"
	  return MQ_OK;
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
    MqDLogX(context,__func__,4,"finish start server '%s' as '%s' with ID '%s'\n",
		name, pIoLogStartType(startType), pIoLogId(buf,50,*idP));
  }

  // cleanup alfa1/2
  MqBufferLDelete (&alfa1);
  MqBufferLDelete (&alfa2);

error:
  return MqErrorStack (context);
}

MQ_BOL
pIoCheck (
  struct MqIoS * const io
) {
  return (io && io->sockP ? (*(io->sockP) >= 0) : MQ_NO);
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
  switch (id.type) {
    case MQ_ID_PROCESS:
      mq_snprintf(buf,size,"%i",id.val.process);
      break;
#if defined(MQ_HAS_THREAD)
    case MQ_ID_THREAD:
      mq_snprintf(buf,size,"%lu",(long unsigned int)id.val.thread);
      break;
#endif
    case MQ_ID_UNUSED:
      strcpy(buf,"unknown");
      break;
  }
  return buf;
}

MQ_CST 
pIoLogCom (
  enum MqIoComE com
) {
  switch (com) {
#if defined(MQ_IS_POSIX)
    case MQ_IO_UDS:   return "UDS";
#endif
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
  struct MqIoS * const io,	///< [in] io interface data
  MQ_SOCK * sockP		///< [in] socket pointer
)
{
  struct MqS * const context = MQ_CONTEXT_S;
  MQ_INT const buffersize = io->config->buffersize;
  const MQ_SOCK sock = *sockP;
  MQ_INT current;
  socklen_t len;

  // 1. set the send-buffersize
  len = sizeof (MQ_INT);
  MqErrorCheck (SysGetSockOpt (context, sock, SOL_SOCKET, SO_SNDBUF, (MQ_sockopt_optval_T) &current, &len));
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
  if (current < buffersize) {
    len = sizeof (MQ_INT);
    MqErrorCheck (SysSetSockOpt (context, sock, SOL_SOCKET, SO_RCVBUF, (MQ_sockopt_optval_T) &buffersize, len));
    MqErrorCheck (SysGetSockOpt (context, sock, SOL_SOCKET, SO_RCVBUF, (MQ_sockopt_optval_T) &current, &len));
    MqDLogV (context, 5, "set SO_RCVBUF to '%d'\n", current);
  }

  if (current < io->config->buffersize) {
    io->config->buffersize = current;
  }

  // 3. set to asyncrone
  MqErrorCheck (SysSetAsync (context, sock));

  // 4. save
  pEventAdd (io->context, sockP);

error:
  return MqErrorStack(context);
}

MQ_TIME_T
pIoGetTimeout(
  struct MqIoS * const io	///< [in] io interface data
)
{
  return io->config->timeout;
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
#if defined(MQ_IS_POSIX)
    case  MQ_IO_UDS:
#endif
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
  MqDLogX(io->context, proc, 0, "IO: event<%p>\n", io->event);
}

#endif

END_C_DECLS


