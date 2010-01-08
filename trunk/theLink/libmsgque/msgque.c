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

void sEventFree (void);
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

void
pDeleteProtectionCreate (
  struct MqS * const context
)
{
  context->link.deleteProtection = MQ_YES;
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
  switch ((*context->setup.Event.fFunc) (context, context->setup.Event.data)) {
    case MQ_OK:
      break;
    case MQ_CONTINUE:
      if (context->link.onShutdown == MQ_YES)
	CONTINUE++;
      break;
    case MQ_ERROR:
    case MQ_EXIT:
      goto error;
  }
  // call the event-proc's of my child's
  for (child = context->link.childs; child != NULL; child=child->right) {
    NUM++;
    cldCtx = child->context;
    if (cldCtx != NULL && cldCtx->setup.Event.fFunc != NULL) {
      switch (sCallEventProc (cldCtx)) {
	case MQ_OK:
	  break;
	case MQ_CONTINUE:
	  if (cldCtx->link.onShutdown == MQ_YES)
	    CONTINUE++;
	  break;
	case MQ_ERROR:
	case MQ_EXIT:
	  MqErrorCopy(context, cldCtx);
	  goto error;
	  break;
      }
    }
  }
  // with "ignoreExit" all "context" have to be on "CONTINUE" to trigger an exit
  if (context->setup.ignoreExit == MQ_NO || NUM != CONTINUE) {
    MqErrorReset(context);
  }
  return MqErrorGetCodeI(context);
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
  if (context->setup.Event.fFunc != NULL) {
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
      case MQ_EXIT:	return MQ_EXIT;	  // something unexpected happen
    }

    // 2. call external event-proc
    if (context != NULL && context->setup.Event.fFunc != NULL) {
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
	  return pErrorSetEXIT (context, __func__);
	case MQ_ERROR:
	case MQ_EXIT:
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
  return MQ_OK;

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
  if (context != NULL && context->setup.Event.fFunc != NULL) {
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

struct MqS *
pMqGetFirstParent(
  struct MqS * const context
)
{
  return context->link.ctxIdP;
}

void
MqExit (
  struct MqS * context
)
{
  int num;
  MqExitF exitF;
  MQ_BOL isThread;

  // exit on empty context
  if (context == NULL) SysExit (0,0);
  // no double invovation of MqExit
  if (context->link.onExit == MQ_YES) MqPanicSYS(context);
  context->link.onExit = MQ_YES;
  MqDLogC(context,3,"EXIT\n");

  // switch to parent
  if (MQ_IS_CHILD(context)) {
    struct MqS * parent = pMqGetFirstParent(context);
    MqErrorCopy (parent, context);
    context = parent;
    context->link.onExit = MQ_YES;
    MqDLogC(context,3,"switch to PARENT\n");
  }

  // save context data because the next section will delete it
  num = MqErrorGetNum(context);

  // MQ_ERROR_EXIT is a "normal" exit 
  if (num == (200+MQ_ERROR_EXIT)) num = EXIT_SUCCESS;

  // if context was started as thread?
  isThread = ((context->statusIs & MQ_STATUS_IS_THREAD) != 0);

  // get the right application specific exit function
  exitF = (isThread ? context->setup.fThreadExit : context->setup.fProcessExit);

  // 1. delete the main Link and all depending links also
  MqLinkDelete (context);

  // 2. delete all other parent context from current thread or process
  sEventFree();

  // 3. call application specific exit function
  if (exitF) (*exitF) (num);

  // 4. finally call libmsgque exit function
  SysExit(isThread, num);
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
  MqDLogX (context, prefix, 0, ">>>> MqS \n");

  MqDLogX (context, prefix, 0, "debug     = " MQ_FORMAT_I "\n", context->config.debug);
  MqDLogX (context, prefix, 0, "silent    = " MQ_FORMAT_C "\n", (context->config.isSilent ? "yes" : "no"));
  MqDLogX (context, prefix, 0, "string    = " MQ_FORMAT_C "\n", (context->config.isString ? "yes" : "no"));
  MqDLogX (context, prefix, 0, "ctxId     = " MQ_FORMAT_I "\n", context->link.ctxId);
  MqDLogX (context, prefix, 0, "sOc       = " MQ_FORMAT_C "\n", MqLogServerOrClient (context));
  MqDLogX (context, prefix, 0, "pOc       = " MQ_FORMAT_C "\n", MqLogParentOrChild (context));

  MqDLogX (context, prefix, 0, "<<<< MqS\n");
}
#endif /* _DEBUG */

  void sGenericCreate (void);
  void sGenericDelete (void);
  void sEventCreate (void);
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
      sEventCreate();
      sGenericCreate();
      break;
    case DLL_PROCESS_DETACH:
      sEventDelete();
      sGenericDelete();
      break;
  }
  return TRUE;
}
#endif

