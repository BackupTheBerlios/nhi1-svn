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

#include "msgque.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

#define ARGS  struct MqS * const mqctx, void *data
#define TRANSCTX ((struct FilterCtxS*const)mqctx)
#define MQ_CONTEXT_S mqctx
#define SETUP_ctx struct FilterCtxS*const ctx = TRANSCTX
#define CHECK_ARGS(s) \
  if (MqReadGetNumItems(mqctx))  { \
    return MqErrorV (mqctx, __func__, -1, "usage: %s (%s)\n", __func__, s); \
  }

struct FilterCtxS {
  struct MqS	    mqctx;
  struct MqDumpS    **itm;
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
    struct MqS * ftr;
    MqErrorCheck (MqServiceGetFilter(mqctx, 0, &ftr));

    // if connection is down -> connect again
    MqErrorCheck1 (MqLinkConnect (ftr));

    // fill the read-buffer from storage
    MqErrorCheck2 (MqReadLOAD (mqctx, ctx->itm[ctx->rIdx]));

    // send BDY data to the link-target, on error write message but do not stop processing
    if (MqErrorCheckI (MqReadForward(mqctx, ftr))) {
      if (MqErrorIsEXIT(mqctx)) {
	return MqErrorReset(mqctx);
      } else {
	ErrorWrite (mqctx);
      }
      goto end;
    }
    // reset the item-storage
end:
    MqDumpDelete(&ctx->itm[ctx->rIdx]);
    ctx->rIdx++;
    return MQ_OK;
error1:
    MqErrorCopy(mqctx,ftr);
error2:
    ErrorWrite (mqctx);
    return MQ_OK;
  }
error:
  return MqErrorStack(mqctx);
}

static enum MqErrorE LOGF ( ARGS ) {
  SETUP_ctx;
  struct MqS * ftr;
  MqErrorCheck(MqServiceGetFilter(mqctx, 0, &ftr));
  if (!strcmp(MqLinkGetTargetIdent (ftr),"transFilter")) {
    MqErrorCheck (MqReadForward(mqctx, ftr));
  } else {
    MQ_CST file;
    MqErrorCheck (MqReadC (mqctx, &file));
    ctx->FH = fopen (file, "a");
  }
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE WRIT ( ARGS ) {
  MQ_CST str;
  struct FilterCtxS *ctx;
  MqErrorCheck(MqServiceGetFilter(mqctx, 0, (struct MqS**) &ctx));
  MqErrorCheck (MqReadC (mqctx, &str));
  fprintf(ctx->FH, "%s\n", str);
  fflush(ctx->FH);
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE EXIT ( ARGS ) {
  exit(1);
}

static enum MqErrorE FilterIn ( ARGS ) {
  register struct FilterCtxS*const ctx = TRANSCTX;
  
  // add space if space is empty
  if (ctx->wIdx==ctx->size-1) {
    MQ_SIZE i;
    ctx->size*=2;
    MqSysRealloc(MQ_ERROR_PANIC, ctx->itm, sizeof(struct MqDumpS*)*ctx->size);
    for (i=ctx->wIdx;i<ctx->size;i++) {
      ctx->itm[i] = NULL;
    }
  }

  // save data to storage
  MqErrorCheck(MqReadDUMP(mqctx, &ctx->itm[ctx->wIdx]));

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
      MqDumpDelete(&ctx->itm[i]);
    }
  }
  MqSysFree (ctx->itm);

  if (ctx->FH != NULL) fclose(ctx->FH);

  return MQ_OK;
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
  if (MQ_IS_SERVER(mqctx)) {
    ctx->itm = (struct MqDumpS**)MqSysCalloc(MQ_ERROR_PANIC,100,sizeof(struct MqDumpS*));
    ctx->rIdx = 0;
    ctx->wIdx = 0;
    ctx->size = 100;
    ctx->FH = NULL;
  }

  // SERVER: listen on every token (+ALL)
  MqErrorCheck (MqServiceCreate (mqctx, "LOGF", LOGF, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "EXIT", EXIT, NULL, NULL));
  MqErrorCheck (MqServiceCreate (mqctx, "+ALL", FilterIn, NULL, NULL));
  MqErrorCheck (MqServiceCreate (ftr,   "WRIT", WRIT, NULL, NULL));
  MqErrorCheck (MqServiceProxy  (ftr,   "+TRT", 0));


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
  // setup default Factory
  MqFactoryDefault (MQ_ERROR_PANIC, "transFilter", MqFactoryDefaultCreate, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

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

  MqConfigSetEvent (mqctx, FilterEvent, NULL, NULL, NULL);

  // create the link
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT_USER, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}

