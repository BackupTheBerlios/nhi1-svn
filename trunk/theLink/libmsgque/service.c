/**
 *  \file       theLink/libmsgque/service.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"

#include <limits.h>

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                              service                                      */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
MqServiceGetFilter (
  struct MqS * const context,
  MQ_SIZE const id,
  struct MqS ** filterP
 )
{
  return (
    (
      *filterP = (
       context->config.master != NULL ?
         context->config.master :
         MqSlaveGet (context, id)
      )
    ) == NULL ?
      MqErrorDb(MQ_ERROR_NO_FILTER) :
      MQ_OK
  );
}

struct MqS * const
MqServiceGetFilter2 (
  struct MqS * const context,
  MQ_SIZE const id
 )
{
  struct MqS * const ret = context->config.master != NULL ?  context->config.master : MqSlaveGet (context, id);
  if (ret == NULL) MqErrorDb(MQ_ERROR_NO_FILTER);
  return ret;
}

int
MqServiceIsTransaction (
  struct MqS const * const context
)
{
  return context->link._trans != 0;
}

MQ_CST 
MqServiceGetToken (
  struct MqS const * const context
)
{
  return (context->link.srvT != NULL? pTokenGetCurrent(context->link.srvT) : NULL);
}

MQ_BOL
MqServiceCheckToken(
  struct MqS const * const context,
  MQ_TOK const token
)
{
  return pTokenCheck(context->link.srvT,token);
}

static enum MqErrorE 
sServiceProxy (
  struct MqS * const context,
  MQ_PTR const data
) {
  struct MqS * ftrctx;
  MqErrorCheck (MqServiceGetFilter (context, (MQ_SIZE) (long) data, &ftrctx));
  MqErrorCheck (MqReadForward      (context, ftrctx));
error:
  return MqSendRETURN(context);
}

enum MqErrorE 
MqServiceProxy(
  struct MqS * const context, 
  MQ_TOK const token,
  MQ_SIZE const id
)
{
  return MqServiceCreate (context, token, sServiceProxy, (void*)(long)id,  NULL);
}

static enum MqErrorE 
sServiceStorage (
  struct MqS * const context,
  MQ_PTR const data
) {
  MqErrorCheck (MqStorageInsert (context, NULL));
error:
  return MqSendRETURN(context);
}

enum MqErrorE 
MqServiceStorage(
  struct MqS * const context, 
  MQ_TOK const token
)
{
  return MqServiceCreate (context, token, sServiceStorage, NULL,  NULL);
}

enum MqErrorE 
MqServiceCreate(
  struct MqS * const context, 
  MQ_TOK const token,
  MqTokenF const proc,
  MQ_PTR data,
  MqDataFreeF datafreeF
)
{
  struct MqCallbackS cb = {proc, data, datafreeF, NULL};
  if (context->link.srvT == NULL) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  }
  if (strlen(token) != HDR_TOK_LEN) {
    return MqErrorDbV (MQ_ERROR_TOKEN_LENGTH, token);
  }
  return (pTokenAddHdl (context->link.srvT, token, cb));
}

enum MqErrorE 
MqServiceDelete(
  struct MqS * const context, 
  MQ_TOK const token
)
{
  if (context->link.srvT == NULL) {
    return MqErrorDbV(MQ_ERROR_CONNECTED, "msgque", "not");
  }
  return (pTokenDelHdl (context->link.srvT, token));
}

enum MqErrorE
pServiceStart (
  struct MqS * const context,
  EventReadF const reader,
  MQ_PTR env
)
{
  struct MqS * a_context = context;
  const int MqSetDebugLevel(context);

  MqDLogCL(context,6,"START\n");

  // ##################### READ HEADER #####################
  switch ((*reader) (env, &a_context)) {
    case MQ_OK:	      break;
    case MQ_CONTINUE: return MqErrorReset(context);
    case MQ_ERROR:    goto error;
  }

  // ##################### TOKEN Handler #####################
  // The following code will "only" run on the "server" site.
  // An error in a service-handler will !not! shutdown the server
  switch (pTokenInvoke (a_context->link.srvT)) {
    case MQ_OK:       
      break;
    case MQ_ERROR:
      // on EXIT do return nothing and just report EXIT to the TOPLEVEL
      if (MqErrorIsEXIT(a_context)) goto error;
      // on a client the error will be reported to the toplevel
      if (MQ_IS_CLIENT(a_context) && a_context->link._trans == 0) {
	goto error;
      } else {
	// on a server the error will be reported to the client
	// in a transaction, "MqSendRETURN" will convert the context error 
	// into an "error" package and send this package back to the client
	if (MqErrorCheckI (MqSendRETURN (a_context))) {
	  if (pIoCheck (a_context->link.io)) {
	    // outsite of a transaction we have to send a "real" error,
	    // but "only" if the connection "pIoCheck" is still available
	    MqErrorCheck (MqSendERROR (a_context));
	  } else {
	    // report the error to the top-level
	    goto error;
	  }
	}
      }
      break;
    case MQ_CONTINUE: 
      MqPanicSYS(context);
  }

  MqDLogCL(a_context,6,"END-OK\n");
  return MQ_OK;

error:
  // the errors are reported into a_context->link.error
  MqErrorStack (a_context);
  return MqErrorCopy (context, a_context);
}

MQ_TIME_T pGetTimeout (
  struct MqS *const context,
  MQ_TIME_T timeout,
  enum MqWaitOnEventE wait
)
{
  if (timeout >= 0) {
    return timeout;
  } else if (timeout == MQ_TIMEOUT_USER) {
    return pIoGetTimeout(context->link.io);
  } else if (timeout == MQ_TIMEOUT_MAX) {
    return LONG_MAX;
  } else {
    return wait == MQ_WAIT_ONCE ? pIoGetTimeout(context->link.io) : LONG_MAX;
  }
}

enum MqErrorE
MqProcessEvent (
  struct MqS * const context,
  MQ_TIME_T timeout,
  enum MqWaitOnEventE const wait
)
{
  enum MqErrorE ret = MQ_OK;
  const int forever = (wait >= MQ_WAIT_FOREVER);
  const int once = (wait >= MQ_WAIT_ONCE);
  struct mq_timeval tv = {0L, 0L};
  int debugLevel;

  // save master transaction
  MQ_HDL master_trans = (context->config.master != NULL ? context->config.master->link._trans : 0);
  MQ_HDL trans = context->link._trans;
  enum MqHandShakeE hs = pReadGetHandShake(context);

  // protection code
  MqSetDebugLevel(context);

  // first announce that the event-handling is active
  if (forever) {
    MqDLogC(context,4,"send token<_PEO>\n");
    MqSendSTART (context);
    MqErrorCheck (MqSendEND (context, "_PEO"));
  }

  // set the default for timeout
  timeout = pGetTimeout (context, timeout, wait);

  // check for an event
  MqDLogCL(context,6,"START\n");
  context->refCount++;
  MqErrorReset(context);
  do {
    // ################ CHECK TO BE READABLE ##################
    if (once) {
      switch (ret = pWaitOnEvent (context, MQ_SELECT_RECV, timeout)) {
        case MQ_OK:	  break;
        case MQ_CONTINUE: continue;
        case MQ_ERROR:	  goto error;
      }
    }

    // ##################### Process Events #####################
    ret = pIoSelectStart(context, &tv);
    MqErrorCheck (ret);

    // clean up delete objects
    if (forever) GcRun (context);
  }
  while (forever);

  // restore master transaction
  MqDLogVL(context,6,"END-%s\n", MqLogErrorCode(MqErrorGetCode(context)));

end:
  // restore the "master" transaction
  context->refCount--;
  if (context->config.master != NULL) context->config.master->link._trans = master_trans;
  context->link._trans = trans;
  pReadSetHandShake (context, hs);
  return ret;

error:
  // on "error" add stack-trace
  ret = MqErrorStack (context);
  goto end;
}

END_C_DECLS


