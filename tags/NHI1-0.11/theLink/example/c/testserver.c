/**
 *  \file       theLink/example/c/testserver.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "string.h"
#include "msgque.h"
static enum MqErrorE  GTCX( struct MqS *ctx, MQ_PTR data) {
  MqErrorCheck(MqSendSTART(ctx));
  MqSendI(ctx, MqLinkGetCtxId(ctx));
  MqSendC(ctx, "+");
  if (MqLinkIsParent(ctx)) {
    MqSendI(ctx, -1);
  } else {
    MqSendI(ctx, MqLinkGetCtxId(MqLinkGetParent(ctx)));
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
  MqConfigSetServerSetup (ctx, ServerSetup, NULL, NULL, NULL);
  MqConfigSetName (ctx, "testserver");
  ctx->setup.Parent.fCreate = MqLinkDefault;
  ctx->setup.Child.fCreate = MqLinkDefault;
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}



