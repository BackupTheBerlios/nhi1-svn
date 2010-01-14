/**
 *  \file       theLink/example/c/Filter4.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <stdlib.h>
#include <string.h>

#include "msgque.h"
#include "debug.h"

#define ARGS  struct MqS * const mqctx, void *data
#define TRANSCTX ((struct FilterCtxS*const)mqctx)
#define MQ_CONTEXT_S mqctx
#define SETUP_ctx struct FilterCtxS*const ctx = TRANSCTX
#define CHECK_ARGS(s) \
  if (MqReadGetNumItems(mqctx))  { \
    return MqErrorV (mqctx, __func__, -1, "usage: %s (%s)\n", __func__, s); \
  }

struct FilterItmS {
  MQ_STRB token[5];
  MQ_BOL  isFilter;
  MQ_BUF  data;
};

struct FilterCtxS {
  struct MqS	    mqctx;	///< \mqctxI
  struct FilterItmS  **itm;
  MQ_INT	    rIdx;
  MQ_INT	    wIdx;
  MQ_INT	    size;
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE FilterEvent (
  struct MqS * const mqctx,
  MQ_PTR const data
)
{
  register SETUP_ctx;

  // check if an item is available
  if (ctx->rIdx == ctx->wIdx) {
    // no transaction available
    return MqErrorSetCONTINUE(mqctx);
  } else {
    struct MqS * ftr;
    register struct FilterItmS * itm;
    // is an item available?
    switch (MqServiceGetFilter(mqctx, 0, &ftr)) {
      case MQ_OK:
	break;
      case MQ_ERROR:
	// ignore the error
	MqErrorReset(mqctx);
	goto end;
      case MQ_EXIT:
      case MQ_CONTINUE:
	goto error;
    }

    // extract the first (oldest) item from the store
    itm = ctx->itm[ctx->rIdx++];

    // send the ctxaction to the ctx, on error write message but do not stop processing
    MqErrorCheck1 (MqSendSTART(ftr));
    MqErrorCheck1 (MqSendBDY(ftr, itm->data->cur.B, itm->data->cursize));
    MqErrorCheck1 (
      itm->isFilter ?
	MqSendEND_AND_WAIT(ftr, itm->token, MQ_TIMEOUT_USER) :
	  MqSendEND(ftr, itm->token)
    );

    // reset the item-storage
    MqBufferReset(itm->data);
error:
    return MqErrorStack(mqctx);
error1:
    MqErrorPrint(ftr);
    MqErrorReset(ftr);
    MqErrorReset(mqctx);
    return MQ_OK;
  }
end:
  return MQ_OK;
}

static enum MqErrorE FilterIn ( ARGS ) {
  MQ_BIN bdy;
  MQ_SIZE len;
  SETUP_ctx;
  register struct FilterItmS * it;
  MqErrorCheck(MqReadBDY(mqctx, &bdy, &len));
  
  // add space if space is empty
  if (ctx->rIdx-1==ctx->wIdx || (ctx->rIdx==0 && ctx->wIdx==ctx->size-1)) {
    MQ_SIZE i;
    MqSysRealloc(MQ_ERROR_PANIC, ctx->itm, sizeof(struct FilterItmS*)*ctx->size*2);
    for (i=0;i<ctx->rIdx;i++) {
      ctx->itm[ctx->wIdx++] = ctx->itm[i];
      ctx->itm[i] = NULL;
    }
    for (i=ctx->wIdx;i<ctx->size;i++) {
      ctx->itm[i] = NULL;
    }
    ctx->size*=2;
  }

  it = ctx->itm[ctx->wIdx];

  // create storage if NULL
  if (it == NULL) {
    ctx->itm[ctx->wIdx] = it = MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof(struct FilterItmS));
  }
  if (it->data == NULL) {
    it->data = MqBufferCreate(MQ_ERROR_PANIC, len);
  }
  MqBufferSetB(it->data, bdy, len);
  strncpy(it->token, MqServiceGetToken(mqctx), 5);
  it->isFilter = MqServiceIsTransaction(mqctx);
  ctx->wIdx++;
error:
  return MqSendRETURN(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                context_init                               */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
FilterCleanup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  MQ_SIZE i;
  SETUP_ctx;

  for (i=0;i<ctx->size;i++) {
    if (ctx->itm[i] != NULL) {
      MqBufferDelete (&ctx->itm[i]->data);
      MqSysFree(ctx->itm[i]);
    }
  }
  return MQ_OK;
}

static enum MqErrorE
FilterSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  register SETUP_ctx;

  // init the chache
  ctx->itm = (struct FilterItmS**)MqSysCalloc(MQ_ERROR_PANIC,100,sizeof(struct FilterItmS*));
  ctx->rIdx = 0;
  ctx->wIdx = 0;
  ctx->size = 100;

  // SERVER: listen on every token (+ALL)
  MqErrorCheck (MqServiceCreate (mqctx, "+ALL", FilterIn, NULL, NULL));

error:
  return MqErrorStack(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                  main                                     */
/*                                                                           */
/*****************************************************************************/

int
main (
  const int argc,
  MQ_CST argv[]
)
{
  // the parent-context
  struct MqS * const mqctx = MqContextCreate(sizeof(struct FilterCtxS), NULL);

  // parse the command-line
  MQ_BFL args = MqBufferLCreateArgs (argc, argv);

  // add config data
  mqctx->setup.Child.fCreate	    = MqLinkDefault;
  mqctx->setup.Parent.fCreate	    = MqLinkDefault;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fFunc    = FilterSetup;
  mqctx->setup.ServerCleanup.fFunc  = FilterCleanup;
  mqctx->setup.ignoreExit	    = MQ_YES;

  MqConfigSetDefaultFactory (mqctx);
  MqConfigSetEvent (mqctx, FilterEvent, NULL, NULL, NULL);

  // create the ServerCtxS
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}


