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
  MQ_BOL  isTransaction;
  MQ_BUF  data;
};

struct FilterCtxS {
  struct MqS	    mqctx;
  struct FilterItmS **itm;
  MQ_INT	    rIdx;
  MQ_INT	    wIdx;
  MQ_INT	    size;
  MQ_CST	    file;
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

static void ErrorWrite (
  struct MqS *mqctx,
  MQ_CST file
)
{
  if (file != NULL) {
    FILE *FH=fopen (file, "a");
    fprintf(FH, "ERROR: %s\n", MqErrorGetText(mqctx));
    fclose (FH);
    MqErrorReset (mqctx);
  } else {
    MqErrorPrint (mqctx);
  }
}

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
    register struct FilterItmS * itm;
    struct MqS * ftr;
    MqErrorCheck (MqServiceGetFilter(mqctx, 0, &ftr));

    // if connection is down -> connect again
    switch (MqLinkConnect (ftr)) {
      case MQ_OK:	  break;
      case MQ_CONTINUE:	  return MQ_OK;
      case MQ_EXIT:	  goto error1;
      case MQ_ERROR:	  goto error1;
    }

    // extract the first (oldest) item from the store
    itm = ctx->itm[ctx->rIdx];

    // send the data to the "filter", on error write message but do not stop processing
    MqErrorCheck1 (MqSendSTART(ftr));
    MqErrorCheck1 (MqSendBDY(ftr, itm->data->cur.B, itm->data->cursize));
    switch ( itm->isTransaction ?
      MqSendEND_AND_WAIT(ftr, itm->token, MQ_TIMEOUT_USER) :
	MqSendEND(ftr, itm->token)
    ) {
      case MQ_OK:	  
	break;
      case MQ_CONTINUE:	  
	return MQ_OK;
      case MQ_ERROR:
	ErrorWrite (mqctx, ctx->file);
	goto end;
      case MQ_EXIT:
	MqErrorReset(ftr);
	return MQ_OK;
    }
    // reset the item-storage
end:
    MqBufferReset(itm->data);
    ctx->rIdx++;
    return MQ_OK;
error1:
    ErrorWrite (ftr, ctx->file);
    return MQ_OK;
  }
error:
  return MqErrorStack(mqctx);
}

static enum MqErrorE SetLogFile ( ARGS ) {
  SETUP_ctx;
  MQ_CST f;
  MqErrorCheck (MqReadC (mqctx, &f));
  ctx->file = mq_strdup_save(f);
  if (!strcmp(MqLinkGetTargetIdent (mqctx),"transFilter")) {
    struct MqS * ftr;
    MqErrorCheck (MqServiceGetFilter(mqctx, 0, &ftr));
    MqErrorCheck (MqSendSTART(ftr));
    MqErrorCheck (MqSendC(ftr, f));
    MqErrorCheck (MqSendEND_AND_WAIT(ftr, "LOGF", MQ_TIMEOUT_USER));
  }
error:
  return MqSendRETURN(mqctx);
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
  it->isTransaction = MqServiceIsTransaction(mqctx);
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
  MqSysFree (ctx->itm);
  MqSysFree (ctx->file);
  return MQ_OK;
}

static enum MqErrorE
FilterSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  register SETUP_ctx;

  // init the cache
  ctx->itm = (struct FilterItmS**)MqSysCalloc(MQ_ERROR_PANIC,100,sizeof(struct FilterItmS*));
  ctx->rIdx = 0;
  ctx->wIdx = 0;
  ctx->size = 100;

  // SERVER: listen on every token (+ALL)
  MqErrorCheck (MqServiceCreate (mqctx, "LOGF", SetLogFile, NULL, NULL));
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
  MqConfigSetIdent (mqctx, "transFilter");

  // create the link
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT_USER, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}

