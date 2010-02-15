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
  FILE		    *FH;
};

/*****************************************************************************/
/*                                                                           */
/*                               Request Handler                             */
/*                                                                           */
/*****************************************************************************/

static void ErrorWrite (
  struct MqS *mqctx
)
{
  SETUP_ctx;
  fprintf(ctx->FH, "ERROR: %s\n", MqErrorGetText(mqctx));
  fflush(ctx->FH);
  MqErrorReset (mqctx);
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
    MqErrorCheck1 (MqLinkConnect (ftr));

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
	if (MqErrorIsEXIT(ftr)) {
	  return MqErrorDeleteEXIT(ftr);
	} else {
	  ErrorWrite (ftr);
	}
	break;
    }
    // reset the item-storage
    MqBufferReset(itm->data);
    ctx->rIdx++;
    return MQ_OK;
error1:
    ErrorWrite (ftr);
    return MQ_OK;
  }
error:
  return MqErrorStack(mqctx);
}

static enum MqErrorE LOGF ( ARGS ) {
  MQ_CST file;
  struct MqS * ftr;
  struct FilterCtxS *ftrctx;
  MqErrorCheck (MqServiceGetFilter(mqctx, 0, &ftr));
  ftrctx = (struct FilterCtxS*const)ftr;
  MqErrorCheck (MqReadC (mqctx, &file));
  if (!strcmp(MqLinkGetTargetIdent (ftr),"transFilter")) {
    MqErrorCheck (MqSendSTART(ftr));
    MqErrorCheck (MqSendC(ftr, file));
    MqErrorCheck (MqSendEND_AND_WAIT(ftr, "LOGF", MQ_TIMEOUT_USER));
  } else {
    ftrctx->FH = fopen (file, "a");
  }
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE WRIT ( ARGS ) {
  MQ_CST str;
  SETUP_ctx;
  MqErrorCheck (MqReadC (mqctx, &str));
  fprintf(ctx->FH, "%s\n", str);
  fflush(ctx->FH);
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE EXIT ( ARGS ) {
  MqExit(mqctx);
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
  struct MqS * ftr;
  struct FilterCtxS *ftrctx;

  for (i=0;i<ctx->size;i++) {
    if (ctx->itm[i] != NULL) {
      MqBufferDelete (&ctx->itm[i]->data);
      MqSysFree(ctx->itm[i]);
    }
  }
  MqSysFree (ctx->itm);

  MqErrorCheck(MqServiceGetFilter(mqctx, 0, &ftr));
  ftrctx = (struct FilterCtxS*const)ftr;
  if (ftrctx->FH != NULL) fclose(ftrctx->FH);

error:
  return MqErrorStack(mqctx);
}

static enum MqErrorE
FilterSetup (
  struct MqS * const mqctx,
  MQ_PTR data
)
{
  register SETUP_ctx;
  struct MqS *ftr;
  MqErrorCheck (MqServiceGetFilter (mqctx, 0, &ftr));

  // init the cache
  ctx->itm = (struct FilterItmS**)MqSysCalloc(MQ_ERROR_PANIC,100,sizeof(struct FilterItmS*));
  ctx->rIdx = 0;
  ctx->wIdx = 0;
  ctx->size = 100;

  // SERVER: listen on every token (+ALL)
  MqErrorCheck (MqServiceCreate (mqctx, "LOGF", LOGF, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "EXIT", EXIT, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "+ALL", FilterIn, NULL, NULL));
  MqErrorCheck (MqServiceCreate (ftr,   "WRIT", WRIT, NULL, NULL));

  ctx->FH = NULL;

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
  mqctx->setup.ServerSetup.fCall    = FilterSetup;
  mqctx->setup.ServerCleanup.fCall  = FilterCleanup;
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

