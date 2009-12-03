/**
 *  \file       theLink/example/c/testserver.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "string.h"
#include "msgque.h"
static enum MqErrorE  GTCX( struct MqS *ctx, MQ_PTR data) {
  MqErrorCheck(MqSendSTART(ctx));
  MqSendI(ctx, MqConfigGetCtxId(ctx));
  MqSendC(ctx, "+");
  if (MqConfigGetIsParent(ctx)) {
    MqSendI(ctx, -1);
  } else {
    MqSendI(ctx, MqConfigGetCtxId(MqConfigGetParent(ctx)));
  }
  MqSendC(ctx, "+");
  MqSendC(ctx, MqConfigGetName(ctx));
  MqSendC(ctx, ":");
error:
  return MqSendRETURN(ctx);
}

static enum MqErrorE ServerSetup (struct MqS *ctx, MQ_PTR data) {
  return MqServiceCreate(ctx,"GTCX", GTCX, NULL, NULL);
}

int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  MqConfigSetName (ctx, "testserver");
  MqConfigSetServerSetup (ctx, ServerSetup, NULL, NULL, NULL);
  MqConfigSetDefaultFactory (ctx);
  ctx->setup.Parent.fCreate = MqDefaultLinkCreate;
  ctx->setup.Child.fCreate = MqDefaultLinkCreate;
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
