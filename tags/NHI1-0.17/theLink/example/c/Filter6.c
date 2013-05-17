/**
 *  \file       theLink/example/c/Filter6.c
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
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
  fseek(ctx->FH, 0L, SEEK_END);
  fprintf(ctx->FH, "ERROR: %s\n", MqErrorGetText(mqctx));
  fflush(ctx->FH);
  MqErrorReset (mqctx);
}

static enum MqErrorE FilterEvent (
  struct MqS * const mqctx,
  MQ_PTR const data
)
{
  MQ_TRA count;
  MqErrorCheck (MqStorageCount (mqctx, &count));
  // check if an item is available
  if (count == 0) {
    // no data available
    return MqErrorSetCONTINUE(mqctx);
  } else {
    MQ_TRA Id = 0LL;
    struct MqS * ftr;
    MqErrorCheck (MqServiceGetFilter(mqctx, 0, &ftr));

    // if connection is down -> connect again
    MqErrorCheck1 (MqLinkConnect (ftr));

    // fill the read-buffer from storage
    MqErrorCheck2 (MqStorageSelect (mqctx, &Id));

    // send BDY data to the link-target, on error write message but do not stop processing
    if (MqErrorCheckI (MqReadForward(mqctx, ftr, NULL))) {
      if (MqErrorIsEXIT(mqctx)) {
	return MqErrorReset(mqctx);
      } else {
	goto error2;
      }
    }
    MqStorageDelete (mqctx, Id);
    return MQ_OK;
error1:
    MqErrorCopy(mqctx,ftr);
error2:
    ErrorWrite (mqctx);
    if (Id != 0LL) MqStorageDelete (mqctx, Id);
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
    MqErrorCheck (MqReadForward(mqctx, ftr, NULL));
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
  MqErrorCheck (MqServiceGetFilter (mqctx, 0, (struct MqS**) &ctx));
  MqErrorCheck (MqReadC (mqctx, &str));
  fseek(ctx->FH, 0L, SEEK_END);
  fprintf(ctx->FH, "%s\n", str);
  fflush(ctx->FH);
error:
  return MqSendRETURN(mqctx);
}

static enum MqErrorE EXIT ( ARGS ) {
  MqExit(mqctx);
  return MQ_OK;
  //exit(1);
}

static enum MqErrorE FilterIn ( ARGS ) {
  MqErrorCheck (MqStorageInsert (mqctx, NULL));
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
  SETUP_ctx;
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

  MqErrorCheck (MqServiceCreate	  (mqctx, "LOGF", LOGF, NULL, NULL));
  MqErrorCheck (MqServiceCreate	  (mqctx, "EXIT", EXIT, NULL, NULL));
  MqErrorCheck (MqServiceStorage  (mqctx, "PRNT"));
  MqErrorCheck (MqServiceCreate	  (mqctx, "+ALL", FilterIn, NULL, NULL));
  MqErrorCheck (MqServiceCreate	  (ftr,   "WRIT", WRIT, NULL, NULL));
  MqErrorCheck (MqServiceProxy	  (ftr,   "+TRT", 0));

  ctx->FH = NULL;

error:
  return MqErrorStack(mqctx);
}

enum MqErrorE
Filter6Factory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS * const item,
  struct MqS **contextP
)
{ 
  struct MqS * const mqctx = *contextP = MqContextCreate(sizeof(struct FilterCtxS),tmpl);
  
  mqctx->setup.Child.fCreate	    = MqLinkDefault;
  mqctx->setup.Parent.fCreate	    = MqLinkDefault;
  mqctx->setup.isServer		    = MQ_YES;
  mqctx->setup.ServerSetup.fCall    = FilterSetup;
  mqctx->setup.ServerCleanup.fCall  = FilterCleanup;
  mqctx->setup.ignoreExit	    = MQ_YES;

  MqConfigSetEvent (mqctx, FilterEvent, NULL, NULL, NULL);
  return MQ_OK;
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
  // parse the command-line
  MQ_BFL args = MqBufferLCreateArgs (argc, argv);

  // call Factory 
  struct MqS *mqctx = MqFactoryNew (MQ_ERROR_PANIC, NULL,
    MqFactoryAdd(MQ_ERROR_PANIC, "transFilter", Filter6Factory, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
  );

  // create the link
  MqErrorCheck(MqLinkCreate (mqctx, &args));

  // start event-loop and wait forever
  MqProcessEvent (mqctx, MQ_TIMEOUT_USER, MQ_WAIT_FOREVER);

  // finish and exit
error:
  MqExit (mqctx);
}
