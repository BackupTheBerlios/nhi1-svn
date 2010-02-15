/**
 *  \file       theLink/libmsgque/msgque.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#define MQ_PRIVATE_IS_MAIN 1

#include "main.h"
#include "link.h"
#include "buffer.h"
#include "mq_io.h"
#include "token.h"
#include "error.h"
#include "sys.h"

//#ifdef HAVE_STRINGS_H
//#  include <strings.h>
//#endif

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

void EventCleanup (void);
void GcDelete (void);
MQ_EXTERN struct MqBufferLS * MqInitBuf = NULL;
MQ_EXTERN MqFactorySelectorF MqFactorySelector = NULL;
static void pMqCleanup (void) __attribute__ ((destructor));

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

static void pMqCleanup(void)
{
  if (MqInitBuf != NULL) MqBufferLDelete(&MqInitBuf);
}

MQ_STR
MqHelp ( MQ_CST  tool )
{
  MqBufferCreateStatic (buf, 1000);

  if (tool) {
      MqBufferAppendV (buf, "  %s [ARGUMENT]... syntax:\n", tool);
      MqBufferAppendV (buf, "    aclient [OPTION]... %c %s [OPTION]... %c [ARGUMENT]...\n", MQ_ALFA, tool, MQ_ALFA);
      MqBufferAppendV (buf, "\n");
  }
  MqBufferAppendV (buf, "  msgque [OPTION]:\n");
  MqBufferAppendV (buf, "    --help-msgque    print msgque specific help\n");

  return (MQ_STR) buf->data;
}

/*****************************************************************************/
/*                                                                           */
/*                             create / delete                               */
/*                                                                           */
/*****************************************************************************/

struct MqBufferLS*
MqInitCreate ()
{
  if (MqInitBuf != NULL) MqBufferLDelete(&MqInitBuf);
  MqInitBuf = MqBufferLCreate(2);
  return MqInitBuf;
}

/*****************************************************************************/
/*                                                                           */
/*                                context/event                               */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
sCallEventProc (
  struct MqS * const context
)
{
  struct MqS * cldCtx;
  MQ_INT NUM=1, CONTINUE=0;
  struct pChildS * child;
  // call my own event-proc
  if (context->bits.EventProc_LOCK == MQ_NO && 
	context->link.bits.onCreateEnd == MQ_YES &&
	  context->setup.Event.fCall != NULL) {
    context->bits.EventProc_LOCK = MQ_YES;
    switch (MqCallbackCall(context, context->setup.Event)) {
      case MQ_OK:
	break;
      case MQ_CONTINUE:
	if (context->link.bits.onShutdown == MQ_YES || context->link.bits.isConnected == MQ_NO) {
	  CONTINUE++;
	}
	MqErrorReset(context);
	break;
      case MQ_ERROR:
	context->bits.EventProc_LOCK = MQ_NO;
	goto error;
    }
    context->bits.EventProc_LOCK = MQ_NO;
  }
  // call the event-proc's of my child's
  for (child = context->link.childs; child != NULL; child=child->right) {
    NUM++;
    cldCtx = child->context;
    if (cldCtx != NULL && cldCtx->setup.Event.fCall != NULL) {
      switch (sCallEventProc (cldCtx)) {
	case MQ_OK:
	  break;
	case MQ_CONTINUE:
	  CONTINUE++;
	  MqErrorReset(cldCtx);
	  break;
	case MQ_ERROR:
	  MqErrorCopy(context, cldCtx);
	  goto error;
	  break;
      }
    }
  }
  // with "ignoreExit" all "context" have to be on "CONTINUE" to trigger an exit
  if (context->setup.ignoreExit == MQ_NO || NUM != CONTINUE) {
    return MqErrorGetCodeI(context);
  } else {
    return MQ_CONTINUE;
  }
error:
  context->setup.ignoreExit = MQ_NO;
  return MqErrorStack(context);
}

enum MqErrorE
pWaitOnEvent (
  struct MqS * const context,
  const enum MqIoSelectE what,
  const MQ_TIME_T timeout
)
{
  struct MqS * const parent = pMqGetFirstParent (context);
  const MQ_TIME_T startT = time (NULL);
  register MQ_TIME_T nowT;

  struct timeval tv;
  const int MqSetDebugLevel(context);

  // set initial timeout
  if (context->setup.Event.fCall != NULL) {
    // we have an extern event-handlig procedure (proc) ... give this code the chance 
    // to do somethig usefull
    tv.tv_sec = 0L;
    // why 10000 uSec, to let the select a chance to process the direct response
    // on a previously submitted 'MqSendEND_AND_WAIT' packages (e.g. special _ERR)
    tv.tv_usec = 10000L;
  } else {
    tv.tv_sec = (long)timeout;
    tv.tv_usec = 0L;
  }

  MqDLogCL(context,6,"START loop\n");
  do {
    // 1. test on 'select'
    switch (pIoSelectAll (context->link.io, what, &tv)) {
      case MQ_CONTINUE:	break;		  // nothing found wait for next event
      case MQ_OK:	goto end;	  // found event
      case MQ_ERROR:	goto error;	  // something wrong happen
    }

    // 2. call external event-proc
    if (context->setup.Event.fCall != NULL) {
      // this guarding with "____" is important to detect and break-out-of "nested" event calls
      // pWaitOnEvent
      //   -> guard 
      //   -> *proc 
      //     ... -> pWaitOnEvent -> found-an-event
      //   -> check-guard
      pTokenSetCurrent(context->link.srvT,"____");

      //MqDLogC(context,7,"call fEvent in<%p>\n", msgque);
      switch (sCallEventProc (parent)) {
	case MQ_OK:
	  break;
	case MQ_CONTINUE:
	  context->setup.ignoreExit = MQ_NO;
	  return MqErrorCreateEXIT (context);
	case MQ_ERROR:
	  MqErrorCopy (context, parent);
	  goto error;
      }
      //MqDLogC(context,7,"finish fEvent in<%p>\n", msgque);

      // the fEvent found an event belonging to "context" but does not know
      // if the event belongs to the "parent" self or to one of his "childs".
      // -> let upper code decide what to do
      if (!pTokenCheck(context->link.srvT,"____")) {
        //pDLog(context,7,"fEvent in<%p>, token<%s>\n", msgque, pTokenGetCurrent(context->link.srvT));
        return MQ_CONTINUE;
      }
    }

    // 3. how long does it takes until now
    nowT = time (NULL) - startT;
  }
  while (nowT < timeout);

  MqDLogCL(context,6,"END-ERROR-TIMEOUT\n");
  return MqErrorDbV(MQ_ERROR_TIMEOUT, timeout);

end:
  MqDLogCL(context,6,"END-OK\n");
error:
  return MqErrorStack (context);
}

/// \attention this code should call all available event-loop functions
enum MqErrorE
pUSleep (
  struct MqS * const context,
  long const usec
)
{
  if (context != NULL && context->setup.Event.fCall != NULL) {
    struct MqS * const parent = pMqGetFirstParent (context);
    struct mq_timeval start;
    struct mq_timeval now;
    MqErrorCheck (MqSysGetTimeOfDay (context, &start, NULL));
    do {
      if (MqErrorCheckI (sCallEventProc (parent))) {
	MqErrorCopy (context, parent);
	goto error;
      }
      MqErrorCheck (MqSysUSleep (context, 99999L));
      MqErrorCheck (MqSysGetTimeOfDay (context, &now, NULL));
    }
    while (((now.tv_sec - start.tv_sec) * 1000000L + (now.tv_usec - start.tv_usec)) < usec);
  } else {
    MqErrorCheck (MqSysUSleep (context, usec));
  }

error:
  return MqErrorStack (context);
}

/*****************************************************************************/
/*                                                                           */
/*                                misc                                       */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqCheckForLeftOverArguments (
  struct MqS * const context,
  struct MqBufferLS ** argvP
)
{
  MQ_SIZE i;
  MQ_CST str;
  struct MqBufferLS * argv;

  if (unlikely(argvP == NULL || *argvP == NULL)) {
    return MQ_OK;
  }
  argv = *argvP;

  if (argv->cursize == 0) {
    MqBufferLDelete(argvP);
    return MQ_OK;
  }

  MqErrorDb2 (context,MQ_ERROR_INVALID_OPTION);
  for (i = 0; i < argv->cursize; i++) {
    if ( MqBufferGetC (argv->data[i], &str) != MQ_ERROR)
       MqErrorSAppendV (context, "=> %s", str);
  }
  MqBufferLDelete(argvP);
  return MqErrorGetCodeI(context);
}

/*****************************************************************************/
/*                                                                           */
/*                                misc                                       */
/*                                                                           */
/*****************************************************************************/

void
MqExitP (
  MQ_CST prefix,
  struct MqS * context
)
{
  if (context == NULL) {
    // exit on empty context
    SysExit (0,0);
  } else if (context->bits.onExit == MQ_YES) {
    // no double calling of MqExit
    MqPanicV(MQ_ERROR_PANIC, prefix, MqMessageNum(MQ_ERROR_EXIT),
      "called '%s' for context '%p' twice", __func__, context);
  } else {
    // do the exit
    int num;
    MqExitF exitF;
    MQ_BOL isThread;

    context->bits.onExit = MQ_YES;
    context->setup.ignoreExit = MQ_NO;
    MqDLogV(context,3,"called from '%s'\n", prefix);

    // switch to parent
    if (MQ_IS_CHILD(context)) {
      struct MqS * parent = pMqGetFirstParent(context);
      MqErrorCopy (parent, context);
      context = parent;
      context->bits.onExit = MQ_YES;
      MqDLogC(context,3,"switch to PARENT\n");
    }

    // save context data because the next section will delete it
    num = MqErrorGetNumI(context);

    // MQ_ERROR_EXIT is a "normal" exit 
    if (num == (200+MQ_ERROR_EXIT)) num = EXIT_SUCCESS;

    // if context was started as thread?
    isThread = ((context->statusIs & MQ_STATUS_IS_THREAD) != 0);

    // get the right application specific exit function
    exitF = (isThread ? context->setup.fThreadExit : context->setup.fProcessExit);

    // 1. delete the main Link and all depending links also
    MqLinkDelete (context);

    // 2. delete all other parent context from current thread or process
    EventCleanup();

    // 3. delete all collected objects
    GcDelete();

    // 4. call application specific exit function
    if (exitF) (*exitF) (num);

    // 5. finally call libmsgque exit function
    SysExit(isThread, num);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                debugging                                  */
/*                                                                           */
/*****************************************************************************/

#ifdef _DEBUG
void
MqLogData (
  struct MqS const * const context,
  MQ_CST const prefix
)
{
  MqLogV (context, prefix, 0, ">>>> MqS \n");

  MqLogV (context, prefix, 0, "debug     = " MQ_FORMAT_I "\n", context->config.debug);
  MqLogV (context, prefix, 0, "silent    = " MQ_FORMAT_C "\n", (context->config.isSilent ? "yes" : "no"));
  MqLogV (context, prefix, 0, "string    = " MQ_FORMAT_C "\n", (context->config.isString ? "yes" : "no"));
  MqLogV (context, prefix, 0, "ctxId     = " MQ_FORMAT_I "\n", context->link.ctxId);
  MqLogV (context, prefix, 0, "sOc       = " MQ_FORMAT_C "\n", MqLogServerOrClient (context));
  MqLogV (context, prefix, 0, "pOc       = " MQ_FORMAT_C "\n", MqLogParentOrChild (context));

  MqLogV (context, prefix, 0, "<<<< MqS\n");
}
#endif /* _DEBUG */

  void sGenericCreate (void);
  void sGenericDelete (void);
  void sEventCleanup (void);
  void sEventDelete (void);

END_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                DLL                                        */
/*                                                                           */
/*****************************************************************************/

#if defined(_MSC_VER)

BOOL WINAPI DllMain( 
    HINSTANCE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
# ifdef MQ_COMPILE_AS_CC
    using namespace libmsgque;
# endif
  
  switch (ul_reason_for_call)
  {
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      return FALSE;
    case DLL_PROCESS_ATTACH:
      sGcCreate();
      sEventCreate();
      sGenericCreate();
      break;
    case DLL_PROCESS_DETACH:
      sGenericDelete();
      sEventCleanup();
      sGcDelete();
      break;
  }
  return TRUE;
}
#endif





