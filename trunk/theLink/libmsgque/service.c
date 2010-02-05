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
#include "error.h"
#include "token.h"
#include "read.h"
#include "mq_io.h"
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
  struct MqS ** const filterP
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

MQ_BOL
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
  MQ_BIN bdy; MQ_SIZE len;
  struct MqS * ftrctx;
  MqErrorCheck (MqServiceGetFilter (context, (int) (long) data, &ftrctx));

  MqErrorCheck  (MqReadBDY (context, &bdy, &len));
  MqErrorCheck1 (MqSendSTART (ftrctx));
  MqErrorCheck1 (MqSendBDY (ftrctx, bdy, len));

  // continue with the original transaction
  if (MqServiceIsTransaction (context)) {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND_AND_WAIT (ftrctx, MqServiceGetToken(context), MQ_TIMEOUT_USER));
    // read the answer
    MqSendSTART (context);
    MqErrorCheck1 (MqReadBDY (ftrctx, &bdy, &len));
    MqErrorCheck  (MqSendBDY (context, bdy, len));
  } else {
    // use a transaction protection
    MqErrorCheck1 (MqSendEND (ftrctx, MqServiceGetToken(context)));
  }

error:
  return MqSendRETURN(context);

error1:
  MqErrorCopy (context, ftrctx);
  goto error;
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

enum MqErrorE 
MqServiceCreate(
  struct MqS * const context, 
  MQ_TOK const token,
  MqTokenF const proc,
  MQ_PTR data,
  MqTokenDataFreeF datafreeF
)
{
//MqDLogX(context,__func__,0,"data<%p>\n", data);
  struct MqCallbackS cb = {proc, data, datafreeF, NULL};
  if (strlen(token) != HDR_TOK_LEN) {
    return MqErrorDbV (MQ_ERROR_TOKEN_LENGTH, token);
  }
  return (pTokenAddHdl (context->link.srvT, token, cb));
}

enum MqErrorE 
MqServiceDelete(
  struct MqS const * const context, 
  MQ_TOK const token
)
{
  return (pTokenDelHdl (context->link.srvT, token));
}

static enum MqErrorE
sMqEventStart (
  struct MqS * const context
)
{
  struct MqS * a_context = context;
  const int MqSetDebugLevel(context);

  MqDLogCL(context,6,"START\n");

  // ##################### READ HEADER #####################
  switch (pReadHDR (context, &a_context)) {
    case MQ_OK:	      break;
    case MQ_CONTINUE: return MQ_OK;
    case MQ_ERROR:    MqDLogCL(context,7,"pReadHDR: report an error\n"); goto error;
    case MQ_EXIT:     return MQ_EXIT;
  }

  // ##################### TOKEN Handler #####################
  // The following code will "only" run on the "server" site.
  // An error in a service-handler will !not! shutdown the server
  if (MqErrorCheckI(pTokenInvoke (a_context->link.srvT))) {
    enum MqErrorE err = MqErrorGetCodeI(a_context);
    if (err == MQ_ERROR) {
      if (a_context->link._trans != 0) {
	// in a transaction, "MqSendRETURN" will convert the context error 
	// into an "error" package and send this package back to the client
	MqErrorCheck (MqSendRETURN (a_context));
      } else if (pIoCheck (a_context->link.io)) {
	// outsite of a transaction we have to send a "real" error,
	// but "only" if the connection "pIoCheck" is still available
	MqErrorCheck (MqSendERROR (a_context));
      } else {
	// report thr error to the top-level
	goto error;
      }
    } else {
      // MQ_EXIT, MQ_CONTINUE, ...
      goto error;
    }
  }

  MqDLogCL(a_context,6,"END-OK\n");
  return MQ_OK;

error:
  // the errors are reported into a_context->link.error
  MqErrorStack (a_context);
  return MqErrorCopy (context, a_context);
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
  struct MqS * const master = context->config.master;

  // save master transaction
  MQ_HDL trans = (master != NULL ? master->link._trans : 0);

  // protection code
  MqSetDebugLevel(context);

  // first announce that the event-handling is active
  if (forever) {
    MqDLogC(context,4,"send token<_PEO>\n");
    MqSendSTART (context);
    MqErrorCheck (MqSendEND (context, "_PEO"));
  }

  // set the default for timeout
  if (timeout == MQ_TIMEOUT_USER) {
    timeout = forever ? LONG_MAX : pIoGetTimeout(context->link.io);
  } else if (timeout < 0) {
    timeout = MQ_TIMEOUT;
  }

  // check for an event
  MqDLogCL(context,6,"START\n");
  do {
    // ################ CHECK TO BE READABLE ##################
    if (once) {
      ret = pWaitOnEvent (context, MQ_SELECT_RECV, timeout);
      switch (ret) {
        case MQ_OK:	  break;
        case MQ_CONTINUE: ret = MQ_OK; continue;
        case MQ_ERROR:	  goto error;
        case MQ_EXIT:	  {
	  // we check for toplevel "server-forever-loop"
	  if (forever) {
	    // on "toplevel"
	  } else {
	    // not on "toplevel"
	  }
	  goto error;
	}
      }
    }

    // ##################### Process Events #####################
    ret=pMqSelectStart(context, &tv, sMqEventStart);
    switch (ret) {
      case MQ_OK:	  break;
      case MQ_CONTINUE:	  ret = MQ_OK; continue;
      case MQ_ERROR:	  goto error;
      case MQ_EXIT:	  {
	goto error;
      }
    }
  }
  while (forever && ret == MQ_OK);

end:
  // restore master transaction
  if (master != NULL) master->link._trans = trans;
  MqDLogVL(context,6,"END-%s\n", MqLogErrorCode(ret));
  return ret;

error:
  // restore master transaction
//I0
//printP(master)
  if (context->config.master != NULL) context->config.master->link._trans = trans;
  return MqErrorStack (context);
}

END_C_DECLS




