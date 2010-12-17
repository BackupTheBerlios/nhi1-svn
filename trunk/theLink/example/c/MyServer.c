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
#include "string.h"
#include "msgque.h"
static enum MqErrorE MyFirstService (struct MqS *ctx, MQ_PTR data) {
  MqSendSTART(ctx);
  MqSendC(ctx, "Hello World");
  return MqSendRETURN(ctx);
}
static enum MqErrorE ServerSetup (struct MqS *ctx, MQ_PTR data) {
  return MqServiceCreate(ctx,"HLWO", MyFirstService, NULL, NULL);
}
int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  MqConfigSetServerSetup (ctx, ServerSetup, NULL, NULL, NULL);
  MqConfigSetName (ctx, "MyServer");
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
