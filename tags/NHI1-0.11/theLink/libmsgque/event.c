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
#include "mq_io.h"

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

/// \brief everything \e io need for local storage
struct MqEventS {
  fd_set fdset;			///< used in 'select'
  int fdmax;			///< the maximum file-handle in \e fdset
  struct MqS ** DataL;		///< list of #MqS objects
  MQ_SIZE DataLNum;		///< size of \e DataL
  MQ_SIZE DataLCur;		///< first free position in \e DataL, <TT>DataLCur <= DataLNum</TT>
#if defined (MQ_HAS_THREAD)
# if defined(HAVE_PTHREAD)
  pthread_t thread;		///< the thread event link to
# else
  DWORD thread;		///< the thread event link to
# endif
#endif
  
};

//######################################################################

static void
sEventDeleteAllClient (
  struct MqEventS * const event
)
{
  // are sockets available? if not do nothing
  if (event == NULL || event->fdmax < 0) return;

  // attention, every delete on a parent-client change the event->DataL too (as side-effect)
  while (event->DataLCur) {
    MQ_SIZE i;
    for (i=0; i<event->DataLCur; i++) {
      struct MqS *context = event->DataL[i];
      if (MQ_IS_CLIENT(context) ) {
	MqDLogC(context,4,"FORCE delete\n");
	// MqLinkDelete requiert -> perl does in "MqContextDelete" only an refCount derease
	// but no event-free
	MqLinkDelete (context);
	// if deletion is not done -> shutdown the link
	// exit "for" loop to accept new "start" and "end"
	break;
      }
    }
    // list has no CLIENT ... exit "while" loop
    if (i >= event->DataLCur) break;
  }
}

#if defined (MQ_HAS_THREAD)

/// \brief Thread-Specific-Data (TSD) key
static MqThreadKeyType event_key = MqThreadKeyNULL;

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

/// \brief one-time initializer for the TSD key
void
EventCreate(void)
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

void
EventDelete(void)
{
  if (event_key != MqThreadKeyNULL) {
    struct MqEventS * sysevent = (struct MqEventS *) MqThreadGetTLS(event_key);
    if (sysevent != NULL) {
      MqThreadSetTLS(event_key, NULL);
      sEventDeleteAllClient(sysevent);
      MqSysFree(sysevent->DataL);
      MqSysFree(sysevent);
    }
  }
}

void
EventCleanup(void)
{
  if (event_key != MqThreadKeyNULL) {
    struct MqEventS * sysevent = (struct MqEventS *) MqThreadGetTLS(event_key);
    if (sysevent != NULL) {
      sEventDeleteAllClient(sysevent);
    }
  }
}
#undef PREFIX

//######################################################################
#else	/* ! MQ_HAS_THREAD */

static struct MqEventS * sysevent = NULL;

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
EventCreate(void)
{
}

void
EventDelete(void)
{
  if (sysevent != NULL) {
    sEventDeleteAllClient(sysevent);
    MqSysFree(sysevent->DataL);
    MqSysFree(sysevent);
  }
}

void
EventCleanup(void)
{
  if (sysevent != NULL) {
    sEventDeleteAllClient(sysevent);
  }
}
  
#define MqThreadSelf() 0L

#endif   /* ! MQ_HAS_THREAD */

//######################################################################
/// \attention only call this function for a \e PARENT context
/// \attention this function set "context->link.io->sockP"
void
pEventAdd (
  MQ_CST caller,
  struct MqS * const context,
  MQ_SOCK * const sockP
)
{
  const MQ_SOCK sock = *sockP;

  // filter to check if work is available
  if (context == NULL || MQ_IS_CHILD(context)) {
    return;
  } else {
    register struct MqEventS * event = context->link.io->event;

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

    MqDLogV(context,4,"TLS-Id<%p> Socket<%i> DataLCur<%i>\n", event, sock, event->DataLCur);

    event->DataLCur++;

//pEventLog(msgque, event, __func__);
  }
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
  MqLogData(context, "select");
  if (MQ_IS_CLIENT(context)) {
    int i;
    for (i=0; i<=event->fdmax; i++) {
      if (FD_ISSET(i,&fds))
        MqDLogV(context,0,"sock<%i>, set<YES>\n", i);
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
    eventctx->refCount++;
//printLV("eventctx<%p>, sock<%d>\n", eventctx, sock);
    ret = (*proc)(eventctx);
    eventctx->refCount--;
    switch (ret) {
      case MQ_OK:
	return MQ_OK;
      case MQ_ERROR: {
	// case 1. check on "error" for the same "context"
	if (context == eventctx->error.errctx) {
	  // do nothing just move the error to the calling function
	  MqErrorCopy (context, eventctx);
	  goto error;
// code not required anymore -> an exit is just a "normal" error
	} else if (MqErrorIsEXIT(eventctx)) {
/*
M0
XI0(context)
XI0(eventctx)
XI0(eventctx->error.errctx)
XI0(pMqGetFirstParent(context))
XI0(pMqGetFirstParent(eventctx->error.errctx))
*/
	  if (
	      /* every exit in a client is reported, but only if the ignoreExit is NOt set for the target */
	      (MQ_IS_CLIENT(eventctx->error.errctx) && context->setup.ignoreExit == MQ_NO) ||
	      (MQ_IS_CLIENT(context) && pMqGetFirstParent(context) == pMqGetFirstParent(eventctx->error.errctx))
	  ) {
//M1
	    // report "link-down-error" from a "parent-context" to a "client-context"
	    MqErrorCopy (context, eventctx);
	    goto error;
	  } else {
//M2
	    // the GC have to handle this
	    MqErrorReset (context);
	  }
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
	  if (MqServiceIsTransaction(eventctx)) {
	    // in a transaction
	    // return a "server" error using a "_RET" package -> reset the error
	    MqSendSTART(eventctx);
	    MqSendRETURN(eventctx);
	  } else {
	    // no transaction
	    // return server error from a MqSendEND call asyncrone -> reset the error
	    MqSendERROR (eventctx);
	  }
	  // error was "reset" and "pEventStart" context is clean
	  break;
	} else {
	  // copy the error to the "pEventStart" context
	  MqErrorCopy(context, eventctx);
	  goto error;
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
    // reason: GarbageCollection will delete these "old" context on
    // exit and will trigger a "full" transaction delete -> "test: slave-Y-1-"
    for (; start < end; start++) {
      pContextDeleteLOCK(*start);
    }
    // now reset the event data
    sEventReset(sysevent);
  }

  // report event back
  if (likely (eventP != NULL))
    *eventP = sysevent;

  MqDLogV(context,4,"TLS-Id<%p>\n", (void*)sysevent);

error:
  return MqErrorStack(context);
}

/// \attention only call this function for a \e PARENT context
/// \attention this function free "context->link.io->sockP"
void
pEventDel (
  MQ_CST caller,
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
        !context ||			// no "context" data
        !context->link.io ||		// no "io" data
        !context->link.io->event ||	// no "event" data
        MQ_IS_CHILD(context )		// we only delete PARENT from the event-list
     ) return;

  event = context->link.io->event;
  start = event->DataL;
  end = event->DataL + event->DataLCur;

//pEventLog(context, event, "pEventDelete-1");

  // find + clear
rescan:
  for (; start < end; start++) {
    sock = *((*start)->link.io->sockP);
    if (*start == context) {
      delsock = (sock < 0 ? -sock : sock);

//printLV("caller<%s>, sock<%d>\n", caller, delsock);

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
  if (MQ_IS_PARENT(context)) {
    if (MQ_IS_SERVER(context)) {
      sEventDeleteAllClient(context->link.io->event);
    } else {
      pEventDel (__func__, context);
    }
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
  MqLogV(context, proc, 0, ">>>>> MqEventS<%p>\n", (void*)event);
  MqLogV(context, proc, 0, "fdmax    = %i\n", event->fdmax);
  MqLogV(context, proc, 0, "DataLNum = %i\n", event->DataLNum);
  MqLogV(context, proc, 0, "DataLCur = %i\n", event->DataLCur);
  start = event->DataL;
  end = event->DataL + event->DataLCur;
  for (; start < end; start++) {
    struct MqS * mq = *start;
    MqLogV(context, proc, 0, " -> context<%p>, server<%s>, sock<%i>\n", (void*) mq, 
	(MQ_IS_SERVER(mq)?"yes":"no"), *(mq->link.io->sockP));
  }
  MqLogV(context, proc, 0, "<<<<< MqEventS<%p>\n", (void*) event);
}
#endif /* _DEBUG */

END_C_DECLS
