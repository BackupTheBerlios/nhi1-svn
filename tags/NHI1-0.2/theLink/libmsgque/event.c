/**
 *  \file       theLink/libmsgque/event.c
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
#include "event.h"
#include "sys.h"
#include "sys_com.h"
#include "io_private.h"

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

/// \brief everything \e io need for local storage
struct MqEventS {
  fd_set fdset;			///< used in 'select'
  int fdmax;			///< the maximum file-handle in \e fdset
  struct MqS ** DataL;		///< list of #MqS objects
  int DataLNum;			///< size of \e DataL
  int DataLCur;			///< first free position in \e DataL, <TT>DataLCur <= DataLNum</TT>
#if defined (MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
  pthread_t thread;		///< the thread event link to
# else
  DWORD thread;		///< the thread event link to
# endif
#endif
  
};

#if defined(_MSC_VER)
#define PREFIX
#else
#define PREFIX static
PREFIX void sEventCreate (void) __attribute__ ((constructor)); 
PREFIX void sEventDelete (void) __attribute__ ((destructor)); 
#endif /* !_MSC_VER */

//######################################################################

static void
sEventDeleteAllClient (
  struct MqEventS * const event
)
{
  // process the results
  struct MqS *context;
  struct MqS **start, **end;

  // are sockets available? if not do nothing
  if (event == NULL || event->fdmax < 0) return;

  // attention, every delete on a parent-client change the event->DataL too (as side-effect)
  while (event->DataLCur) {
    start = event->DataL;
    end   = event->DataL + event->DataLCur;
    for (; start < end; start++) {
      context = *start;
      if (MQ_IS_CLIENT(context)) {
	MqDLogC(context,4,"FORCE delete\n");
	//MqLogMsgque(context,__func__);
	//MqLinkDelete(context);
	MqContextDelete (&context);
	// exit "for" loop to accept new "start" and "end"
	break;
      }
    }
    // list has no CLIENT ... exit "while" loop
    if (start == end) break;
  }
}

#if defined (MQ_HAS_THREAD)

#if defined(HAVE_PTHREAD)
#  define MqThreadSelf() pthread_self()
#  define MqThreadGetTLS(k) pthread_getspecific(k)
#  define MqThreadSetTLS(k,v) pthread_setspecific(k,v)
#  define MqThreadSetTLSCheck(k,v) pthread_setspecific(k,v)
#  define MqThreadKeyType pthread_key_t
#  define event_key_null PTHREAD_KEYS_MAX
#else
#  define MqThreadSelf() GetCurrentThreadId()
#  define MqThreadGetTLS(k) TlsGetValue(k)
#  define MqThreadSetTLS(k,v) TlsSetValue(k,v)
#  define MqThreadSetTLSCheck(k,v) (TlsSetValue(k,v) == 0)
#  define MqThreadKeyType DWORD
#  define event_key_null TLS_OUT_OF_INDEXES
#endif

/// \brief Thread-Specific-Data (TSD) key
static MqThreadKeyType event_key = event_key_null;

/// \brief Check (and if not available) allocate TLS storage
/// \context
/// \param[out] eventP the new poiter to the \e event object
/// \retMqErrorE
static enum MqErrorE
sEventAlloc(
  struct MqS * const context,
  struct MqEventS ** eventP
)
{
  struct MqEventS * sysevent = NULL;
  MqErrorReset(context);
  if ((sysevent = (struct MqEventS *) MqThreadGetTLS(event_key)) == NULL) {
    sysevent = (struct MqEventS *) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*sysevent));
    sysevent->fdmax = -1;
    sysevent->thread = MqThreadSelf();
    if (MqThreadSetTLSCheck(event_key,sysevent)) {
      MqSysFree(sysevent);
      return MqErrorDbV2 (context,MQ_ERROR_CAN_NOT, "thread TLS data");
    }
  }
  *eventP = sysevent;
  //MqDLogV(MqErrorGetMsgque(context), 4, "create EVENT<%p>\n", sysevent);
  return MQ_OK;
}

void
sEventFree(void)
{
  struct MqEventS * sysevent;
  if (event_key == event_key_null) return;

  sysevent = (struct MqEventS *) MqThreadGetTLS(event_key);

  if (sysevent != NULL) {
    MqThreadSetTLS(event_key, NULL);
    sEventDeleteAllClient(sysevent);
    MqSysFree(sysevent->DataL);
    MqSysFree(sysevent);
  }
}

/// \brief one-time initializer for the TSD key
PREFIX void
sEventCreate(void)
{
#if defined(HAVE_PTHREAD)
  if (pthread_key_create(&event_key, NULL) != 0) {
    MqPanicC(MQ_ERROR_PANIC,__func__,-1,"unable to 'pthread_key_create'");
  }
#else
  if ((event_key = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
    MqPanicC(MQ_ERROR_PANIC,__func__,-1,"unable to 'TlsAlloc'");
  }
#endif
}

PREFIX void
sEventDelete(void)
{
  sEventFree();
}
#undef PREFIX

//######################################################################
#else	/* ! MQ_HAS_THREAD */

struct MqEventS * sysevent = NULL;

static enum MqErrorE
sEventAlloc(
  struct MqS * const context,
  struct MqEventS ** eventP
)
{
  MqErrorReset(context);
  if (sysevent == NULL) {
    sysevent = (struct MqEventS*) MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof (*sysevent));
    sysevent->fdmax = -1;
  }
  *eventP = sysevent;
  return MQ_OK;
}

void
sEventFree(void)
{
  if (sysevent != NULL) {
    sEventDeleteAllClient(sysevent);
    MqSysFree(sysevent);
  }
}

static void
sEventCreate(void)
{
}

static void
sEventDelete(void)
{
  sEventFree();
}

#define MqThreadSelf() 0L

#endif   /* ! MQ_HAS_THREAD */

//######################################################################
/// \attention only call this function for a \e PARENT context
/// \attention this function set "context->link.io->sockP"
void
pEventAdd (
  struct MqS * const context,
  MQ_SOCK * const sockP
)
{
  const MQ_SOCK sock = *sockP;
  register struct MqEventS * event = context->link.io->event;

  // filter to check if work is available
  if (	    
        !context ||              // no msgque no data
        MQ_IS_CHILD(context)     // we only add PARENT to the event-list
     ) return;

  // check if enough space is available
  if (event->DataLCur >= event->DataLNum) {
    event->DataLNum += 10;
    event->DataL = (struct MqS **) MqSysRealloc (MQ_ERROR_PANIC, event->DataL, 
        sizeof (*event->DataL) * event->DataLNum);
  }
  // ATTENTION: we don't check on double insert !!
  event->DataL[event->DataLCur] = context;

  if (sock > event->fdmax)
    event->fdmax = sock;

  if (sock >= 0) {
    FD_SET (sock, &event->fdset);

    // add the link between "io" and the current socket handle
    context->link.io->sockP = sockP;
  }

  event->DataLCur++;

  MqDLogV(context,4,"sock<%i> DataLCur<%i>\n", sock, event->DataLCur);

//pEventLog(msgque, event, __func__);
}

/// brief delete all events
void
sEventReset(
  struct MqEventS * event
)
{
  MqSysFree (event->DataL);
  event->DataLNum = 0;
  event->DataLCur = 0;
  event->fdmax = -1;
  FD_ZERO(&event->fdset);
}

enum MqErrorE
pEventCheck (
  struct MqS * const context,
  struct MqEventS * event,
  enum MqIoSelectE const typ,
  struct timeval * const timeout
)
{
  // init
  fd_set fds, *readfds = NULL, *writefds = NULL;

  // are sockets available? if not do nothing
  if (event == NULL || event->fdmax < 0)
    return MQ_CONTINUE;

  // fill fd_set for use in 'select'
  memcpy (&fds, &event->fdset, sizeof (fd_set));

  // composit
  switch (typ) {
    case MQ_SELECT_RECV:
      readfds = &fds;
      break;
    case MQ_SELECT_SEND:
      writefds = &fds;
      break;
  }

/*
{
  MqLogData(context, "select");
  if (MQ_IS_CLIENT(context)) {
    int i;
    for (i=0; i<=event->fdmax; i++) {
      if (FD_ISSET(i,&fds))
        MqDLogX(context,__func__,0,"sock<%i>, set<YES>\n", i);
    }
  }
}
*/

  // do the select
  return SysSelect (context, event->fdmax + 1, readfds, writefds, timeout);
}

enum MqErrorE
pEventStart (
  struct MqS * const context,
  struct MqEventS * event,
  struct timeval const * const timeout,
  EventCreateF const proc
)
{
  // init
  fd_set fds;
  struct MqS ** start;
  struct MqS ** end;
  register struct MqS * eventctx = NULL;
  register MQ_SOCK sock;
  enum MqErrorE ret;

  // are sockets available? if not do nothing
  if (!event || event->fdmax < 0)
    return MQ_CONTINUE;

  // fill fd_set for use in 'select'
  fds = event->fdset;

  // do the select and process the results
  ret = SysSelect (context, event->fdmax + 1, &fds, NULL, timeout);
  switch (ret) {
    case MQ_CONTINUE: return MQ_CONTINUE;
    case MQ_OK:	      break;
    case MQ_ERROR:    goto error;
    case MQ_EXIT:     return MQ_EXIT;
  }

  // process the results
  start = event->DataL;
  end = event->DataL + event->DataLCur;
  for (; start < end; start++) {
    eventctx = *start;
    sock = *(eventctx->link.io->sockP);
    // is the context still in duty ? (sock >= 0)
    if (sock < 0 || !FD_ISSET (sock, &fds)) continue;
    // found valid socket -> call it
    switch ((*proc)(eventctx)) {
      case MQ_OK:
	return MQ_OK;
      case MQ_ERROR: {
	// the following error's have to be handled:
	// 1. error happen in the same context as "pEventStart" was called
	// 2. "pEventStart" is called on a server context and get an error on a 
	//	client context
	// 3. error happen in the server context
	// 4. error happen in different client context	

	if (context == eventctx) {
	  // case 1.
	  // do nothing just report the error
	  goto error;
	} else if (MQ_IS_SERVER(context) && MQ_IS_CLIENT(eventctx)) {
	  // case 2.
	  // copy the error from the client to the server context
	  MqErrorCopy(context, eventctx);
	  // send the error to the server-client and reset the server error
	  MqSendERROR(context);
	  break;
	} else if (MQ_IS_SERVER(eventctx)) {
	  // case 3. -> a server error have to be reported
	  // we have an MQ error
	  // check to be in a transaction or not
	  if (MqIsTransaction(eventctx) == MQ_NO) {
	    // no transaction
	    // return server error from a MqSendEND call asyncrone -> reset the error
	    MqSendERROR (eventctx);
	  } else {
	    // in a transaction
	    // return a "server" error using a "_RET" package -> reset the error
	    MqSendSTART(eventctx);
	    MqSendRETURN(eventctx);
	  }
	  // error was "reset" and "pEventStart" context is clean
	  break;
	} else {
	  // copy the error to the "pEventStart" context
	  MqErrorCopy(context, eventctx);
	  goto error;
	}
      }
      case MQ_EXIT: {
	// the MQ_EXIT is handeled here becasue this is the only place who
	// know the caller object (throught error) and the exit object (eventctx)
	struct MqS * exitctx  = eventctx->link.exitctx;
	eventctx->link.exitctx = NULL;
	// security check (propably not used)
	if (exitctx == NULL) MqPanicSYS(context);
	// case 1. delete myself
	if (context == exitctx)  {
	  // can not delete myself -> upper code have to handle this
	  return MQ_EXIT;
	}
	if (MQ_IS_SERVER(exitctx)) {
	  if (MQ_IS_CHILD(exitctx)) {
	    // case 2. exitctx is a server-child and not in use
	    pMqGetFirstParent(context)->link.deleteProtection = MQ_YES;
	    MqLinkDelete(exitctx);
	    pMqGetFirstParent(context)->link.deleteProtection = MQ_NO;
	    return MqErrorGetCode(context);
	  } else {
	    // case 3. there is only one server
	    // context != exitctx && exitctx == SERVER+PARENT
	    // shift EXIT to the toplevel
	    return MqErrorCopy (context, exitctx);
	  }
	} else {
	  // case 4 exitctx is a client -> the client is allways delete 
	  // at the end of the client-executable
	  // copy the exit message to the context
	  return MqErrorCopy (context, exitctx);
	}
      }
      case MQ_CONTINUE:	  // never used
	break;
    }
    // allways break after one was found, event->DataL could be invalid
    break;
  }

error:
  return MqErrorStack(context);
}

enum MqErrorE
pEventCreate (
  struct MqS * const context,
  struct MqEventS ** const eventP
) {
  struct MqEventS * sysevent = NULL;

  // get/create the TLS data
  MqErrorCheck(sEventAlloc(context, &sysevent));

  // if the context was created by a "fork" cleanup events first
  if (MQ_IS_PARENT(context) && context->statusIs & MQ_STATUS_IS_FORK) {
    struct MqS **start = sysevent->DataL;
    struct MqS **end   = sysevent->DataL + sysevent->DataLCur;
    // make all "old" context undelete-able
    // reason: perl GarbageCollection will delete these "old" context on
    // exit and will trigger a "full" transaction delete -> "test: slave-Y-1-"
    for (; start < end; start++) {
      // block all "open" context
      (*start)->MqContextDelete_LOCK = MQ_YES;
    }
    // now reset the event data
    sEventReset(sysevent);
  }

  // report event back
  if (likely (eventP != NULL))
    *eventP = sysevent;

  MqDLogV(context,4,"TSD-Id<%p>, Thread-Id<0x%lx>\n", sysevent, (long unsigned int)MqThreadSelf());

error:
  return MqErrorStack(context);
}

/// \attention only call this function for a \e PARENT context
/// \attention this function free "context->link.io->sockP"
void
pEventShutdown (
  struct MqS const * const context
)
{
  struct MqEventS * event;
  struct MqS ** start;
  struct MqS ** end;
  int max = -1;  
  MQ_SOCK sock = -1, delsock = -1;

  // filter to check if work is available
  if (	    
        !context ||			// no "msgque" data
        !context->link.io ||		// no "io" data
        !context->link.io->event ||   // no "event" data
        MQ_IS_CHILD(context )		// we only delete PARENT from the event-list
     ) return;

  event = context->link.io->event;
  start = event->DataL;
  end = event->DataL + event->DataLCur;

//pEventLog(msgque, event, "pEventDelete-1");

  // find + clear
rescan:
  for (; start < end; start++) {
    sock = *((*start)->link.io->sockP);
    if (*start == context) {
      delsock = (sock < 0 ? -sock : sock);
      // free fdset
      FD_CLR (delsock, &event->fdset);
      // move following data 'one' to the left
      memmove(start,start+1,(end-start-1)*sizeof(*start));
      // decrease end
      end -= 1;
      event->DataLCur--;
      // we have shift the date one to the left -> get the new socket
      goto rescan;
    }
    // find the new max
    if (sock > max) {
      max = sock;
    }
  }

  // cleanup
  event->fdmax = max;

  // last one -> delete the infrastructure
  if (event->DataLCur == 0) {
    sEventReset(event);
  }

  // report status back
  MqDLogV(context,4,"sock<%i> DataLCur<%i>\n", delsock, event->DataLCur);
}

/// get the msgque object from a socket
struct MqS const *
pEventSocketFind (
  struct MqS const * const context,
  MQ_SOCK sock
)
{
  struct MqEventS * event;
  struct MqS ** start;
  struct MqS ** end;
  MQ_SOCK fsock;

  // filter to check if work is available
  if (	    
        !context ||			// no "msgque" data
        !context->link.io ||		// no "io" data
        !context->link.io->event	// no "event" data
     ) return NULL;

  event = context->link.io->event;
  start = event->DataL;
  end = event->DataL + event->DataLCur;

  // find the socket
  for (; start < end; start++) {
    fsock = *((*start)->link.io->sockP);
    if (fsock == sock || fsock == -sock) return *start;
  }
  return NULL;
}

/// \attention only call this function for a \e PARENT context
/// \attention this function free "context->link.io->sockP"
void
pEventDelete (
  struct MqS const * const context
)
{
  // if SERVER delete all CLIENT too
  if (MQ_IS_SERVER_PARENT(context)) {
    sEventDeleteAllClient(context->link.io->event);
  }

  // free memory 
  context->link.io->event = NULL;
}

/*****************************************************************************/
/*                                                                           */
/*                                debugging                                  */
/*                                                                           */
/*****************************************************************************/

#ifdef _DEBUG
void pEventLog (
  struct MqS const * const context,
  struct MqEventS * event,
  MQ_CST const proc
)
{
  struct MqS ** start, ** end;
  if (!context || !event || context->config.isSilent == MQ_YES) return;
  MqDLogX(context, proc, 0, ">>>>> MqEventS<%p>\n", event);
  MqDLogX(context, proc, 0, "fdmax    = %i\n", event->fdmax);
  MqDLogX(context, proc, 0, "DataLNum = %i\n", event->DataLNum);
  MqDLogX(context, proc, 0, "DataLCur = %i\n", event->DataLCur);
  start = event->DataL;
  end = event->DataL + event->DataLCur;
  for (; start < end; start++) {
    struct MqS * mq = *start;
    MqDLogX(context, proc, 0, " -> context<%p>, server<%s>, sock<%i>\n", mq, 
	(MQ_IS_SERVER(mq)?"yes":"no"), *(mq->link.io->sockP));
  }
  MqDLogX(context, proc, 0, "<<<<< MqEventS<%p>\n", event);
}
#endif /* _DEBUG */

END_C_DECLS










