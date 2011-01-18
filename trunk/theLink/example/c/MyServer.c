/**
 *  \file       theLink/example/c/MyServer.c
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
#include "string.h"

static enum MqErrorE MyFirstService (struct MqS *ctx, MQ_PTR data) {
  MqSendSTART(ctx);
  MqSendC(ctx, "Hello World");
  return MqSendRETURN(ctx);
}
static enum MqErrorE ServerSetup (struct MqS *ctx, MQ_PTR data) {
  return MqServiceCreate(ctx,"HLWO", MyFirstService, NULL, NULL);
}
enum MqErrorE
MyServerFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS * const item,
  struct MqS **contextP
)
{ 
  struct MqS * const ctx = *contextP = MqContextCreate(0,tmpl);
  MqConfigSetServerSetup (ctx, ServerSetup, NULL, NULL, NULL);
  return MQ_OK;
}
int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx;
  MqFactoryErrorPanic (
    MqFactoryNew("MyServer", MyServerFactory, NULL, NULL, NULL, NULL, NULL, NULL, &ctx)
  );
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}

