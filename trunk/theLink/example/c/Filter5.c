/**
 *  \file       theLink/example/c/Filter5.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque.h"
#include "debug.h"
#include "string.h"

// F1 **************************************************************************

static enum MqErrorE F1( struct MqS *ctx, MQ_PTR data) {
  struct MqS * ftr;
  MQ_CST str;
  MqErrorCheck (MqServiceGetFilter(ctx, 0, &ftr));
  MqSendSTART(ftr);
  MqErrorCheck (MqSendC(ftr, "F1"));
  MqErrorCheck (MqSendC(ftr, ftr->config.name));
  MqErrorCheck (MqSendI(ftr, ftr->config.startAs));
  MqErrorCheck (MqSendI(ftr, ctx->statusIs));
  while (MqReadItemExists(ctx)) {
    MqErrorCheck (MqReadC(ctx, &str));
    MqErrorCheck (MqSendC(ftr, str));
  }
  MqErrorCheck (MqSendEND_AND_WAIT (ftr, "+FTR", MQ_TIMEOUT_USER));
error:
  return MqSendRETURN (ctx);
}

enum MqErrorE
F1Setup (
  struct MqS * const ctx,
  MQ_PTR const data
) {
  MqErrorCheck (MqServiceCreate (ctx, "+FTR", F1, NULL, NULL));
  MqErrorCheck (MqServiceProxy (ctx, "+EOF", 0));
error:
  return MqErrorStack(ctx);
}

enum MqErrorE
F1New (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS* const item,
  struct MqS  ** contextP
) {
  struct MqS * ctx = MqContextCreate(0, tmpl);
  MqConfigSetSetup(ctx, MqLinkDefault, NULL, MqLinkDefault, NULL, NULL, NULL);
  MqConfigSetServerSetup(ctx, F1Setup, NULL, NULL, NULL);
  *contextP = ctx;
  return MQ_OK;
}

// F2 **************************************************************************

static enum MqErrorE F2( struct MqS *ctx, MQ_PTR data) {
  struct MqS * ftr;
  MQ_CST str;
  MqErrorCheck (MqServiceGetFilter(ctx, 0, &ftr));
  MqSendSTART(ftr);
  MqErrorCheck (MqSendC(ftr, "F2"));
  MqErrorCheck (MqSendC(ftr, ftr->config.name));
  MqErrorCheck (MqSendI(ftr, ftr->config.startAs));
  MqErrorCheck (MqSendI(ftr, ctx->statusIs));
  while (MqReadItemExists(ctx)) {
    MqErrorCheck (MqReadC(ctx, &str));
    MqErrorCheck (MqSendC(ftr, str));
  }
  MqErrorCheck (MqSendEND_AND_WAIT (ftr, "+FTR", MQ_TIMEOUT_USER));
error:
  return MqSendRETURN (ctx);
}

enum MqErrorE
F2Setup (
  struct MqS * const ctx,
  MQ_PTR const data
) {
  MqErrorCheck (MqServiceCreate (ctx, "+FTR", F2, NULL, NULL));
  MqErrorCheck (MqServiceProxy (ctx, "+EOF", 0));
error:
  return MqErrorStack(ctx);
}

enum MqErrorE
F2New (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS* const item,
  struct MqS  ** contextP
) {
  struct MqS * ctx = MqContextCreate(0, tmpl);
  MqConfigSetSetup(ctx, MqLinkDefault, NULL, MqLinkDefault, NULL, NULL, NULL);
  MqConfigSetServerSetup(ctx, F2Setup, NULL, NULL, NULL);
  *contextP = ctx;
  return MQ_OK;
}

// F3 **************************************************************************

static enum MqErrorE F3( struct MqS *ctx, MQ_PTR data) {
  struct MqS * ftr;
  MQ_CST str;
  MqErrorCheck (MqServiceGetFilter(ctx, 0, &ftr));
  MqSendSTART(ftr);
  MqErrorCheck (MqSendC(ftr, "F3"));
  MqErrorCheck (MqSendC(ftr, ftr->config.name));
  MqErrorCheck (MqSendI(ftr, ftr->config.startAs));
  MqErrorCheck (MqSendI(ftr, ctx->statusIs));
  while (MqReadItemExists(ctx)) {
    MqErrorCheck (MqReadC(ctx, &str));
    MqErrorCheck (MqSendC(ftr, str));
  }
  MqErrorCheck (MqSendEND_AND_WAIT (ftr, "+FTR", MQ_TIMEOUT_USER));
error:
  return MqSendRETURN (ctx);
}

enum MqErrorE
F3Setup (
  struct MqS * const ctx,
  MQ_PTR const data
) {
  MqErrorCheck (MqServiceCreate (ctx, "+FTR", F3, NULL, NULL));
  MqErrorCheck (MqServiceProxy (ctx, "+EOF", 0));
error:
  return MqErrorStack(ctx);
}

enum MqErrorE
F3New (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS* const item,
  struct MqS  ** contextP
) {
  struct MqS * ctx = MqContextCreate(0, tmpl);
  MqConfigSetSetup(ctx, MqLinkDefault, NULL, MqLinkDefault, NULL, NULL, NULL);
  MqConfigSetServerSetup(ctx, F3Setup, NULL, NULL, NULL);
  *contextP = ctx;
  return MQ_OK;
}

// Main ************************************************************************

int main (int argc, MQ_CST argv[]) 
{
  struct MqS * ctx;
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc-1, argv+1);

  // init libmsgque global data
  MqBufferLAppendC(MqInitCreate(), argv[0]);

  // add Factory
  MqFactoryAdd ("F1", F1New, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  MqFactoryAdd ("F2", F2New, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  MqFactoryAdd ("F3", F3New, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  // call the initial factory to initialize the "config"
  MqFactoryErrorPanic(MqFactoryCall(argv[1], NULL, &ctx));

  // call entry point
  MqErrorCheck(MqLinkCreate(ctx, &largv));

  // start event-loop and wait forever
  MqProcessEvent (ctx, MQ_TIMEOUT, MQ_WAIT_FOREVER);

error:
  MqExit(ctx);
}

